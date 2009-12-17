/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Loads the browser plugin via ECOM.
*
*/



// INCLUDE FILES
#include    "xnnppluginhandler.h"
#include    <ecom/implementationinformation.h>
#include    <CEcomBrowserPluginInterface.h>

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ParseAndMatchMimeTypeL Parses the matching mime types from the.given string.
// Returns: TDesC8&: String that holds the mime types.
//          TDesC8&: Mime type to match with.
// -----------------------------------------------------------------------------
//
TBool ParseAndMatchMimeTypeL(
    const TDesC8& aMIMEDescription,
    const TDesC8& aMatch )
    {
    _LIT(KPatternPtr, ";|");

    TUint    end = aMIMEDescription.Length();
    TUint    i = 0;
    TUint    marker = 0;
    TUint    mimeSeparator = '|';

    // Parse the aMIMEDescription string.
    for (; i < end;)
        {
        // Search until end of buffer or match one of the delimiters ';' or '|'.
        // We are looking for the mimeType, ie "text/html", "application/pdf", etc.
        for (; (i < end) && (KPatternPtr().Locate((aMIMEDescription)[i]) == 
                             KErrNotFound); i++)
            {
            // Walking the aMIMEDescription string
            }

        if (i > marker)
            {
            // Try matching the found mime type with the given type.
            TPtrC8 newString( aMIMEDescription.Mid(marker, i - marker ).Ptr(), i - marker );
            if( newString == aMatch )
                {
                return ETrue;
                }
            }

        // Are we at the end of the supported mime string
        if (i == end)
            {
            // This break with i=end means we leave outer for loop
            break;
            }

        marker = ++i;
        if ((aMIMEDescription)[i - 1] == mimeSeparator)
            {
            // Found a mime separator '|', get next supported mime
            continue;
            }        
        }   // end of for
        return EFalse;
    }

// -----------------------------------------------------------------------------
// PluginWrapperGetUrl
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperGetUrl(NPP /*aInstance*/, const TDesC& /*aUrl*/, 
        const TDesC* /*aWindowType*/ )
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperPostUrl
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperPostUrl(NPP /*aInstance*/, const TDesC& /*aUrl*/, 
        const TDesC* /*aWindowType*/, const TDesC& /*aBuf*/, NPBool /*aFile*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperRequestRead
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperRequestRead(NPStream* /*aStream*/, 
        NPByteRange* /*aRangeList*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperNewStream
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperNewStream(NPP /*aInstance*/, NPMIMEType /*aType*/, 
        const TDesC* /*aWindowType*/, NPStream** /*aStream*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperWrite
// Unsupported.
// -----------------------------------------------------------------------------
//
static TInt32 PluginWrapperWrite(NPP /*aInstance*/, NPStream* /*aStream*/, 
        TInt32 /*aLen*/, void* /*aBuffer*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperDestroyStream
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperDestroyStream(NPP /*aInstance*/, 
        NPStream* /*aStream*/, NPReason /*aReason*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperStatus
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperStatus(NPP /*aInstance*/, const TDesC& /*aMessage*/)
    {
    }

// -----------------------------------------------------------------------------
// PluginWrapperUAgent
// Unsupported.
// -----------------------------------------------------------------------------
//
static const TDesC* PluginWrapperUAgent(NPP /*aInstance*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// PluginWrapperMemAlloc
// Unsupported.
// -----------------------------------------------------------------------------
//
static void* PluginWrapperMemAlloc( uint32 /*aSize*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// PluginWrapperMemFree
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperMemFree( void* aPtr )
    {
    delete aPtr;
    }

// -----------------------------------------------------------------------------
// PluginWrapperMemFlush
// Unsupported.
// -----------------------------------------------------------------------------
//
static uint32 PluginWrapperMemFlush(uint32 /*aSize*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperReloadPlugins
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperReloadPlugins(NPBool /*aReloadPages*/)
    {
    }

