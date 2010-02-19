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
* Description:  Xuikon view adapter source file
*
*/

// System includes
#include <aknViewAppUi.h>
#include <eikbtgpc.h>

// User includes
#include "xnappuiadapter.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xnuiengine.h"

#include "xnkeyeventdispatcher.h"
#include "xncontroladapter.h"

#include "xndomdocument.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xnodt.h"
#include "xnproperty.h"
#include "xnnodeimpl.h"
#include "xnnode.h"
#include "xntype.h"
#include "xnbgcontrol.h"
#include "xnfocuscontrol.h"
#include "xneditor.h"
#include "xnbackgroundmanager.h"

#include "xnviewadapter.h"
#include "xnmenu.h"
#include "xneditmode.h"

// Constants
const TUid KXmlViewUid = { 1 };

// Data types
enum 
    {
    EIsActivated,
    EIsInCall,
    EIsLightsOn,
    EIsForeground,    
    EIsControlsAwake,    
    EIsDestructionRunning
    };

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// BuildTriggerL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildTriggerL(
    CXnUiEngine& aUiEngine,
    const TDesC8& aTriggerName )
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
    nameValue->SetStringValueL( CXnDomPropertyValue::EString, aTriggerName );
    CXnProperty* name = CXnProperty::NewL(
        XnPropertyNames::action::trigger::KName,
        nameValue,
        *aUiEngine.ODT()->DomDocument().StringPool() );
    CleanupStack::Pop( nameValue );
    CleanupStack::PushL( name );
    node->SetPropertyL( name );
    CleanupStack::Pop( 2, node ); // name 
    
    return node;
    }

// -----------------------------------------------------------------------------
// BuildActivateTriggerL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildActivateTriggerL( CXnUiEngine& aUiEngine )
    {
    return BuildTriggerL(
        aUiEngine, XnPropertyNames::action::trigger::name::KViewActivate );
    }

// -----------------------------------------------------------------------------
// BuildDeactivateTriggerL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildDeactivateTriggerL( CXnUiEngine& aUiEngine )
    {
    return BuildTriggerL(
        aUiEngine, XnPropertyNames::action::trigger::name::KViewDeactivate );
    }

