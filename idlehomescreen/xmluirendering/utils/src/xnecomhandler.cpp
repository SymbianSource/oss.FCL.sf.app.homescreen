/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Handles ecom plugins
*
*/



// INCLUDE FILES
#include    "xnecomhandler.h"
#include    "xnpanic.h"
#include    "xnarray.h"
#include    "xnpointerarraywithownership.h"
#include    <utf.h>
#include    <ecom/ecom.h>
#include    <ecom/implementationinformation.h>
#include    <f32file.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def
// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

/**
*  A struct used for holding an implementation pointer and notifying ecom of implementation destruction
*  @since Series 90 2.0
*/
struct CXnEcomHandler::CEcomItem : public CBase
    {
    CEcomItem(TUid aInterfaceUid, CBase* aPluginImpl, TUid aDestructorId) : iInterfaceUid(aInterfaceUid), iPluginImpl(aPluginImpl), iDestructorId(aDestructorId) 
        {
        }
    // plugin interface id
    TUid iInterfaceUid;
    // plugin implementation pointer
    CBase* iPluginImpl;
    // ecom destructor id
    TUid iDestructorId;
    // plugin type
    HBufC8* iType;
    // destructor
    ~CEcomItem()
        {
        delete iPluginImpl;
        delete iType;
        REComSession::DestroyedImplementation(iDestructorId);
        }
    };

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
//?type ?function_name(
//    ?arg_type arg,  // ?description
//    ?arg_type arg)  // ?description
//    {

//    ?code  // ?comment

    // ?comment
//    ?code
//    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnEcomHandler::CXnEcomHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnEcomHandler::CXnEcomHandler()
    {
    }

// -----------------------------------------------------------------------------
// CXnEcomHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnEcomHandler::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CXnEcomHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnEcomHandler* CXnEcomHandler::NewL()
    {
    CXnEcomHandler* self = new( ELeave ) CXnEcomHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CXnEcomHandler::~CXnEcomHandler()
    {
    iPlugins.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CXnEcomHandler::PluginsL
// Get pointers to plugins
// -----------------------------------------------------------------------------
EXPORT_C void CXnEcomHandler::PluginsL(TUid aInterfaceUid, const TDesC8& aType, CXnArray& aFunctions)
    {
    TBool cached = EFalse;
    for (TInt i = iPlugins.Count() - 1 ; i >= 0; --i)
        {
        CEcomItem* tmp = iPlugins[i];
        if (tmp->iInterfaceUid == aInterfaceUid
        && *(tmp->iType) == aType)
            {
            cached = ETrue;
            if(aFunctions.Container().Append(tmp->iPluginImpl) != KErrNone)
                {
                User::Leave(KXnErrAppendingPluginFailed);
                }
            }
        }
    if (cached)
        {
        return;
        }
    HBufC8* defaultData = aType.AllocL();
    CleanupStack::PushL(defaultData);
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(*defaultData);
    RImplInfoPtrArray implInfoArray;
    CXnPointerArrayWithOwnership* safeArray = CXnPointerArrayWithOwnership::NewL();
    CleanupStack::PushL(safeArray);
    REComSession::ListImplementationsL(aInterfaceUid, resolverParams, implInfoArray);        
//    REComSession::ListImplementationsL(aInterfaceUid, implInfoArray);        
    TInt count = implInfoArray.Count();
    for (TInt i = implInfoArray.Count() - 1; i >= 0; --i)
        {
        CImplementationInformation* impl = implInfoArray[i];
        (safeArray->Container()).Insert(impl, 0); 
        }
    if ((safeArray->Container()).Count() != count)
        {
        (safeArray->Container()).Reset();
        implInfoArray.ResetAndDestroy();
        User::Leave(KErrNoMemory);
        }
    implInfoArray.Reset();        
    for (TInt i = safeArray->Container().Count() - 1; i >= 0; --i)
        {
        CImplementationInformation* impl = static_cast<CImplementationInformation*>(safeArray->Container()[i]);
        TUid dtorIdKey;
        CBase* plugin = (CBase*) REComSession::CreateImplementationL(impl->ImplementationUid(), dtorIdKey);
        CleanupStack::PushL(plugin);
        CEcomItem* tmp = new (ELeave) CEcomItem(aInterfaceUid, plugin, dtorIdKey);
        CleanupStack::PushL(tmp);
        tmp->iType = aType.AllocL();
        //tmp->iType = impl->DataType().AllocL();
        if(iPlugins.Append(tmp) != KErrNone)
            {
            User::Leave(KXnErrAppendingPluginFailedAgain);
            }
        CleanupStack::Pop(tmp);
        CleanupStack::Pop(plugin);        
        }
    for (TInt i = iPlugins.Count() - 1 ; i >= 0; --i)
        {
        CEcomItem* tmp = iPlugins[i];
        if (tmp->iInterfaceUid == aInterfaceUid
        && *(tmp->iType) == aType)
            {
            cached = ETrue;
            if(aFunctions.Container().Append(tmp->iPluginImpl) != KErrNone)
                {
                User::Leave(KXnErrAppendingPluginImplFailed);
                }
            }
        }
    CleanupStack::PopAndDestroy(safeArray);        
    CleanupStack::PopAndDestroy(defaultData);
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
//?type ?classname::?member_function(
//    ?arg_type arg,
//    ?arg_type arg )
//    {
    
//    ?code
    
//    }


	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
//?type  ?function_name(
//    ?arg_type arg,  // ?description
//    ?arg_type arg )  // ?description
//    {

//    ?code

//    }


//  End of File  
