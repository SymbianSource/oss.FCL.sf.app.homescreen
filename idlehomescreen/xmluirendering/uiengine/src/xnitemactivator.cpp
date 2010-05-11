/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Item activator
*
*/

// System includes

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xneditor.h"
#include "xndomstringpool.h"
#include "xndomdocument.h"
#include "xnproperty.h"
#include "xnnodeimpl.h"
#include "xnnode.h"
#include "xntype.h"
#include "xnodt.h"

#include "xnitemactivator.h"

// Constants
_LIT8( KMenuItem, "menuitem" );
_LIT8( KDynMenuItem, "dynmenuitem" );
_LIT8( KWidgetMenuItem, "widgetmenuitem" );
    
// ============================ LOCAL FUNCTIONS ================================
// -----------------------------------------------------------------------------
// BuildActivateTriggerNodeL
//
// -----------------------------------------------------------------------------
//
static CXnNode* BuildActivateTriggerNodeL( CXnUiEngine& aUiEngine )
    {
    CXnNode* node = CXnNode::NewL();
    CleanupStack::PushL( node );
    CXnType* type = CXnType::NewL( XnPropertyNames::action::KTrigger );
    CleanupStack::PushL( type );
    CXnNodeImpl* impl = CXnNodeImpl::NewL( type );
    CleanupStack::Pop( type );
    node->SetImpl( impl );
    node->SetUiEngine( aUiEngine );
    CXnDomPropertyValue* nameValue = CXnDomPropertyValue::NewL(
        aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::PushL( nameValue );
    nameValue->SetStringValueL( CXnDomPropertyValue::EString,
        XnPropertyNames::action::trigger::name::KActivate );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( name );
    CleanupStack::Pop( node );
    return node;
    }

// -----------------------------------------------------------------------------
// IsMenuItem
// 
// -----------------------------------------------------------------------------
//
static TBool IsMenuItem( CXnNode& aItem )
    {
    const TDesC8& type( aItem.Type()->Type() );
    
    return ( type == KMenuItem || 
             type == KDynMenuItem || 
             type == KWidgetMenuItem );
    }

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnItemActivator::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnItemActivator* CXnItemActivator::NewL( CXnAppUiAdapter& aAppUiAdapter ) 
    {
    CXnItemActivator* self = CXnItemActivator::NewLC( aAppUiAdapter );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnItemActivator* CXnItemActivator::NewLC( CXnAppUiAdapter& aAppUiAdapter )
    {
    CXnItemActivator* self = new ( ELeave ) CXnItemActivator( aAppUiAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::~CXnItemActivator()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnItemActivator::~CXnItemActivator()
    {    
    Cancel();
    
    delete iTrigger;
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::CXnItemActivator()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnItemActivator::CXnItemActivator( CXnAppUiAdapter& aAppUiAdapter )
    : CTimer( CActive::EPriorityUserInput + 1 ), iAppUiAdapter( aAppUiAdapter )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnItemActivator::ConstructL()
    {             
    CTimer::ConstructL();

    iTrigger = BuildActivateTriggerNodeL( iAppUiAdapter.UiEngine() );
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::RunL()
// 
// -----------------------------------------------------------------------------
//
void CXnItemActivator::RunL()
    {   
    CXnNode* item( iItemToActivate );
    
    iItemToActivate = NULL;
    
    if ( item && iAppUiAdapter.IsForeground() )
        {       
        iAppUiAdapter.UiEngine().DisableRenderUiLC();
        
        item->ReportXuikonEventL( *iTrigger );
                       
        CleanupStack::PopAndDestroy(); // DisableRenderUiLC
        }          
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::RunError()
// 
// -----------------------------------------------------------------------------
//
TInt CXnItemActivator::RunError( TInt /*aError*/ )
    {
    // Ignore
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::DoCancel()
// 
// -----------------------------------------------------------------------------
//
void CXnItemActivator::DoCancel()
    {
    iItemToActivate = NULL;
    }

// -----------------------------------------------------------------------------
// CXnItemActivator::Activate()
// 
// -----------------------------------------------------------------------------
//
void CXnItemActivator::Activate( CXnNode* aItemToActivate )
    {
    if ( aItemToActivate )
        {
        Cancel();
        
        iItemToActivate = aItemToActivate;
        
        if ( IsMenuItem( *aItemToActivate ) )
            {
            TRAP_IGNORE( RunL() );
            }
        else
            {                              
            After( 0 );            
            }               
        }
    }

// End of file
