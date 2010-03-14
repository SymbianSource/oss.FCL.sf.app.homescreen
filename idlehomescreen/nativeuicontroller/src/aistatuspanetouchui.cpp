/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Choice list.
*
*/

// System includes
#include <AknLayout2ScalableDef.h>
#include <aknstyluspopupmenu.h>
#include <AknUtils.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <MProfileEngine.h>
#include <viewcli.h>
#include <aknlayoutscalable_avkon.cdl.h>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include "aistrcnv.h"
#include "aistatuspanetouchui.h"
#include "aistatuspanel.h"
#include "ainativeuiplugins.h"
#include "aifweventhandler.h"
#include "aiconsts.h"
#include "aidevicestatuscontentmodel.h"
#include <AiNativeUi.rsg>

using namespace AiNativeUiController;

// Constants
_LIT( KProfileSwitchByIndexEvent, "Profile/SwitchProfileByIndex" );
const TInt KProfileSwitchByIndexEventLength = 28;
const TInt KRadix = 10;
const TText16 KEventParameterTerminator = ')';
const TInt KExtraItems = 2;
const TInt KStylusMenuMaxItemsDefault = 0;

_LIT( KOpenAppProfiles, "Shortcut/LaunchByValue(localapp:0x100058F8)" );
_LIT( KOpenAppCalendar, "Shortcut/LaunchByValue(localapp:0x10005901)" );

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// IndexLength()
//
// ----------------------------------------------------------------------------
//
static TInt IndexLength( TInt aIndex )
    {
    TInt length = 0;

    if ( aIndex < 0 )
         {
         ++length;
         }

    do
      {
        aIndex /= KRadix;
        ++length;
        }
    while ( aIndex != 0 );
    return length;
  }

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::NewL()
//
// ----------------------------------------------------------------------------
//
CAiStatusPaneTouchUi* CAiStatusPaneTouchUi::NewL( CAiStatusPanel& aStatusPanel,
    MAiFwEventHandler& aEventHandler )
    {
    CAiStatusPaneTouchUi* self = 
        CAiStatusPaneTouchUi::NewLC( aStatusPanel, aEventHandler );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::NewLC()
//
// ----------------------------------------------------------------------------
//
CAiStatusPaneTouchUi* CAiStatusPaneTouchUi::NewLC( CAiStatusPanel& aStatusPanel,
    MAiFwEventHandler& aEventHandler )
    {
    CAiStatusPaneTouchUi* self = 
        new ( ELeave ) CAiStatusPaneTouchUi( aStatusPanel, aEventHandler );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::CAiStatusPaneTouchUi()
//
// ----------------------------------------------------------------------------
//
CAiStatusPaneTouchUi::CAiStatusPaneTouchUi( CAiStatusPanel& aStatusPanel,
    MAiFwEventHandler& aEventHandler )
    : iStatusPane( aStatusPanel ), iEventHandler( aEventHandler )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::~CAiStatusPaneTouchUi()
//
// ----------------------------------------------------------------------------
//
CAiStatusPaneTouchUi::~CAiStatusPaneTouchUi()
    {
    delete iEventBuffer;
    delete iMenu;
    iProfileNamePointerArray.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::ConstructL()
    {
    if( AknLayoutUtils::PenEnabled() )
        {
        iStatusPane.SetNaviDecoratorObserver( this );
        }
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::LoadUIDefinitionL()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::LoadUIDefinitionL()
    {
    // If profile popup exists, for example, when screendevicechange occurs,
    // the popup is closed
    if( iMenu )
        {
        delete iMenu;
        iMenu = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::ProcessCommandL()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::ProcessCommandL( TInt aCommandId )
    {
    if( aCommandId == EAIGotoCalendarCmdLink )
      {
      //Open calendar application
      iEventHandler.HandlePluginEvent( KOpenAppCalendar );
      }
    else if( aCommandId == EAIShowAllProfileCmdLink )
      {
      //Open profile application
      iEventHandler.HandlePluginEvent( KOpenAppProfiles );
      }
    else
      {

    // Calculate event buffer length
      TInt length = KProfileSwitchByIndexEventLength
                    + KPluginEventSeparatorLength
                  + IndexLength( aCommandId - EAIProfileCmdLink )
                    + KEventParameterSeparatorLength;

        // Allocate event buffer
        TPtr event = AiUtility::EnsureBufMaxLengthL( iEventBuffer, length );

        // Create event string
        event.Zero();
        event.Append( KProfileSwitchByIndexEvent );
        event.Append( KEventParameterSeparator );
        event.AppendNum( aCommandId - EAIProfileCmdLink );
        event.Append( KEventParameterTerminator );

      iEventHandler.HandlePluginEvent( event );
      }
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::HandleNaviDecoratorEventL()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::HandleNaviDecoratorEventL( TInt aEventID )
    {
    if( aEventID == EAknNaviDecoratorEventNaviLabel )
        {
#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( ETouchFeedbackBasic );
            }
#endif

        TBool isShowAllProfilesItem = EFalse;
        TInt count = iProfileNamePointerArray.Count();

        if ( !count )
            {
            //profile plugin is not ready!
            return;
            }

        TInt aiStylusMenuMaxItems = KStylusMenuMaxItemsDefault;
        TPoint tapPoint(0,0);
        TRect clientRect;
        TRect naviRect;

        // Check screen sizes
        TInt isMainPane = AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, clientRect );
        TInt isNaviPane = AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane, naviRect );

        // Calculating how many items fits into menu, using font height
        TAknLayoutText layoutText;
        layoutText.LayoutText( naviRect,
                AknLayoutScalable_Avkon::list_single_touch_menu_pane_t1().LayoutLine());

        TInt txtHeight = layoutText.Font()->HeightInPixels();
        // More additional space for marginals
        txtHeight += txtHeight + txtHeight/2;

        if( isNaviPane )
            {
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                tapPoint.SetXY( naviRect.iBr.iX, naviRect.iBr.iY );
                }
            else
                {
                tapPoint.SetXY( naviRect.iTl.iX, naviRect.iBr.iY );
                }
            }

        if ( iMenu )
            {
            delete iMenu;
            iMenu = NULL;
            }

        iMenu = CAknStylusPopUpMenu::NewL( this, tapPoint );

        if( isMainPane )
            {
            // Count how many menu items would fit into screen
            aiStylusMenuMaxItems = clientRect.Height() / txtHeight;
            }

            HBufC* caleText = StringLoader::LoadLC( R_IDLE_NAVI_TAPPED_CALE );
            iMenu->AddMenuItemL( *caleText, EAIGotoCalendarCmdLink );
            CleanupStack::PopAndDestroy( caleText );
            count++;

      // Space for 2 extra items GotoCalendar and ShowAllProfiles
      if( count > aiStylusMenuMaxItems )
    	  {
    	  count = aiStylusMenuMaxItems - KExtraItems;
            isShowAllProfilesItem = ETrue;
            }
        else
            {
    	  count -= 1;
            }

        for( TInt i = 0; i < count; i++ )
            {
            iMenu->AddMenuItemL( iProfileNamePointerArray[i]->Des(), i + EAIProfileCmdLink );
            }

        if( isShowAllProfilesItem )
            {
            HBufC* allProfilesText = StringLoader::LoadLC( R_IDLE_NAVI_TAPPED_ALL_PROF );
            iMenu->AddMenuItemL( *allProfilesText, EAIShowAllProfileCmdLink );
            CleanupStack::PopAndDestroy( allProfilesText );
            }

        iMenu->ShowMenu();
        }
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::DoPublishL()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::DoPublishL( CHsContentPublisher& aPlugin,
    TInt aContent, const TDesC16& aText, TInt aIndex )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    if( info.Uid() == KProfilePluginUid )
      {
      switch( aContent )
        {
        case EAiProfileContentProfileName:
        {
        if( aIndex - 1 == 0  )
          {
          iProfileNamePointerArray.ResetAndDestroy();
          }

        TInt count = iProfileNamePointerArray.Count();

        if( count >= aIndex  )
          {
          delete iProfileNamePointerArray[ aIndex - 1 ];
          iProfileNamePointerArray.Remove( aIndex - 1);
          }

        HBufC* name = aText.AllocL();
        iProfileNamePointerArray.Insert( name, aIndex - 1 );
        break;
        }
        default:
          {
          break;
          }
        };
      }

    else
      {
      User::Leave( KErrNotFound );
      }
    }

// ----------------------------------------------------------------------------
// CAiStatusPaneTouchUi::SetEmphasis()
//
// ----------------------------------------------------------------------------
//
void CAiStatusPaneTouchUi::SetEmphasis( CCoeControl* /*aMenuControl*/, 
    TBool /*aEmphasis*/ )
    {
    }

// End of file
