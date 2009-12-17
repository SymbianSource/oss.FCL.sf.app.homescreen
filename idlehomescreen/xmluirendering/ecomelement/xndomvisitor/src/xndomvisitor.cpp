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
* Description:  Visitor class to modify DOM node element's data 
*
*/



// INCLUDE FILES
#include    "xndomvisitor.h"
#include    "xnliteral.h"
#include    "xndomlist.h"
#include    "xndomproperty.h"
#include    "xndomstringpool.h"
#include    <ecom/ecom.h>

// CONSTANTS
_LIT8(KNS, "inheritpropertyvisitor");

//Function declarations
LOCAL_C void SetPropertiesL( const TXnPropertyProxy aTable[], TInt aCount, TAny* aData );
LOCAL_C void SetCommonPropertiesL( TAny* aData );
LOCAL_C void SetElementWithTextPropertiesL( TAny* aData );
LOCAL_C void SetTextElementPropertiesL( TAny* aData );
LOCAL_C void SetGridAndListPropertiesL( TAny* aData );
LOCAL_C void SetDataGridAndGridPropertiesL( TAny* aData );
LOCAL_C void SetViewsAndViewPropertiesL( TAny* aData );
LOCAL_C void SetViewPropertiesL( TAny* aData );

const TXnElementVisitor KXnElementVisitorTable[]=
    {
        { (const void*)&KProperty,          NULL,                           EEmpty},
        { (const void*)&KXmluiml,           NULL,                           EEmpty},
        { (const void*)&KInclude,           NULL,                           EEmpty},
        { (const void*)&KViews,             NULL,                           EEmpty},
        { (const void*)&KView,              &SetViewPropertiesL,            EEmpty},
        { (const void*)&KPanes,             &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KStatusPane,        &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KMainPane,          &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KControlPane,       &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KDialogs,           &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KDialog,            &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KNote,              &SetTextElementPropertiesL,     EEmpty},
        { (const void*)&KUiDefinition,      NULL,                           EEmpty},
        { (const void*)&KApplication,       NULL,                           EEmpty},
        { (const void*)&KDesc,              NULL,                           EPCData},
        { (const void*)&KButton,            &SetTextElementPropertiesL,     EEmpty},
        { (const void*)&KGrid,              &SetDataGridAndGridPropertiesL, EEmpty},
        { (const void*)&KListItem,          &SetElementWithTextPropertiesL, EEmpty},
        { (const void*)&KDataGrid,          &SetDataGridAndGridPropertiesL, EEmpty},
        { (const void*)&KGridCellTemplate,  &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KList,              &SetGridAndListPropertiesL,     EEmpty},
        { (const void*)&KDataList,          &SetGridAndListPropertiesL,     EEmpty},
        { (const void*)&KListRowTemplate,   &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KMenuBar,           NULL,                           EEmpty},
        { (const void*)&KMenu,              NULL,                           EEmpty},
        { (const void*)&KMenuItem,          NULL,                           EEmpty},
        { (const void*)&KText,              &SetTextElementPropertiesL,     EPCData},
        { (const void*)&KImage,             &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KEditor,            &SetElementWithTextPropertiesL, EEmpty},
	    { (const void*)&KMarquee,           &SetElementWithTextPropertiesL, EPCData},
	    { (const void*)&KNewsticker,        &SetElementWithTextPropertiesL, EPCData},

        { (const void*)&KObject,            NULL,          		            EPCData},
        { (const void*)&KParam,             NULL,          		            EEmpty},
	    { (const void*)&KTooltip,           &SetElementWithTextPropertiesL, EEmpty},
        
	    { (const void*)&KBox,               &SetCommonPropertiesL,          EEmpty},
        { (const void*)&KAction,            NULL,                           EEmpty},
        { (const void*)&KTrigger,           NULL,                           EEmpty},
        { (const void*)&KEvent,             NULL,                           EEmpty}
    };

// ============================ LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// SetPropertiesL Set properties which can inherit and doesn't exist yet.
// Returns: On return proeprty list contains properties which can inherit 
// -----------------------------------------------------------------------------
//
LOCAL_C void SetPropertiesL( 
    const TXnPropertyProxy aTable[], 
    TInt aItemCount, 
    TAny* aData )
    {
    CXnDomList* propertyList = reinterpret_cast<CXnDomList*>( aData );
    CXnDomStringPool& stringPool = propertyList->StringPool();
    
    for ( TInt i=0; i<aItemCount; i++ )
        {
        CXnDomProperty* property = NULL;
        const TDesC8* name = 
            reinterpret_cast<const TDesC8*>( aTable[i].iName );
        
        CXnDomProperty* foundProperty = 
            static_cast<CXnDomProperty*>( propertyList->FindByName( *name ) );
        if (  aTable[i].iInherit )
            {
            if (!foundProperty)
                {
                property = CXnDomProperty::NewL( *name, stringPool );
                property->SetInherited( aTable[i].iInherit );
                CleanupStack::PushL( property );
                propertyList->AddItemL( property );
                CleanupStack::Pop( property );
                }
            else
                {
                foundProperty->SetInherited( aTable[i].iInherit );
                }    
            }
        }
    }
    
