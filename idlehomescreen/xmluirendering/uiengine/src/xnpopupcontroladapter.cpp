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
* Description:  Popup control adapter
*
*/

// System includes
#include <e32base.h>
#include <e32const.h>
#include <coecntrl.h>
#include <AknUtils.h>
#include <aknstyluspopupmenu.h>

// User includes
#include "xnappuiadapter.h"
#include "xnviewadapter.h"
#include "xnkeyeventdispatcher.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xnplugindata.h"
#include "xnproperty.h"
#include "xncontroladapter.h"
#include "xndomnode.h"
#include "xnnodepluginif.h"
#include "xnnode.h"
#include "xntype.h"
#include "xnuiengine.h"
#include "xneditor.h"
#include "xnfocuscontrol.h"

#include "xnpopupcontroladapter.h"

// Constants
_LIT8( KMenuItem, "menuitem" );
_LIT8( KDynMenuItem, "dynmenuitem" );
_LIT8( KWidgetMenuItem, "widgetmenuitem" );
_LIT8( KMenuExtension, "menuextension" );

_LIT8( KSource, "source" );
_LIT8( KTarget, "target" );

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FindWidgetMenuItemL
//
// -----------------------------------------------------------------------------
//
static CXnNode* FindWidgetMenuItemL( CXnPluginData& aPlugin,
    const TDesC8& aSource )     
    {        
    if ( !aPlugin.Occupied() )
        {
        return NULL;
        }

    CXnNode* widget( aPlugin.Node()->LayoutNode() );

    RPointerArray< CXnNode >& children( widget->Children() );
    
    for ( TInt i = children.Count() - 1; i >= 0; i-- )
        {
        if ( children[i]->Type()->Type() == KMenuExtension )
            {
            RPointerArray< CXnNode >& items( children[i]->Children() );
            
            for ( TInt i = 0; i < items.Count(); i++ )
                {
                CXnProperty* prop( items[i]->GetPropertyL( KTarget ) );
                
                if ( prop && prop->StringValue() == aSource )
                    {
                    return items[i];
                    }                
                }
            
            break;
            }
        }
    
    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnPopupControlAdapter* CXnPopupControlAdapter::NewL( CXnNodePluginIf& aNode )
    {
    CXnPopupControlAdapter* self =
        new ( ELeave ) CXnPopupControlAdapter();

    CleanupStack::PushL( self );

    self->ConstructL( aNode );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::~CXnPopupControlAdapter
// Destructor.
// -----------------------------------------------------------------------------
//
CXnPopupControlAdapter::~CXnPopupControlAdapter()
    {           
    delete iStylusPopupMenu;
           
    iMenuItems.Reset();
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::CXnPopupControlAdapter
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnPopupControlAdapter::CXnPopupControlAdapter()    
    {
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::ConstructL
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::ConstructL( CXnNodePluginIf& aNode )
    {
    CXnControlAdapter::ConstructL( aNode ); 
    
    iNode = &aNode.Node();
    iUiEngine = iNode->UiEngine();
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::ProcessCommandL( TInt aCommandId )
    {    
    // Resolve selected item
    for ( TInt i = 0; aCommandId != KErrCancel && i < iMenuItems.Count(); i++ )
        {
        if ( i == aCommandId )
            {
        	// Do the actual item selection			
            iMenuItems[i]->SetStateL( 
            	XnPropertyNames::style::common::KActive );
                
            break;
            }
        }        

    iUiEngine->Editor()->SetTargetPlugin( NULL );
    
    iMenuItems.Reset();
    iCommandId = 0;
    
    iMenuShown = EFalse;
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::SetEmphasis
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::SetEmphasis( CCoeControl* /*aMenuControl*/, 
    TBool /*aEmphasis*/ )
    {         
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::SetObserver
// -----------------------------------------------------------------------------
//    
void CXnPopupControlAdapter::SetObserver( 
    XnMenuInterface::MXnMenuObserver& aObserver )   
    {
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::TryDisplayingStylusPopupL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::TryDisplayingStylusPopupL( CXnNode& aPlugin )
    {
    HideMenuL();
    
    CXnViewManager* manager( iUiEngine->ViewManager() );            
    CXnPluginData* plugin( manager->ActiveViewData().Plugin( &aPlugin ) );
    
    if ( !plugin )
        {
        return;
        }
    
    CXnViewAdapter& adapter( iUiEngine->AppUiAdapter().ViewAdapter() );
    
    const TPointerEvent& event( adapter.EventDispatcher()->PointerEvent() );
    
    delete iStylusPopupMenu;
    iStylusPopupMenu = NULL;
    
    // Popup is positioned relatively to screen
    iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this, event.iParentPosition );
      
    RPointerArray< CXnNode >& children( iNode->Children() );       
    
    for ( TInt i = 0; i < children.Count(); i++ )
        {      
        PopulateMenuL( *plugin, children[i] );
        }
    
    ShowMenuL( *plugin, event.iParentPosition );
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::HandleScreenDeviceChangedL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::HandleScreenDeviceChangedL()
    {
    HideMenuL();
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::PopulateMenuL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::PopulateMenuL( CXnPluginData& aPlugin, 
    CXnNode* aItem )    
    {                     
    CXnNode* menuitem( NULL );    
    CXnProperty* prop( NULL );
    
    TBool mappedItem( EFalse );

    // Map widget menuitem first
    if ( aItem->Type()->Type() == KWidgetMenuItem )
        {
        // Widget menuitem is shown if a widget defined by the "aPlugin" 
        // defines a proper source - target menuitem pairing
        CXnProperty* source( aItem->GetPropertyL( KSource ) );
                       
        if ( source )
            {                       
            CXnNode* widgetItem( 
                FindWidgetMenuItemL( aPlugin, source->StringValue() ) );
            
            if ( widgetItem )
                {
                mappedItem = ETrue;
                
                // Get label from the view item
                prop = aItem->LabelL();
                
                if ( !prop )
                    {
                    // no localization in view, check from widget
                    prop = widgetItem->LabelL();
                    }
                
                // Pretend the original item is this widget item
                aItem = widgetItem;            
                }                                               
            }                              
        }

    const TDesC8& type( aItem->Type()->Type() );
    
    if ( type == KMenuItem )
        {
        // By default plain menuitems are always shown
        menuitem = aItem;                                       
        }
    else if ( type == KDynMenuItem && iObserver )
        {
        CXnNode* plugin( aPlugin.Owner()->LayoutNode() );
        
        // Let observer decide whether dynmenuitem is visible or not
        if ( iObserver->DynInitMenuItemL( aItem->AppIfL(), &plugin->AppIfL() ) )
            {
            menuitem = aItem;                       
            }
        }
        
    if ( menuitem )
        {
        CXnProperty* display( menuitem->DisplayL() );
        
        if ( display && display->StringValue() == 
            XnPropertyNames::style::common::display::KNone )
            {
            return;
            }
        
        if ( !mappedItem )
            {
            prop = menuitem->LabelL(); 
            }

        if ( prop )
            {
            HBufC* label( prop->StringValueL() );
            CleanupStack::PushL( label );
            
            iStylusPopupMenu->AddMenuItemL( *label, iCommandId++ );
            iMenuItems.AppendL( menuitem );

            CleanupStack::PopAndDestroy( label );                    
            }        
        }       
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::ShowMenuL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::ShowMenuL( CXnPluginData& aPlugin, 
    TPoint aPosition )
    {
    if ( iMenuItems.Count() > 0 && !iMenuShown )
        {
        iUiEngine->AppUiAdapter().HideFocus();            
    
        CXnNode* plugin( aPlugin.Owner()->LayoutNode() );
        
        iUiEngine->Editor()->SetTargetPlugin( plugin );
                       
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            iStylusPopupMenu->SetPosition( aPosition, 
                CAknStylusPopUpMenu::EPositionTypeRightBottom );                        
            }
        else
            {
            iStylusPopupMenu->SetPosition( aPosition, 
                CAknStylusPopUpMenu::EPositionTypeLeftBottom );                        
            }
        
        iStylusPopupMenu->ShowMenu();
        iMenuShown = ETrue;
        }    
    }

// -----------------------------------------------------------------------------
// CXnPopupControlAdapter::HideMenuL
// -----------------------------------------------------------------------------
//
void CXnPopupControlAdapter::HideMenuL()
    {
    if ( iMenuShown )
        {        
        ProcessCommandL( KErrCancel );
        
        delete iStylusPopupMenu;
        iStylusPopupMenu = NULL;
        
        iMenuShown = EFalse;
        }    
    }

// End of file