// -----------------------------------------------------------------------------
// PluginWrapperGetJavaEnv
// Unsupported.
// -----------------------------------------------------------------------------
//
static JRIEnv* PluginWrapperGetJavaEnv()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// PluginWrapperGetJavaPeer
// Unsupported.
// -----------------------------------------------------------------------------
//
static jref PluginWrapperGetJavaPeer(NPP /*aInstance*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// PluginWrapperGetUrlNotify
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperGetUrlNotify(NPP /*aInstance*/, const TDesC& /*aUrl*/, 
        const TDesC* /*aWindowType*/, void* /*aNotifyData*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperPostUrlNotify
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperPostUrlNotify(NPP /*aInstance*/, const TDesC& /*aUr*l*/, 
        const TDesC* /*aWindowType*/, const TDesC& /*aBuf*/, NPBool /*aFile*/, void* /*aNotifyData*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperGetValue
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperGetValue(NPP /*aInstance*/, NPNVariable /*aVariable*/, 
        void* /*aRetValue*/)
    {
    return NPERR_NO_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperSetValue
// Unsupported.
// -----------------------------------------------------------------------------
//
static NPError PluginWrapperSetValue(NPP /*aInstance*/, NPPVariable /*aVariable*/, 
        void * /*aRetValue*/)
    {
    return NPERR_GENERIC_ERROR;
    }

// -----------------------------------------------------------------------------
// PluginWrapperInvalidateRect
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperInvalidateRect(NPP /*aInstance*/, NPRect * /*aRect*/)
    {
    }

// -----------------------------------------------------------------------------
// PluginWrapperInvalidateRegion
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperInvalidateRegion(NPP /*aInstance*/, NPRegion /*aRegion*/)
    {
    }

// -----------------------------------------------------------------------------
// PluginWrapperForceRedraw
// Unsupported.
// -----------------------------------------------------------------------------
//
static void PluginWrapperForceRedraw(NPP /*aInstance*/)
    {
    }
    
const NPNetscapeFuncs PluginWrapperFuncs =
    {
    sizeof(NPNetscapeFuncs),
    1,
    PluginWrapperGetUrl,
    PluginWrapperPostUrl,
    PluginWrapperRequestRead,
    PluginWrapperNewStream,
    PluginWrapperWrite,
    PluginWrapperDestroyStream,
    PluginWrapperStatus,
    PluginWrapperUAgent,
    PluginWrapperMemAlloc,
    PluginWrapperMemFree,
    PluginWrapperMemFlush,
    PluginWrapperReloadPlugins,
    PluginWrapperGetJavaEnv,
    PluginWrapperGetJavaPeer,
    PluginWrapperGetUrlNotify,
    PluginWrapperPostUrlNotify,
    PluginWrapperGetValue,
    PluginWrapperSetValue,
    PluginWrapperInvalidateRect,
    PluginWrapperInvalidateRegion,
    PluginWrapperForceRedraw
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNPPluginHandler::CXnNPPluginHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnNPPluginHandler::CXnNPPluginHandler()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPluginHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNPPluginHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPluginHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnNPPluginHandler* CXnNPPluginHandler::NewL()
    {
    CXnNPPluginHandler* self = new( ELeave ) CXnNPPluginHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CXnNPPluginHandler::~CXnNPPluginHandler()
    {
    delete iPluginInterface;
    }

// -----------------------------------------------------------------------------
// CXnNPPluginHandler::LoadSpecificPluginL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CXnNPPluginHandler::LoadSpecificPluginL(
    const TDesC8& aResolveparam,
    NPPluginFuncs** aPluginFuncs )
    {
    RImplInfoPtrArray implInfoArray;
    CImplementationInformation* impl = NULL;
    REComSession::ListImplementationsL( KBrowserPluginInterfaceUid, implInfoArray );     
    TBool implFound( EFalse );
    TInt count = implInfoArray.Count();
    for(TInt index = 0; index < count; ++index )
        {
        impl = implInfoArray[index];
        if( ParseAndMatchMimeTypeL( impl->DataType(), aResolveparam ) )
            {
            implFound = ETrue;
            break;
            }
        }

    if( !implFound )
        {
        implInfoArray.ResetAndDestroy();
        implInfoArray.Close();
        return KErrNotFound;
        }
        
    NPPluginFuncs* pluginFuncs = (NPPluginFuncs*) User::AllocLC(sizeof (NPPluginFuncs));
    Mem::FillZ((void*) pluginFuncs, sizeof (NPPluginFuncs));

    iPluginInterface = CEcomBrowserPluginInterface::CreatePluginL( 
         impl->ImplementationUid(),(NPNetscapeFuncs *) (&PluginWrapperFuncs), pluginFuncs );

    CleanupStack::Pop(pluginFuncs);

    *aPluginFuncs = pluginFuncs;
    pluginFuncs = NULL;
    
    implInfoArray.ResetAndDestroy();
    implInfoArray.Close();
    return KErrNone;
    }

//  End of File  