// -----------------------------------------------------------------------------
// BuildEditStateTriggerL
// -----------------------------------------------------------------------------
//
static CXnNode* BuildEditStateTriggerL( CXnUiEngine& aUiEngine )
    {
    CXnDomStringPool* sp( aUiEngine.ODT()->DomDocument().StringPool() );
    
    CXnProperty* value = CXnProperty::NewL(
        XnPropertyNames::action::KValue,
        KNullDesC8, CXnDomPropertyValue::EString, *sp );
    CleanupStack::PushL( value );
    
    CXnNode* trigger( BuildTriggerL(
        aUiEngine, XnPropertyNames::action::trigger::name::KEditMode ) );    
    CleanupStack::PushL( trigger );
    
    trigger->SetPropertyL( value );
    CleanupStack::Pop( 2, value ); // trigger
    
    return trigger;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewAdapter::CXnViewAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnViewAdapter::CXnViewAdapter( CXnAppUiAdapter& aAdapter )
    : iAppUiAdapter( aAdapter )
    {
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::~CXnViewAdapter
// Destructor.
// -----------------------------------------------------------------------------
//
CXnViewAdapter::~CXnViewAdapter()
    {           
    delete iActivate;
    delete iDeactivate;
    delete iEditState;
    delete iBgControl;
    delete iBgManager;
    delete iFocusControl;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::NewL
// 2nd phase construction.
// -----------------------------------------------------------------------------
//
CXnViewAdapter* CXnViewAdapter::NewL( CXnAppUiAdapter& aAdapter )
    {
    CXnViewAdapter* self = new ( ELeave ) CXnViewAdapter( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::ConstructL
// 2nd phase construction.
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::ConstructL()
    {
    BaseConstructL();
    
    // Base class CAknViewAppUi takes ownership of iViewAdapter
    iAppUiAdapter.AddViewL( this );    
    iAppUiAdapter.SetDefaultViewL( *this );

    iBgControl = CXnBgControl::NewL();
    iBgControl->SetMopParent( this );

    iBgManager = CXnBackgroundManager::NewL( iAppUiAdapter.ViewManager(),
        iAppUiAdapter.ViewManager().Editor().HspsWrapper() );

    iFocusControl = CXnFocusControl::NewL( iAppUiAdapter );   
    
    iEventDispatcher = 
        CXnKeyEventDispatcher::NewL( iAppUiAdapter.UiEngine() );
    
    iAppUiAdapter.UiEngine().SetEventDispatcher( iEventDispatcher );
    
    iAppUiAdapter.UiStateListener().AddObserver( *this );
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::ReloadUiL
// Called when application UI is reloaded
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::ReloadUiL()
    {
    DeactivateContainerL();
    
    iAppUiAdapter.RemoveFromStack( iEventDispatcher );
    
    delete iEventDispatcher;
    iEventDispatcher = NULL;
    
    delete iActivate;
    iActivate = NULL;
    
    delete iDeactivate;
    iDeactivate = NULL;
    
    delete iEditState;
    iEditState = NULL;
        
    iEventDispatcher = CXnKeyEventDispatcher::NewL( iAppUiAdapter.UiEngine() );
    
    iAppUiAdapter.UiEngine().SetEventDispatcher( iEventDispatcher );
    
    if ( iFlags.IsSet( EIsActivated ) )
        {
        iAppUiAdapter.AddToStackL( *this, iEventDispatcher );
        }
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::PrepareDestroy
// Sets view to be destroyed
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::PrepareDestroy()
    {
    iAppUiAdapter.UiStateListener().RemoveObserver( *this );
    iBgControl->PrepareDestroy();
    
    TRAP_IGNORE( DeactivateContainerL() );
    
    iAppUiAdapter.RemoveFromStack( iEventDispatcher );
    delete iEventDispatcher;
    iEventDispatcher = NULL;
    
    iContainer = NULL;
    
    iFlags.Set( EIsDestructionRunning );    
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::EventDispatcher
// Get event dispatcher
// -----------------------------------------------------------------------------
//
CXnKeyEventDispatcher* CXnViewAdapter::EventDispatcher() const
    {
    return iEventDispatcher;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::BgControl
// Returns bg control.
// -----------------------------------------------------------------------------
//
CCoeControl& CXnViewAdapter::BgControl() const
    {
    return *iBgControl;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::BgManager
// Returns background manager.
// -----------------------------------------------------------------------------
//
CXnBackgroundManager& CXnViewAdapter::BgManager() const
    {
    return *iBgManager;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::BgControl
// Returns focus control.
// -----------------------------------------------------------------------------
//
CXnFocusControl& CXnViewAdapter::FocusControl() const
    {
    return *iFocusControl;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::Id
// Returns view uid.
// -----------------------------------------------------------------------------
//
TUid CXnViewAdapter::Id() const
    {
    return KXmlViewUid;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::DoActivateL
// Activates view.
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::DoActivateL( const TVwsViewId& /*aPrevViewId*/,    
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if ( iFlags.IsSet( EIsDestructionRunning ) )
        {
        return;
        }
    
    iFlags.Set( EIsActivated );
    
    iAppUiAdapter.AddToStackL( *this, iEventDispatcher );

    CEikButtonGroupContainer* bgc( iAppUiAdapter.Cba() );
    
    if ( bgc )
        {
        // EventDispatcher will handle sotkey keyevents 
        CEikCba* cba( 
            static_cast< CEikCba* >( bgc->ButtonGroup() ) );

        iAppUiAdapter.RemoveFromStack( cba );        
        }
    
    iBgControl->MakeVisible( ETrue );
    iBgManager->MakeVisible( ETrue );
    
    // Set the active container
    ActivateContainerL( iAppUiAdapter.ViewManager().ActiveViewData() );             
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::DoDeactivate
// Deactivates view.
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::DoDeactivate()
    {        
    if ( iFlags.IsSet( EIsDestructionRunning ) )
        {
        return;
        }
    
    iAppUiAdapter.RemoveFromStack( iEventDispatcher );

    TRAP_IGNORE( DeactivateContainerL() );
    
    iBgControl->MakeVisible( EFalse );
    iBgManager->MakeVisible( EFalse );
    
    iFocusControl->MakeVisible( EFalse );
    
    iFlags.Clear( EIsActivated );
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::ActivateContainerL
// Activates container
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::ActivateContainerL( CXnViewData& aContainer, 
    TBool aEnterEditState )
    {                       
    if ( iContainer == &aContainer )    
        {            
        return;
        }

    // Deactivate previous
    DeactivateContainerL();
    
    if ( iFlags.IsClear( EIsActivated ) )
        {
        // Some other view than this in this appui is currently active,
        // postpone container activation
        return;
        }
    
    // Update
    iContainer = &aContainer;
        
    // Disable layout and redraw until container activation is done
    iAppUiAdapter.UiEngine().DisableRenderUiLC();

    // Try exit controls from powersave mode
    ChangeControlsStateL( ETrue );
    
    if ( !iActivate )
        {
        iActivate = BuildActivateTriggerL( iAppUiAdapter.UiEngine() );
        }

    CXnNode* node( aContainer.Node()->LayoutNode() );
    
    node->ReportXuikonEventL( *iActivate );
                           
    if ( iFlags.IsSet( EIsInCall ) )
        {
        iFlags.Clear( EIsInCall );
        
        // This container is in-call state
        NotifyInCallStateChaged( ETrue );        
        }
                 
    iAppUiAdapter.ViewManager().NotifyContainerChangedL( aContainer );
    
    if ( !iEditState )
        {
        iEditState = BuildEditStateTriggerL( iAppUiAdapter.UiEngine() ); 
        }
    
    CXnProperty* prop( iEditState->GetPropertyL( 
        XnPropertyNames::action::KValue ) );
    
    if ( aEnterEditState || iAppUiAdapter.UiEngine().IsEditMode() )
        {
        static_cast< CXnDomPropertyValue* >(
            prop->Property()->PropertyValueList().Item( 0 ) )
            ->SetStringValueL( CXnDomPropertyValue::EString,
            XnPropertyNames::action::trigger::name::editmode::KEnter() );                        
        }
    else
        {
        static_cast< CXnDomPropertyValue* >(
            prop->Property()->PropertyValueList().Item( 0 ) )
            ->SetStringValueL( CXnDomPropertyValue::EString,
            XnPropertyNames::action::trigger::name::editmode::KExit() );                                
        }
    
    node->ReportXuikonEventL( *iEditState );
    
    CXnControlAdapter* adapter( node->Control() );
    
    iBgControl->SetCompoundControl( adapter );
            
    iAppUiAdapter.UiEngine().RenderUIL();
    
    CleanupStack::PopAndDestroy(); // DisableRenderUiLC        
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::DeactivateContainerL
// Deactivates current container
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::DeactivateContainerL()
    {
    if ( !iContainer )    
        {
        return;
        }
    
    // Run controls to powersave mode
    ChangeControlsStateL( EFalse );

    if ( !iDeactivate )
        {
        iDeactivate = BuildDeactivateTriggerL( iAppUiAdapter.UiEngine() );
        }

    CXnNode* node( iContainer->Node()->LayoutNode() );
                
    node->ReportXuikonEventL( *iDeactivate );

    iBgControl->SetCompoundControl( NULL );
    
    iContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::NotifyForegroundChanged
// 
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::NotifyForegroundChanged( 
    MXnUiStateObserver::TForegroundStatus aStatus ) 
    {     
    if ( aStatus == MXnUiStateObserver::EPartialForeground || 
         aStatus == MXnUiStateObserver::EBackground )
        {
        iFlags.Clear( EIsForeground );

        CXnEditMode* editMode = iAppUiAdapter.UiEngine().EditMode();
        if( editMode )
            {
            TRAP_IGNORE( editMode->StopDraggingL() );
            }

        iFocusControl->MakeVisible( EFalse );
        
        TRAP_IGNORE( ChangeControlsStateL( EFalse ) );
        }
    else
        {
        iFlags.Set( EIsForeground );
        
        TRAP_IGNORE( ChangeControlsStateL( ETrue ) );               
        }
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::NotifyLightStatusChanged
// 
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::NotifyLightStatusChanged( TBool aLightsOn )
    {       
    if ( aLightsOn )
        {
        iFlags.Set( EIsLightsOn );
        
        TRAP_IGNORE( ChangeControlsStateL( ETrue ) );        
        }
    else 
        {
        iFlags.Clear( EIsLightsOn );
        
        TRAP_IGNORE( ChangeControlsStateL( EFalse ) );        
        }        
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::NotifyInCallStateChaged
// 
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::NotifyInCallStateChaged( TBool aInCall )
    {       
    if ( !iContainer )
        {
        return;
        }
    
    TBool incallNow( iFlags.IsSet( EIsInCall ) ? ETrue : EFalse );
    
    if ( incallNow == aInCall )
        {
        return;
        }

    if ( aInCall )
        {
        iFlags.Set( EIsInCall );
        }
    else
        {
        iFlags.Clear( EIsInCall );
        }
    
    TRAP_IGNORE( UpdateRskByModeL() );
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::ChangeControlsStateL
// 
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::ChangeControlsStateL( TBool aAwake )
    {
    if ( !iContainer )
        {
        return;
        }

    TBool awakeNow( iFlags.IsSet( EIsControlsAwake ) ? ETrue : EFalse );
    
    if ( aAwake == awakeNow )
        {
        // No change in the state
        return;
        }

    TBool changeState( EFalse );
    
    if ( aAwake )
        {
        if ( iFlags.IsSet( EIsForeground ) && iFlags.IsSet( EIsLightsOn ) )
            {
            iFlags.Set( EIsControlsAwake );
            changeState = ETrue;
            }       
        }
    else
        {
        iFlags.Clear( EIsControlsAwake );
        changeState = ETrue;
        }
    
    if( changeState )
        {
        RPointerArray< CXnControlAdapter > controls;
        CleanupClosePushL( controls );
        
        iContainer->ControlsL( controls );
        
        for ( TInt i = 0; i < controls.Count(); i++ )
            {
            if ( aAwake )
                {
                controls[i]->ExitPowerSaveModeL();
                }
            else
                {
                controls[i]->EnterPowerSaveModeL();
                }               
            }
            
        CleanupStack::PopAndDestroy( &controls );            
        }           
    }

// -----------------------------------------------------------------------------
// CXnViewAdapter::UpdateRskByModeL()
// 
// -----------------------------------------------------------------------------
//
void CXnViewAdapter::UpdateRskByModeL()
    {
    CXnNode* menubar( iAppUiAdapter.UiEngine().MenuBarNode() );
    
    if( menubar )
        {
        const TDesC8* state( NULL );
        
        if( iAppUiAdapter.UiEngine().IsEditMode() )
            {
            state = &XnPropertyNames::softkey::mode::KModeEdit;
            }     
        else if( iFlags.IsSet( EIsInCall ) )
            {
            state = &XnPropertyNames::softkey::mode::KModeCallActive;
            }
        else
            {
            state = &XnPropertyNames::softkey::mode::KModeCallInactive;
            }
        
        XnMenuInterface::MXnMenuInterface* menuIf( NULL );
        XnComponentInterface::MakeInterfaceL( menuIf, menubar->AppIfL() );
               
        RPointerArray<CXnNode>& childrens( menubar->Children() );
        
        for( TInt i=0; i < childrens.Count(); i++ )
            {
            CXnNode* node = childrens[i];
            
            if( node && node->Type()->Type() == XnPropertyNames::softkey::KNodeName )
                {                  
                CXnProperty* mode = node->GetPropertyL( XnPropertyNames::softkey::KModeAttribute );
                CXnProperty* type = node->GetPropertyL( XnPropertyNames::softkey::KTypeAttribute );
                
                if( ( mode && *state == mode->StringValue() ) &&
                        ( type && type->StringValue().Compare( XnPropertyNames::softkey::type::KRight ) == 0 ) )
                    {
                    menuIf->SetSoftKeyL( &node->PluginIfL() , XnMenuInterface::MXnMenuInterface::ERight );
                    node->SetDirtyL( XnDirtyLevel::ERender );
                    } 
                }
            }
        }
    }


// End of file
