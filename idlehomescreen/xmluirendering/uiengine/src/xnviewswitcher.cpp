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
 * Description:  Implementation for wrapper for a box
 *
 */

// System includes
#include <akntouchgesturefwevents.h>
#include <centralrepository.h>

#include <activeidle2domaincrkeys.h>
#include <gfxtranseffect/gfxtranseffect.h>  

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK

// User includes
#include "xnappuiadapter.h"
#include "xnuiengine.h"
#include "xnproperty.h"
#include "xnviewmanager.h"
#include "xnviewdata.h"
#include "xndomnode.h"
#include "xnnode.h"
#include "xnviewcontroladapter.h"
#include "xneffectmanager.h"
#include "xnrootdata.h"
#include "xnviewadapter.h"
#include "xnbackgroundmanager.h"

#include "xnviewswitcher.h"

// Constants
const TInt KDrawDelay( 25000 ); // 16100 = 62 fps
const TInt KDragTreshold( 30 );
const TInt KMaxScrollSpeed( 150 );
const TInt KMinScrollSpeed( 20 );
const TInt KAnimationDuration( 500000 );
const TInt KDragOffsetForBackground( 100 );
const TInt KDragOffsetForPageSwitch( 50 );

enum TViewPosition
    {
    KCurrentView, KNextView, KPreviousView
    };

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void HideAdapter( CXnControlAdapter* aAdapter )
    {
    if( aAdapter )
        {
        aAdapter->Window().SetOrdinalPosition( -1 );
        }    
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void ShowAdapter( CXnControlAdapter* aAdapter )
    {
    if( aAdapter )
        {
        aAdapter->Window().SetOrdinalPosition( 1 );
        }    
    }

// -----------------------------------------------------------------------------
// Changes window position
// -----------------------------------------------------------------------------
//
void SetWindowPosition( CXnControlAdapter* aAdapter, const TPoint& aPos )
    {
    if( aAdapter )
        {
        aAdapter->Window().SetPosition( aPos );
        }
    }

// -----------------------------------------------------------------------------
// Hide/Show control
// -----------------------------------------------------------------------------
//
void SetVisible( CCoeControl* aControl, TBool aVisible )
    {
    if( aControl )
        {
        aControl->MakeVisible( aVisible );
        }
    }

// -----------------------------------------------------------------------------
// ShowNodes
// -----------------------------------------------------------------------------
//
void ShowNodesL( CXnNode& aNode, TBool aShow )
    {    
    CXnDomStringPool* sp( aNode.DomNode()->StringPool() );
    
    CXnProperty* prop( NULL );
    if( aShow )
        {
        prop = CXnProperty::NewL(
            XnPropertyNames::style::common::KDisplay, 
            XnPropertyNames::style::common::display::KBlock,
            CXnDomPropertyValue::EString, *sp );    
        } 
    else
        {
        prop = CXnProperty::NewL(
            XnPropertyNames::style::common::KDisplay, 
            XnPropertyNames::style::common::display::KNone,
            CXnDomPropertyValue::EString, *sp );
        }
    CleanupStack::PushL( prop );    
    
    aNode.SetPropertyWithoutNotificationL( prop );
    CleanupStack::Pop( prop );
    }

// -----------------------------------------------------------------------------
// IsNodeVisible
// -----------------------------------------------------------------------------
//
TBool IsNodeVisibleL( CXnNode& aNode )
    {
    CXnProperty* displayProp( aNode.DisplayL() );

    if ( displayProp )
        {
        const TDesC8& display( displayProp->StringValue() );

        if ( display == XnPropertyNames::style::common::display::KBlock )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnViewSwitcher::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnViewSwitcher* CXnViewSwitcher::NewL()
    {
    CXnViewSwitcher* self = new (ELeave) CXnViewSwitcher();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CXnViewSwitcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::ConstructL()
    {
    iFingerFollowSupported = IsFingerFollowSupportedL();
    iTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
    iViewSwitchState = KViewSwitchIdle;
    }

// -----------------------------------------------------------------------------
// CXnViewSwitcher::CXnViewSwitcher
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnViewSwitcher::CXnViewSwitcher() :
    iAppUi(static_cast<CXnAppUiAdapter&> (*iAvkonAppUi))
    {
    }

// -----------------------------------------------------------------------------
// CXnViewSwitcher::~CXnViewSwitcher
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnViewSwitcher::~CXnViewSwitcher()
    {    
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    delete iTimer;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CXnViewSwitcher::FingerFollowSupported() const
    {
    return iFingerFollowSupported;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CXnViewSwitcher::ProcessPointerEventL(const TPointerEvent& aPointerEvent )
    {
    TBool consumed(EFalse);
    
    if ( FingerFollowSupported() )
        {
        TPoint parentPos = aPointerEvent.iParentPosition;

        switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
                {
                if ( iViewSwitchState == KViewSwitchBegin || iViewSwitchState
                        == KViewSwitchIdle )
                    {
                    if (iTimer->IsActive())
                        {
                        iTimer->Cancel();
                        }
                    if( SetupViewsL() )
                        {
                        iViewSwitchState = KViewSwitchBegin;
                        iStartPosition = parentPos;
                        iScrollSpeed = KMaxScrollSpeed;
                        iTickAmount = 0;
                        iDirection = KCurrentView;
                        iScrollDistance = 0;
                        iScrollDirectionDistance = 0;
                        }
                    }
                else if( iViewSwitchState == KViewSwitchDragOngoing )
                    {
                    consumed = ETrue;
                    }
                else if( iViewSwitchState == KViewSwitchScroll )
                    {
                    consumed = ETrue;

                    if( iActivateView == KCurrentView )
                        {
                        if (iTimer->IsActive())
                            {
                            iTimer->Cancel();
                            }                    

                        iViewSwitchState = KViewSwitchDragOngoing;
                        consumed = ETrue;    
                        iViewStartPosition = iViewPosition;
                        iScrollDistance = 0;
                        iScrollDirectionDistance = 0;
                        iStartPosition = parentPos;

                        iTimer->Start( 0, KDrawDelay, TCallBack(
                                TimerCallback, this ) );
                        }
                    else if( iActivateView == KNextView )
                        {
                        if( iCurrentView.iAdapter && iNextView.iAdapter && 
                            iPreviousView.iAdapter )
                            {
                            if (iTimer->IsActive())
                                {
                                iTimer->Cancel();
                                }
                            
                            iViewSwitchState = KViewSwitchDragOngoing;
                            iActivateView = KCurrentView;
                            
                            if( iPreviousView.iAdapter == iNextView.iAdapter )
                                {
                                iPreviousView = iCurrentView;
                                iCurrentView = iNextView;
                                iNextView = iPreviousView;
                                }
                            else
                                {
                                iPreviousView = iCurrentView;
    
                                iCurrentView = iNextView;
                                
                                iNextView.iReadyToDraw = EFalse;
                                CXnViewData* nextViewData( 
                                    ViewData( *iCurrentView.iViewData, KNextView ) );
                                CXnNode* nextViewNode( nextViewData->ViewNode() );
                                if( nextViewNode )
                                    {
                                    iNextView.iAdapter = nextViewNode->Control();
                                    iNextView.iViewData = nextViewData;
                                    iNextView.iBgDrawn = EFalse;
                                    }                            
                                }
                            
                            iViewStartPosition = iCurrentView.iAdapter->Window().Position();
                            iScrollDistance = 0;
                            iScrollDirectionDistance = 0;
                            iStartPosition = parentPos;
                        
                            iScrollSpeed = KMaxScrollSpeed;
                            iTickAmount = 0;
                            iTimer->Start( 0, KDrawDelay, TCallBack(
                                    TimerCallback, this ) );
                            }
                        consumed = ETrue;
                        }
                    else if( iActivateView == KPreviousView )
                        {
                        if( iCurrentView.iAdapter && iNextView.iAdapter && 
                            iPreviousView.iAdapter )
                            {
                            if (iTimer->IsActive())
                                {
                                iTimer->Cancel();
                                }
                            
                            iViewSwitchState = KViewSwitchDragOngoing;
                            iActivateView = KCurrentView;

                            if( iPreviousView.iAdapter == iNextView.iAdapter )
                                {
                                iNextView = iCurrentView;
                                iCurrentView = iPreviousView;
                                iPreviousView = iNextView;
                                }
                            else
                                {
                                iNextView = iCurrentView;
    
                                iCurrentView = iPreviousView;
                                
                                iPreviousView.iReadyToDraw = EFalse;
                                CXnViewData* prevViewData( 
                                    ViewData( *iCurrentView.iViewData, KPreviousView ) );
                                CXnNode* prevViewNode( prevViewData->ViewNode() );
                                if( prevViewNode )
                                    {
                                    iPreviousView.iAdapter = prevViewNode->Control();
                                    iPreviousView.iViewData = prevViewData;
                                    iPreviousView.iBgDrawn = EFalse;
                                    }                            
                                }

                            iViewStartPosition = iCurrentView.iAdapter->Window().Position();
                            iScrollDistance = 0;
                            iScrollDirectionDistance = 0;
                            iStartPosition = parentPos;
                        
                            iScrollSpeed = KMaxScrollSpeed;
                            iTickAmount = 0;
                            iTimer->Start( 0, KDrawDelay, TCallBack(
                                TimerCallback, this ) );
                            }
                        consumed = ETrue;
                        }
                    }
                }
                break;
            case TPointerEvent::EDrag:
                {
                if( iViewSwitchState == KViewSwitchIdle )
                    {
                    consumed = EFalse;
                    }
                else if( iViewSwitchState == KViewSwitchBegin )
                    {
                    TPoint delta = parentPos - iStartPosition;

                    if( Abs(delta.iX) > KDragTreshold || Abs(delta.iY) > KDragTreshold )
                        {
#ifdef RD_TACTILE_FEEDBACK            
                        MTouchFeedback* feedback( MTouchFeedback::Instance() );
                        
                        if ( feedback )
                            {
                            feedback->InstantFeedback( ETouchFeedbackBasic );
                            }
#endif                           

                        iViewSwitchState = KViewSwitchDragOngoing;
                        iActivateView = KCurrentView;
                        iViewPosition.iY = iCurrentView.iAdapter->Position().iY;
                        iViewPosition.iX = 0;
                        iStartPosition = parentPos;

                        StartViewDraggingL();

                        iTimer->Start( 0, KDrawDelay, TCallBack(
                                TimerCallback, this ) );
                        }
                    consumed = ETrue;
                    }
                else if( iViewSwitchState == KViewSwitchDragOngoing )
                    {
                    iScrollDistance = parentPos.iX - iStartPosition.iX;
                    consumed = ETrue;
                    }
                else
                    {
                    consumed = EFalse;                
                    }
                }
                break;
            case TPointerEvent::EButton1Up:
                {
                if( iViewSwitchState == KViewSwitchBegin )
                    {
                    iViewSwitchState = KViewSwitchIdle;
                    }
                else if( iViewSwitchState == KViewSwitchDragOngoing )
                    {
                    if (iTimer->IsActive())
                        {
                        iTimer->Cancel();
                        }                    

#ifdef RD_TACTILE_FEEDBACK            
/*
                    MTouchFeedback* feedback( MTouchFeedback::Instance() );
                    
                    if ( feedback )
                        {
                        feedback->InstantFeedback( ETouchFeedbackBasic );
                        }
*/
#endif                           

                    consumed = ETrue;
                    
                    StartViewScrolling();
                    iViewSwitchState = KViewSwitchScroll;
                    }
                else if( iViewSwitchState == KViewSwitchScroll )
                    {
                    consumed = ETrue;
                    }
                }
                break;
            default:
                break;
            }
        }

    return consumed;
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::TouchGesture( TAknTouchGestureFwType& /*aTouchGesture*/ )
    {
    }

// -----------------------------------------------------------------------------
// CXnViewSwitcher::SizeChanged
// -----------------------------------------------------------------------------
// 
void CXnViewSwitcher::SizeChanged( TRect aRect )
    {
    iRect = aRect;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CXnViewSwitcher::SetupViewsL()
    {
    TBool ret = EFalse;
    
    CXnViewManager& wManager = iAppUi.ViewManager();
    iCurrentView.iViewData = &wManager.ActiveViewData();
    iCurrentView.iAdapter = iCurrentView.iViewData->ViewNode()->Control();
    iCurrentView.iReadyToDraw = ETrue;
    iCurrentView.iBgDrawn = ETrue;

    iViewStartPosition = TPoint( 0, iCurrentView.iAdapter->Window().Position().iY );

    if ( wManager.ViewAmount() > 1 )
        {
        ret = ETrue;
        
        CXnNode* nextViewNode( wManager.NextViewData().ViewNode() );
        if( nextViewNode )
            {
            iNextView.iAdapter = nextViewNode->Control();
            iNextView.iViewData = &wManager.NextViewData();
            }

        CXnNode* prevViewNode( wManager.PreviousViewData().ViewNode() );
        if( prevViewNode && prevViewNode == nextViewNode )
            {
            // Only 2 views in Homesceern -> previous = next
            iPreviousView = iNextView;
            }
        else if( prevViewNode )
            {
            iPreviousView.iAdapter = prevViewNode->Control();
            iPreviousView.iViewData = &wManager.PreviousViewData();
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::PrepareViewL( CXnViewSwitcher::TViewInformation& aView )
    {
    if( !aView.iAdapter || !aView.iViewData )
        {
        return;
        }
    
    CXnControlAdapter& adapter = *aView.iAdapter;
    RWindow& win( adapter.Window() );
    win.SetOrdinalPosition( -1 );
    adapter.MakeVisible( ETrue );

    CXnNode* viewNode( aView.iViewData->ViewNode() );
    TBool nodesVisible( IsNodeVisibleL( *viewNode ) );
    if( !nodesVisible )
        {
        // Set visible temporarily
        ShowNodesL( *viewNode, ETrue );
        }
    
    iAppUi.ViewAdapter().EnterEditStateL( *aView.iViewData, EFalse );
        
    viewNode->UiEngine()->RenderUIL( viewNode );
    aView.iReadyToDraw = ETrue;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::StartViewDraggingL()
    {
    // Remove focused node, in order to prevent activate triggers
    CXnNode* currentViewNode =
            iAppUi.ViewManager().ActiveViewData(). Node()->LayoutNode();
    CXnUiEngine* engine(currentViewNode->UiEngine());

    CXnNode* focusedNode = engine->FocusedNode();
    if (focusedNode)
        {
        focusedNode->UnsetStateL(XnPropertyNames::style::common::KPressedDown);
        focusedNode->UnsetStateL(XnPropertyNames::style::common::KFocus);
        }

    // Send button up to prevent long tapping 
    TPointerEvent pointerEvent;
    pointerEvent.iType = TPointerEvent::EButton1Up;
    if ( iCurrentView.iAdapter )
        {
        iCurrentView.iAdapter->CXnControlAdapter::HandlePointerEventL(
                pointerEvent);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::StartViewScrolling()
    {
    TInt screenWidth = iCurrentView.iAdapter->Window().Size().iWidth;

    UpdateViewToBeActivated( KDragOffsetForPageSwitch );

    switch (iActivateView)
        {
        case KPreviousView:
            {
            iScrollDistance = screenWidth - iViewPosition.iX;
            }
            break;
        case KNextView:
            {
            iScrollDistance = -screenWidth - iViewPosition.iX;
            }
            break;
        default: // KCurrentView
            {
            iScrollDistance = -iViewPosition.iX;
            }
            break;
        }

    TInt minSpeed = Abs( iScrollDistance / ( KAnimationDuration / KDrawDelay ) );
    iScrollSpeed = Abs( iScrollSpeed );
    if( iScrollSpeed < minSpeed )
        {
        iScrollSpeed = minSpeed;
        }
    if( iScrollSpeed < KMinScrollSpeed )
        {
        iScrollSpeed = KMinScrollSpeed;
        }
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    
    iTimer->Start(KDrawDelay, KDrawDelay, TCallBack(
            TimerCallback, this));
    }


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::ViewSwitchEndedL()
    {
    iViewSwitchState = KViewSwitchIdle;

    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }

    iViewPosition.iX = 0;

    if ( iCurrentView.iAdapter )
        {
        CXnViewManager& wManager( iAppUi.ViewManager() );
        CXnViewData* activeViewData( &wManager.ActiveViewData() ); 
        
        switch( iActivateView )
            {
            case KCurrentView:
                SetVisible( iNextView.iAdapter, EFalse );
                SetVisible( iPreviousView.iAdapter, EFalse );    
                
                if( iCurrentView.iViewData && activeViewData != iCurrentView.iViewData )
                    {
                    wManager.ActivateViewL( *iCurrentView.iViewData, TUid::Null(), EFalse );            
                    }
                break;
            case KPreviousView:
                if ( iPreviousView.iAdapter )
                    {
                    SetVisible( iCurrentView.iAdapter, EFalse );
                    if( iPreviousView.iAdapter != iNextView.iAdapter )
                        {
                        SetVisible( iNextView.iAdapter, EFalse );
                        }

                    CXnViewData* prevViewData( ViewData( *iCurrentView.iViewData, KPreviousView ) );
                    if( prevViewData && activeViewData != prevViewData )
                        {
                        wManager.ActivateViewL( *prevViewData, TUid::Null(), EFalse );            
                        }
                    }
                break;
            case KNextView:
                if ( iNextView.iAdapter )
                    {
                    SetVisible( iCurrentView.iAdapter, EFalse );
                    if( iPreviousView.iAdapter != iNextView.iAdapter )
                        {
                        SetVisible( iPreviousView.iAdapter, EFalse );                
                        }

                    CXnViewData* nextViewData( ViewData( *iCurrentView.iViewData, KNextView ) );
                    if( nextViewData && activeViewData != nextViewData )
                        {
                        wManager.ActivateViewL( *nextViewData, TUid::Null(), EFalse );            
                        }                    
                    }
                break;
            }

        ClearViews();
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::Scroll()
    {
    TInt scrollStep = iScrollDistance / 1.5;

    if( Abs( scrollStep ) > Abs( iScrollSpeed ) )
        {
        scrollStep = ( scrollStep > 0 ) ? iScrollSpeed : -iScrollSpeed;
        }

    iViewPosition.iX += scrollStep;
    iScrollDistance -= scrollStep;
    }
        
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::DoScroll()
    {
    SetWindowPosition( iCurrentView.iAdapter, iViewPosition );

    if( iViewPosition.iX < 0 && iNextView.iAdapter )
        {
        // next view shown
        if( !iNextView.iReadyToDraw )
            {
            TRAP_IGNORE( PrepareViewL( iNextView ) );
            if( iPreviousView.iAdapter == iNextView.iAdapter )
                {
                iPreviousView.iReadyToDraw = ETrue;          
                }
            }
        
        ShowAdapter( iNextView.iAdapter );
        if( iPreviousView.iAdapter != iNextView.iAdapter )
            {
            HideAdapter( iPreviousView.iAdapter );
            }
        
        TPoint pos( TPoint( iViewPosition.iX + iRect.Width(), iViewPosition.iY ) );
        SetWindowPosition( iNextView.iAdapter, pos );
        }

    else if( iViewPosition.iX > 0 && iPreviousView.iAdapter )
        {
        // previous view shown
        if( !iPreviousView.iReadyToDraw )
            {
            TRAP_IGNORE( PrepareViewL( iPreviousView ) );
            }

        ShowAdapter( iPreviousView.iAdapter );
        if( iPreviousView.iAdapter != iNextView.iAdapter )
            {
            HideAdapter( iNextView.iAdapter );
            }
        TPoint pos( TPoint( iViewPosition.iX - iRect.Width(), iViewPosition.iY ) );
        SetWindowPosition( iPreviousView.iAdapter, pos );
        }
    
    else
        {
        HideAdapter( iPreviousView.iAdapter );
        HideAdapter( iNextView.iAdapter );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CXnViewSwitcher::TimerCallback(TAny *aPtr)
    {
    CXnViewSwitcher* self = reinterpret_cast<CXnViewSwitcher*> (aPtr);

    if( self->iViewSwitchState == KViewSwitchDragOngoing )
        {
        TInt prevViewPos( self->iViewPosition.iX );
        self->iViewPosition.iX = self->iViewStartPosition.iX + self->iScrollDistance;
        self->DoScroll();
        
        self->UpdateViewToBeActivated( KDragOffsetForBackground );
        
        // Hack for informing NGA to draw. 
        self->iAppUi.ViewAdapter().BgManager().DrawNow(TRect(0,0,1,1));
        
        // Scroll speed is average drag delta between timer callbacks.
        self->iTickAmount++;

        TInt scrollDistance = self->iViewPosition.iX - prevViewPos;
        
        if( ( scrollDistance < 0 && self->iScrollDirectionDistance > 0 ) || 
            ( scrollDistance > 0 && self->iScrollDirectionDistance < 0 ) )
            {
            // Scroll direction has been changed
            self->iScrollDirectionDistance = scrollDistance;
            self->iScrollSpeed = scrollDistance;
            self->iTickAmount = 1;            
            }
        else
            {
            self->iScrollDirectionDistance += scrollDistance;
            self->iScrollSpeed = self->iScrollDistance / self->iTickAmount;    
            }
        
        if( scrollDistance < 0 )
            {
            self->iDirection = KNextView;
            }
        else if( scrollDistance > 0 )
            {
            self->iDirection = KPreviousView;
            }

        return 0;
        }

    if ( Abs( self->iScrollDistance ) <= 2 )
        {
        TRAP_IGNORE( self->ViewSwitchEndedL() );
        }
    else
        {
        self->Scroll();
        self->DoScroll();
        
        // Hack for inforing NGA to draw. 
        self->iAppUi.ViewAdapter().BgManager().DrawNow(TRect(0,0,1,1));
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CXnViewSwitcher::IsFingerFollowSupportedL() const
    {    
    CRepository* repository =
            CRepository::NewL(TUid::Uid(KCRUidActiveIdleLV));
    TBool value(EFalse);
    TInt err(repository->Get(KAIFingerFollowSupport, value));
    delete repository;

    return value && (KErrNone == err);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CXnViewData* CXnViewSwitcher::ViewData( CXnViewData& aCurrentViewData, 
    TInt aView )
    {
    CXnRootData& rootData = iAppUi.ViewManager().ActiveAppData();
    RPointerArray<CXnPluginData>& rootDataArr = rootData.PluginData();

    CXnViewData* ret( NULL );
    
    TInt viewAmount( rootDataArr.Count() );
    for( TInt i = 0; i < viewAmount; i++ )
        {
        CXnViewData* viewData = static_cast<CXnViewData*>( rootDataArr[i] );
        if( &aCurrentViewData == viewData )
            {
            if( aView == KNextView )
                {
                if( i < viewAmount - 1 )
                    {
                    ret = static_cast<CXnViewData*>( rootDataArr[i + 1] );
                    }
                else
                    {
                    ret = static_cast<CXnViewData*>( rootDataArr[0] );            
                    }
                break;
                }
            else if( aView == KPreviousView )
                {
                if( i == 0 )
                    {
                    ret = static_cast<CXnViewData*>( rootDataArr[viewAmount - 1] );
                    }
                else
                    {
                    ret = static_cast<CXnViewData*>( rootDataArr[i - 1] );            
                    }            
                break;
                }
            }
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::ClearViews()
    {
    TPoint p( 0, iCurrentView.iAdapter->Window().Position().iY );
    SetWindowPosition( iCurrentView.iAdapter, p );
    SetWindowPosition( iPreviousView.iAdapter, p );
    SetWindowPosition( iNextView.iAdapter, p );
    ShowAdapter( iPreviousView.iAdapter );
    ShowAdapter( iNextView.iAdapter );
    ShowAdapter( iCurrentView.iAdapter );
    iCurrentView.iAdapter = NULL;
    iCurrentView.iViewData = NULL;
    iCurrentView.iReadyToDraw = EFalse;
    iCurrentView.iBgDrawn = EFalse;
    iNextView.iAdapter = NULL;
    iNextView.iViewData = NULL;
    iNextView.iReadyToDraw = EFalse;
    iNextView.iBgDrawn = EFalse;
    iPreviousView.iAdapter = NULL;
    iPreviousView.iViewData = NULL;
    iPreviousView.iReadyToDraw = EFalse;
    iPreviousView.iBgDrawn = EFalse;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::UpdateViewToBeActivated( TInt aOffset )
    {
    if ( iViewPosition.iX > aOffset && iDirection == KPreviousView && 
        iScrollDirectionDistance > aOffset / 2)
        {
        if( iActivateView != KPreviousView &&
            iPreviousView.iViewData && iCurrentView.iViewData )
            {
            iActivateView = KPreviousView;
            if( !iPreviousView.iBgDrawn )
                {
                ChangeBackground( *iCurrentView.iViewData, 
                    *iPreviousView.iViewData );
                iPreviousView.iBgDrawn = ETrue; 
                iNextView.iBgDrawn = EFalse;
                iCurrentView.iBgDrawn = EFalse;
                }
            }
        }
    else if ( iViewPosition.iX < -aOffset && iDirection == KNextView && 
        iScrollDirectionDistance < -aOffset / 2 )
        {
        if( iActivateView != KNextView && 
            iNextView.iViewData && iCurrentView.iViewData )
            {
            iActivateView = KNextView;
            if( !iNextView.iBgDrawn )
                {
                ChangeBackground( *iCurrentView.iViewData, 
                    *iNextView.iViewData );
                iPreviousView.iBgDrawn = EFalse; 
                iNextView.iBgDrawn = ETrue;
                iCurrentView.iBgDrawn = EFalse;                            
                }
            }
        }
    else if( iActivateView != KCurrentView && 
        iPreviousView.iViewData && iNextView.iViewData && iCurrentView.iViewData )
        {
        if( Abs( iScrollDirectionDistance ) > aOffset / 2 )
            {
            iActivateView = KCurrentView;
            if( !iCurrentView.iBgDrawn )
                {    
                ChangeBackground( ( iPreviousView.iBgDrawn ) ? *iPreviousView.iViewData :
                    *iNextView.iViewData, *iCurrentView.iViewData );                                
                iPreviousView.iBgDrawn = EFalse; 
                iNextView.iBgDrawn = EFalse;
                iCurrentView.iBgDrawn = ETrue;                            
                }        
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::ChangeBackground( CXnViewData& aCurrent, CXnViewData& aNext )
    {
    CXnBackgroundManager& bg( iAppUi.ViewAdapter().BgManager() );

    GfxTransEffect::Begin( &bg, KGfxControlActionBgImgToImgAppear );
    
    bg.ChangeWallpaper( aCurrent, aNext, EFalse );

    GfxTransEffect::SetDemarcation( &bg, bg.Position() );
    GfxTransEffect::End( &bg ); 
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnViewSwitcher::StopViewSwitchL()
    {
    ViewSwitchEndedL();
    }

// End of file