LOCAL_C void SetCommonPropertiesL( TAny* aData )
    {
    TInt count( sizeof (KXnCommonPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnCommonPropertyTable, count, aData );
    }

LOCAL_C void SetElementWithTextPropertiesL( TAny* aData )
    {
    SetCommonPropertiesL( aData );
    TInt count( sizeof (KXnElementWithTextPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnElementWithTextPropertyTable, count, aData );
    }    

LOCAL_C void SetTextElementPropertiesL( TAny* aData )
    {
    SetElementWithTextPropertiesL( aData );
    TInt count( sizeof (KXnTextElementPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnTextElementPropertyTable, count, aData );
    }

LOCAL_C void SetGridAndListPropertiesL( TAny* aData )
    {
    SetCommonPropertiesL( aData );
    TInt count( sizeof (KXnGridAndListPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnGridAndListPropertyTable, count, aData );
    }

LOCAL_C void SetDataGridAndGridPropertiesL( TAny* aData )
    {
    SetGridAndListPropertiesL( aData );
    TInt count( sizeof (KXnDataGridAndGridPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnDataGridAndGridPropertyTable, count, aData );
    }
        

LOCAL_C void SetViewsAndViewPropertiesL( TAny* aData )
    {
    SetCommonPropertiesL( aData );
    TInt count( sizeof (KXnViewsAndViewPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnViewsAndViewPropertyTable, count, aData );
    }
    
LOCAL_C void SetViewPropertiesL( TAny* aData )
    {
    SetViewsAndViewPropertiesL( aData );
    TInt count( sizeof (KXnViewPropertyTable) / sizeof(TXnPropertyProxy) );
    SetPropertiesL( KXnViewPropertyTable, count, aData );
    }


    
        
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDomVisitor::NewL
// -----------------------------------------------------------------------------    
MXnDomVisitor* CXnDomVisitor::NewL()
    {
    return new (ELeave) CXnDomVisitor();
    }    

// -----------------------------------------------------------------------------
// CXnDomVisitor::CXnDomVisitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomVisitor::CXnDomVisitor()
    {
    }


// Destructor
CXnDomVisitor::~CXnDomVisitor()
    {
    REComSession::DestroyedImplementation( iDtorKey );
    }

// -----------------------------------------------------------------------------
// CXnDomVisitor::IsVersion
// -----------------------------------------------------------------------------    
TBool CXnDomVisitor::IsVersion( const TDesC8& aVersion )
    {
    return (KNS().Compare( aVersion ) == 0);
    }

// -----------------------------------------------------------------------------
// CXnDomVisitor::ModifyDataL
// -----------------------------------------------------------------------------    
void CXnDomVisitor::ModifyDataL( CXnDomNode& aNode )
    {
    TInt count( sizeof(KXnElementVisitorTable) / sizeof( TXnElementVisitor ) );

    TBool found = EFalse;    
    for ( TInt i=0; i<count && !found; i++ )   
        {
        const TDesC8* name = 
            reinterpret_cast<const TDesC8*>( KXnElementVisitorTable[i].iName );
            
        if ( name && name->Compare( aNode.Name() ) == 0)
            {
            if ( KXnElementVisitorTable[i].iFuncPtr )
                {
                KXnElementVisitorTable[i].iFuncPtr( &aNode.PropertyList() );
                }
            found = ETrue;   
            }    
        }
    }
    
// -----------------------------------------------------------------------------
// CXnDomVisitor::SetElementTypeL
// -----------------------------------------------------------------------------    
void CXnDomVisitor::SetElementTypeL( CXnDomNode& aNode )
    {
    TInt count( sizeof(KXnElementVisitorTable) / sizeof( TXnElementVisitor ) );

    TBool found = EFalse;    
    for ( TInt i=0; i<count && !found; i++ )   
        {
        const TDesC8* name = 
            reinterpret_cast<const TDesC8*>( KXnElementVisitorTable[i].iName );
            
        if ( name && name->Compare( aNode.Name() ) == 0)
            {
            aNode.SetContentType(KXnElementVisitorTable[i].iContentType);
            found = ETrue;   
            }    
        }
    }
// -----------------------------------------------------------------------------
// CXnDomVisitor::CreateElementL
// -----------------------------------------------------------------------------                               
void CXnDomVisitor::SetDtorKey( const TUid aDtorKey )
    {
    iDtorKey = aDtorKey;
    }

// -----------------------------------------------------------------------------
// CXnDomVisitor::Release
// -----------------------------------------------------------------------------                               
void CXnDomVisitor::Release()
    {
    delete this;
    }

//  End of File  
