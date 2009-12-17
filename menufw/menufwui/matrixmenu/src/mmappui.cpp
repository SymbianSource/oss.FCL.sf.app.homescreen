/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI class
*  Version     : %version: MM_176.1.28.1.52 % << Don't touch! Updated by Synergy at check-out.
*  Version     : %version: MM_176.1.28.1.52 % << Don't touch! Updated by Synergy at check-out.
*
*/

#include <e32base.h>
#include <e32cmn.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <akntitle.h>
#include <eikmenub.h>
#include <akntoolbar.h>
#include <aknbutton.h>
#include <e32hashtab.h>
#include <layoutmetadata.cdl.h>
#include <gulicon.h>
#include <fbs.h>
#include <eikcmbut.h>
#include <hlplch.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <activeidle2domainpskeys.h>
#include <UikonInternalPSKeys.h>
#include <e32property.h>
#include <AknTaskList.h>
#include <AknSgcc.h>                           // for transition effects
#include <featmgr.h>
#include <matrixmenu.rsg>
#include <akntabgrp.h>
#include <apgcli.h>
#include <hwrmdomainpskeys.h> //flip status enums
#include <AknDef.hrh>
#include <AknDlgShut.h>
#include <mmenuinternalPSkeys.h>
#include <aknstyluspopupmenu.h> //stylus popup for long tap event

#include "mmgui.hrh"
#include "mmguiconstants.h"
#include "mmappui.h"
#include "menudebug.h"
#include "hnengine.h"
#include "hnglobals.h"
#include "hnsuitemodelcontainer.h"
#include "hnmenuitemmodel.h"
#include "hntoolbarmodel.h"
#include "hnbuttonmodel.h"
#include "hnitemmodel.h"
#include "hnconvutils.h"
#include "hnsuitemodel.h"
#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "mmwidgetcontainer.h"
#include "mmtemplatelibrary.h"
#include "mmextensionmanager.h"
#include "mmpropertysubscriber.h"
#include "hnitemsorder.h"

#include "mmnomemory.h"
#include "mmappkeyhandler.h"

enum TMenuTransEffectContext
    {
    EMenuOpenFolderEffect = 1001,
    EMenuCloseFolderEffect = 1002
    };

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ConstructL()
    {
    DEBUG(("_Mm_:CMmAppUi::ConstructL IN"));
    MMPERF(("Test Logger speed..."));
    MMPERF(("Test Logger speed - DONE"));
    MMPERF(("GO!"));

    //set matrix to be system app
    iEikonEnv->SetSystem( ETrue );

    TInt appUiFlags = AknLayoutUtils::PenEnabled() ?
    	EAknEnableSkin | EAknSingleClickCompatible :
        EAknEnableSkin | EAknEnableMSK;
    BaseConstructL( appUiFlags );
    iAppUiFactory = CEikonEnv::Static()->AppUiFactory( *this );
    ASSERT( iAppUiFactory );

    FeatureManager::InitializeLibL();
    iIsKastorEffectStarted = EFalse;
    StartLayoutSwitchFullScreen( AknTransEffect::EApplicationStart );

    RefreshUiPanesL( ETrue );
    Cba()->MakeVisible( EFalse );
    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );

    iDummyTemplateLib = CMmTemplateLibrary::NewL();
    iDummyContainer = CMmWidgetContainer::NewGridContainerL( ClientRect(),
            this, iDummyTemplateLib );
    iDummyContainer->SetEmptyTextL( KNullDesC );
    AddToStackL( iDummyContainer, ECoeStackPriorityDefault,
            ECoeStackFlagRefusesFocus | ECoeStackFlagRefusesAllKeys );
    iDummyContainer->MakeVisible( ETrue );
    iDummyContainer->DrawNow();

    iHNInterface = CHnEngine::NewL( *this );
    iTemplateLibrary = CMmTemplateLibrary::NewL();
    InitializeL();

    iMmExtManager = CMMExtensionManager::NewL( *this );

    iKeyboardFlipStatus = EPSHWRMFlipStatusUninitialized;
    iSliderEventSubscriber = CMmPropertySubscriber::NewL(KPSUidHWRM,
            KHWRMFlipStatus, *this);

    iScreenOn = ETrue;
    iSkinChangeNeeded = EFalse;
    iSkinChangeInProgress = EFalse;
    iHasFocus = ETrue;

    iSkinSrvSession.Connect(this);
    iAppkeyHandler = CMmAppkeyHandler::NewL();
    iOptionsMenuIsOpen = EFalse;

    RProperty::Define( KMMenuPSCat, KMMenuLastViewKey, RProperty::EText );

    MMPERF(("CMmAppUi::ConstructL - HN ready"));
    DEBUG(("_Mm_:CMmAppUi::ConstructL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::IsRootdisplayedL()
    {
    TBool ret( EFalse );
    if( iHNInterface && iHNInterface->GetSuiteModelsCountL() == KModelCountForRoot )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmAppUi::CMmAppUi()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmAppUi::~CMmAppUi()
    {
    DEBUG(("_Mm_:CMmAppUi::~CMmAppUi IN"));

    delete iOutOfMemoryHandler;
    if (iCurrentSuiteModel)
        {
        iCurrentSuiteModel->UnregisterSuiteObserver( this );
        }
    delete iToolbar;
    iCascadeMenuMap.Close();
    RemoveFromStack( iCurrentContainer );
    RemoveFromStack( iDummyContainer );
    iDummyContainer->MakeVisible( EFalse );
    delete iDummyContainer;
    ResetContainerMap();
    iGarbage.ResetAndDestroy();
    delete iHNInterface;
    FeatureManager::UnInitializeLib();
    delete iMmExtManager;
    delete iSliderEventSubscriber;
    delete iTemplateLibrary;
    delete iDummyTemplateLib;
    delete iAppkeyHandler;
    delete iPopupMenu;
    iSkinSrvSession.Close();

    MMPERF(("Matrix closed"));
    DEBUG(("_Mm_:CMmAppUi::~CMmAppUi OUT"));

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    //we're interested in layout change events
    //only if container is available
    if ( aType == KEikDynamicLayoutVariantSwitch &&
            iCurrentContainer && iCurrentSuiteModel )
        {
        MMPERF(("Layout change START"));
        // Do not call StartLayoutSwitchFullScreen(),
        // layout switch effect is started automatically
        iIsKastorEffectStarted = ETrue;
        TInt lastItemIndex = iCurrentContainer->NumberOfItems() - 1;

        TBool makeLastItemVisible = lastItemIndex >= 0 &&
			iCurrentContainer->ItemIsFullyVisible( lastItemIndex );

        iCurrentContainer->SetRect( ClientRect() );
        iDummyContainer->SetRect( ClientRect() );

        // This fixes a problem (view scrolled one row up) that occurs when
        // switching from portrait to landscape orientation with scrollbar
        // in bottom position.
        if ( makeLastItemVisible )
        	{
        	iCurrentContainer->Widget()->View()->SetTopItemIndex(
        		iCurrentContainer->Widget()->View()->
					CalcNewTopItemIndexSoItemIsVisible(	lastItemIndex ) );
        	}

        THashMapIter< TInt, CMmWidgetContainer* > iterator( iContainerMap );
        while( iterator.NextKey() )
            {
            CMmWidgetContainer* container = *iterator.CurrentValue();
            if ( container != iCurrentContainer )
                {
                container->SetRect( ClientRect() );
                container->HandleResourceChange( aType );
                }
            }
        ShowSuiteL();
        EndFullScreen();
        MMPERF(("Layout change END"));
        }

    if ( aType == KEikMessageFadeAllWindows && iCurrentContainer )
        {
        iCurrentContainer->SetIsFaded( ETrue );
        }
    else if ( aType == KEikMessageUnfadeWindows && iCurrentContainer )
        {
        iCurrentContainer->SetIsFaded( EFalse );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::ProcessCommandParametersL( TApaCommand aCommand,
                                                TFileName& aDocumentName,
                                                const TDesC8& aTail )
    {
    DEBUG(("_Mm_:CMmAppUi::ProcessCommandParametersL IN"));
    DEBUG8(("\t_Mm_:tail: %S", &aTail));

    if ( aTail.Length() )
        {
        DEBUG(("\t_Mm_:call HandleMessageL"));
        HandleMessageL( aTail );
        }

    TBool result = CEikAppUi::ProcessCommandParametersL( aCommand,
                                                    aDocumentName,
                                                    aTail );

    MMPERF(("Matrix started and ready!"));
    DEBUG(("_Mm_:CMmAppUi::ProcessCommandParametersL OUT"));
    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ProcessCommandL(TInt aCommand)
    {
    TBool ignoreCommand = EFalse;

    // ignore options click in edit mode and intermediate states for non-touch
    if ( aCommand == EAknSoftkeyOptions && iEditModeStatus != ENoEditMode &&
            !AknLayoutUtils::PenEnabled() )
        {
        ignoreCommand = ETrue;
        }

    // ignore keyselect for non-touch while in edit mode or stopping edit mode
    if ( aCommand == KKeyIdSelect && !AknLayoutUtils::PenEnabled() &&
            ( iEditModeStatus == ETransitionFromEditMode || IsEditMode() ) )
        {
        ignoreCommand = ETrue;
        }

    if ( !ignoreCommand )
        {
        CAknAppUi::ProcessCommandL( aCommand );
        }

    if (iCurrentContainer && iCurrentContainer == TopFocusedControl() )
    	{
    	iCurrentContainer->HandleTopFocusL();
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ProcessMessageL( TUid /*aUid*/, const TDesC8& aParams )
    {
    DEBUG(("_Mm_:CMmAppUi::ProcessMessageL IN"));
    DEBUG8(("\t_Mm_:params: %S", &aParams));
    MMPERF(("APA Message Recieved"));

    // fix for the CR 417-35490
    // Handle empty message sends by AVKON after app key press or switch
    // to Menu via task swapper.
    if( !aParams.Compare( KNullDesC8 ) )
        {
        if ( !iAppkeyHandler->IsActive() )
        	{
            CleanupForExitL( EExitKeyApplication );
			User::LeaveIfError( iCoeEnv->WsSession().SetWindowGroupOrdinalPosition(
						CEikonEnv::Static()->RootWin().Identifier(), 0 ) );
			iAppkeyHandler->StartL();
        	}
        }

    // first refresh model, show afterwords
    if ( aParams.Length() )
        {
        DEBUG(("\t_Mm_:call HandleMessageL"));
        HandleMessageL( aParams );
        }

    MMPERF(("APA Message Processed"));
    DEBUG(("_Mm_:CMmAppUi::ProcessMessageL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CMmAppUi::HandleMessageL(
         TUint32 aClientHandleOfTargetWindowGroup,
         TUid aMessageUid,
         const TDesC8& aMessageParameters )
    {
    if ( aMessageUid.iUid == KUidApaMessageSwitchOpenFileValue )
        {
        ProcessMessageL( aMessageUid, aMessageParameters );
        return EMessageHandled;
        }
    else
        {
        return CAknAppUi::HandleMessageL(
                aClientHandleOfTargetWindowGroup,
                aMessageUid,
                aMessageParameters );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleCommandL( TInt aCommand )
    {
    DEBUG(("_Mm_:CMmAppUi::HandleCommandL IN"));
    DEBUG(("\t_Mm_:aCommand: %d",aCommand));

    // in case something has gone wrong
    ClearTransitionFromEditModeFlag();

    switch( aCommand )
        {
        case EEikCmdExit:
            {
            DEBUG(("\t_Mm_:EEikCmdExit"));
            // Simply exit
            Exit();
            }
            break;

        case EMmOptionsExit:
            {
            MMPERF(("Options->Exit pressed"));
            DEBUG(("\t_Mm_:EMmOptionsExit"));

            //Check if it is a folder suite.If Yes really exit.
            //Otherwise behave like LSK.
            if( iCurrentSuiteModel &&
                iCurrentSuiteModel->SuiteName().Compare( KFolderSuite) )
                {
                if( iCurrentSuiteModel->ExitMode() == EExitModeHide )
                    {
                    // hide and reset menu
                    TApaTaskList taskList( iCoeEnv->WsSession() );
                    TApaTask me = taskList.FindApp( KUidMatrixMenuApp );
                    me.SendToBackground();
                    CleanupForExitL( EExitKeyApplication );
                    }
                else
                    {
                    StartLayoutSwitchFullScreen( EMenuCloseFolderEffect );
                    HandleBackCommandL();
                    }
                }
            else
                {
                ExitMatrix( EExitReally );
                }
            }
            break;

        case EAknSoftkeyExit:
            {
            MMPERF(("SoftkeyExit pressed"));
            DEBUG(("\t_Mm_:EAknSoftkeyExit"));
            ExitMatrix( EExitToPhone );
            }
            break;

        case EAknSoftkeyBack:
            {
            StartLayoutSwitchFullScreen( EMenuCloseFolderEffect );

            if( iCurrentSuiteModel && iCurrentSuiteModel->ExitMode() == EExitModeHide )
                {
                // hide and reset menu
                TApaTaskList taskList( iCoeEnv->WsSession() );
                TApaTask me = taskList.FindApp( KUidMatrixMenuApp );
                me.SendToBackground();
                if ( iCurrentContainer )
                    {
                    iCurrentContainer->MakeVisible( EFalse );
                    }
                iDummyContainer->MakeVisible( ETrue );
                RefreshCbaL();
                iDummyContainer->DrawNow();
                CleanupForExitL( EExitKeyApplication );
                }
            else if( iCurrentSuiteModel )
                {
                HandleBackCommandL();
                }
            }
            break;

        case EAknSoftkeyOk:
        case EAknSoftkeyDone:
            {
            ASSERT( IsEditMode() );
            SetEditModeL( EFalse );
            }
            break;
        default:
            {
            ForwardEventToHNL( aCommand );
            }
            break;
        }
    DEBUG(("_Mm_:CMmAppUi::HandleCommandL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleBackCommandL()
    {
    DEBUG(("_Mm_:CMmAppUi::HandleBackCommandL IN"));
    MMPERF(("CMmAppUi::HandleBackCommandL - START"));
    DEBUG16(("\t_Mm_:current genre: %S",&iCurrentSuiteModel->SuiteName()));

    iDummyContainer->MakeVisible( ETrue );
    RefreshUiPanesL( ETrue );
    iCurrentContainer->MakeVisible( EFalse );
    iDummyContainer->DrawNow();

    iHNInterface->HandleBackEventL( iCurrentSuiteModel->SuiteName() );

    MMPERF(("CMmAppUi::HandleBackCommandL - DONE"));
    DEBUG(("_Mm_:CMmAppUi::HandleBackCommandL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmWidgetContainer* CMmAppUi::GetAppropriateContainerToLoadL()
    {
    CMmWidgetContainer* retContainer = NULL;
    if ( iCurrentSuiteModel )
        {
        TInt currentSuiteId = iCurrentSuiteModel->GetItemsOrder()->GetSuiteId();

        CMmWidgetContainer** ret = iContainerMap.Find( currentSuiteId );
        retContainer = (ret) ? *ret : NULL;

        if ( ret && (*ret)->WidgetType() != iCurrentSuiteModel->WidgetType() )
            {
            iMakeHightlightedItemFullyVisible = ETrue;
            iGarbage.AppendL( *ret );
            iContainerMap.Remove(
                    iCurrentSuiteModel->GetItemsOrder()->GetSuiteId() );
            retContainer = NULL;
            }
        }
    return retContainer;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ShowSuiteL()
    {
    DEBUG(("_Mm_:CMmAppUi::ShowSuiteL IN"));
//    DEBUG16(("\t_Mm_:current genre: %S", &iCurrentSuiteModel->SuiteName()));

    if ( iCurrentSuiteModel )
        {
        CMmWidgetContainer* containerToLoad = GetAppropriateContainerToLoadL();
        TBool makeHandlePresentationChange( ETrue );

        if (!containerToLoad)
            {
            makeHandlePresentationChange = CreateNewContainerL(
                    iCurrentSuiteModel, containerToLoad );
            }

        if( makeHandlePresentationChange )
            {
            HandlePresentationChangeL( containerToLoad );
            }
        }

    DEBUG(("_Mm_:CMmAppUi::ShowSuiteL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::CreateNewContainerL(
        CHnSuiteModel* aSuiteModel, CMmWidgetContainer*& aContainerToLoad )
    {
    TRect rect = ClientRect();
    switch( aSuiteModel->WidgetType() )
        {
        case ECoverFlowWidget :
        case EGridWidget :
            aContainerToLoad = CMmWidgetContainer::NewGridContainerL(
                    rect, this, iTemplateLibrary );
            break;
        case EListWidget :
            aContainerToLoad = CMmWidgetContainer::NewListBoxContainerL(
                    rect, this, iTemplateLibrary );
            break;
        case EUnspecified:
        default:
            return EFalse;
        }
    aContainerToLoad->SetLongTapObserver(this);
    aContainerToLoad->HandleItemAdditionL();
    iContainerMap.InsertL(
            aSuiteModel->GetItemsOrder()->GetSuiteId(),
            aContainerToLoad );
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ForwardEventToHNL( TInt aEvent, TInt aItemId,
        CLiwGenericParamList* aEventParams )
    {
    DEBUG(("\t_Mm_:event ID: %d for item ID %d", aEvent, aItemId));

    TInt id = iCurrentSuiteModel->IdByIndex( aItemId );
    if ( id >= 0 )
        {
        MMPERF(("Handling event %d for item %d - START",aEvent,aItemId));

        if( aEvent == KKeyIdSelect )
            {
            MMPERF(("This is a 'select' event"));

            TInt modelId = iCurrentSuiteModel->IdByIndex( aItemId );
            CHnItemModel* itemModel =
                iCurrentSuiteModel->GetItemModel( modelId );
            TInt effect = GetKastorEffectL( itemModel );
            TRect rect = GetKastorRectL( itemModel, aItemId );
            TUid appUid = itemModel->GetItemType() == EItemTypeApplication ?
                itemModel->GetItemUid() : KUidMatrixMenuApp;

            if (iScreenOn && IsForeground() && effect
                    != AknTransEffect::ENone)
                {
                DEBUG(("_MM_:CMmAppUi::ForwardEventToHNL Foreground"));
                GfxTransEffect::BeginFullScreen(
                     effect, rect,
                     AknTransEffect::EParameterType,
                     AknTransEffect::GfxTransParam( appUid ) );
                }
            }

        iHNInterface->TriggerHnEventL( aEvent, id, aEventParams );
        MMPERF(("Handling event - END"));
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmAppUi::GetKastorEffectL( CHnItemModel* aItemModel )
	{
	TInt effect( AknTransEffect::ENone );
    if ((IsEditMode() || iEditModeStatus == ETransitionFromEditMode)
            && !AknLayoutUtils::PenEnabled())
        {
        return effect;
        }
	switch( aItemModel->GetItemType() )
		{
		case EItemTypeApplication:
			{
			if( !IsEditMode() && !aItemModel->IsDrmExpired() )
				{
				if( aItemModel->IsRunning() )
					{
					effect = AknTransEffect::EApplicationStartSwitchRect;
					}
				else
					{
					effect = AknTransEffect::EApplicationStartRect;
					}
				}
			break;
			}
		case EItemTypeSuite:
			{
			if( !IsEditMode() )
				{
				effect = EMenuOpenFolderEffect;
				iIsKastorEffectStarted = ETrue;
				}
			break;
			}
		case EItemTypeParentFolder:
		    {
            effect = EMenuCloseFolderEffect;
            iIsKastorEffectStarted = ETrue;
		    break;
		    }
		case EItemTypeFolder:
			{
			if (!(IsEditMode() && aItemModel->IsDeleteLocked()))
                {
                effect = EMenuOpenFolderEffect;
                iIsKastorEffectStarted = ETrue;
                }
			break;
			}
		case EItemTypeUnknown:
		default:
			{
			effect = AknTransEffect::ENone;
			break;
			}
		}
    return effect;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TRect CMmAppUi::GetKastorRectL( CHnItemModel* aItemModel, TInt aItemId )
    {
    TRect rect = TRect();
    if( aItemModel->GetItemType() == EItemTypeApplication )
        {
        rect = iCurrentContainer->GetItemRectL( aItemId );
        RRegion region; CleanupClosePushL( region );
        StatusPane()->GetShapeL( region, true, true );
        TInt statusPaneHeight = region.BoundingRect().Height();
        rect.iBr.iY += statusPaneHeight;
        rect.iTl.iY += statusPaneHeight;
        CleanupStack::PopAndDestroy( &region );
        }
    return rect;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ForwardEventToHNL( TInt aEvent )
    {
    DEBUG(("_Mm_:CMmAppUi::ForwardEventToHNL IN"));
    DEBUG(("\t_Mm_:event ID: %d", aEvent));

    if ( iCurrentContainer )
        {
        TBool idByContainer = iCurrentContainer->IsHighlightVisible() &&
                iCurrentContainer->GetSuiteModelL()->GetItemModelsCount() > 1;
        TInt current = idByContainer ?
             iCurrentContainer->GetHighlight() : KErrNotFound;
        ForwardEventToHNL( aEvent, current );
        }

    DEBUG(("_Mm_:CMmAppUi::ForwardEventToHNL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::NotifyUiRefreshL( const THnUiRefreshType aRefreshType )
    {
    DEBUG(("_Mm_:CMmAppUi::NotifyUiRefreshL IN"));
    DEBUG(("\t_Mm_:refresh type: %d", aRefreshType));

    switch (aRefreshType)
        {
        case ERefreshSuite:
            TRAPD( err, ShowSuiteL() );
            if ( KErrNoMemory == err )
                {
                HandleOutOfMemoryL();
                User::Leave( KErrNoMemory );
                }
            break;
        case ERefreshToolbar:
            RefreshToolbarL();
            break;
        case EStartEditMode:
            SetEditModeL( ETrue );
            break;
        case EStopEditMode:
            SetEditModeL( EFalse );
            break;
        case EZoomLarge:
            SetZoom( EAknUiZoomLarge );
            break;
        case EZoomSmall:
            SetZoom( EAknUiZoomSmall );
            break;
        case EZoomNormal:
            SetZoom( EAknUiZoomNormal );
            break;
        case EForegroundGain:
            {
            DEBUG(("_MM_:CMmAppUi::NotifyUiRefreshL Foreground"));
            TApaTaskList taskList( iCoeEnv->WsSession() );
            TApaTask me = taskList.FindApp( KUidMatrixMenuApp );
            me.BringToForeground();
            }
            break;
        case EBackgroundGain:
            {
            DEBUG(("_MM_:CMmAppUi::NotifyUiRefreshL Foreground"));
            TApaTaskList taskList( iCoeEnv->WsSession() );
            TApaTask me = taskList.FindApp( KUidMatrixMenuApp );
            me.SendToBackground();
            }
            break;
        case ELightOn:
            {
            DEBUG(("_Mm_:CMmAppUi::NotifyUiRefreshL - ELightOn"));
            iScreenOn = ETrue;
            if ( IsForeground() && iCurrentSuiteModel )
                {
                iCurrentSuiteModel->SetVisibleL( ETrue );
                }
            if ( iCurrentContainer )
                {
                iCurrentContainer->HandleForegroundGainedL();
                }
            }
            break;
        case ELightOff:
            {
            DEBUG(("_Mm_:CMmAppUi::NotifyUiRefreshL - ELightOff"));
            iScreenOn = EFalse;
            if (iCurrentContainer)
                {
                iCurrentContainer->HandleBackgroundGainedL();
                if (IsEditMode() && iCurrentContainer->IsDraggable())
                    {
                    iCurrentContainer->CancelDragL(EFalse);
                    }
                }
            if (iCurrentSuiteModel)
                {
                iCurrentSuiteModel->SetVisibleL(EFalse);
                }
            if( iMmExtManager )
                {
                iMmExtManager->ExecuteActionL(TUid::Null(), KCommandDeleteDialog, NULL );
                }
            }
            break;
        case ERemoveLiwObjects:
            {
            THashMapIter< TInt, CMmWidgetContainer* > iterator( iContainerMap );
            while( iterator.NextKey() )
                {
                CMmWidgetContainer** container = iterator.CurrentValue();
                (*container)->RemoveLiwObjects();
                }
            }
            break;
        default:
            ASSERT( false );
        }
    DEBUG(("_Mm_:CMmAppUi::NotifyUiRefreshL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::RefreshToolbarL()
    {
    DEBUG(("_Mm_:CMmAppUi::RefreshToolbarL IN"));
    HandleToolbarVisibilityL();
    UpdateToolbarL();
    DEBUG(("_Mm_:CMmAppUi::RefreshToolbarL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::RefreshUiPanesL( TBool aReset )
    {
    // refresh status pane
    CAknTitlePane* titlePane =
        static_cast<CAknTitlePane*>(
                StatusPane()->ControlL(
                        TUid::Uid( EEikStatusPaneUidTitle ) ) );

    if( aReset )
        {
        titlePane->SetTextL( KNullDesC );
        }
    else
        {
        if ( IsEditMode() && iCurrentSuiteModel )
            {
            HBufC* title = NULL;
            if ( IsRootdisplayedL() )
                {
                title = StringLoader::LoadLC( R_ORG_ROOT_EDITING_TITLE );
                }
            else
                {
                title = StringLoader::LoadLC(
                        R_ORG_FOLDER_EDITING_TITLE,
                        iCurrentSuiteModel->Title() );
                }
            ASSERT( title );
            titlePane->SetTextL( title->Des() );
            CleanupStack::PopAndDestroy( title );
            }
        else if ( iCurrentSuiteModel && iCurrentContainer )
            {
            // refresh status pane
            titlePane->SetTextL( iCurrentSuiteModel->Title() );

            // inform AVKON about current suite to allow proper handling
            // of AppKey and FSW
            RProperty::Set( KMMenuPSCat, KMMenuLastViewKey, iCurrentSuiteModel->SuiteName() );
            }
        else
            {
            titlePane->SetTextL( KNullDesC );
            }
        }

    RefreshCbaL();
    SetMiddleSoftKeyL();

    StatusPane()->DrawNow();
    Cba()->DrawNow();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::RefreshCbaL()
    {
    if ( ( iDummyContainer && iDummyContainer->IsVisible() ) ||
            !iCurrentSuiteModel )
        {
        // R_MENU_SOFTKEYS_EMPTY__EMPTY looks much better than
        // R_AVKON_SOFTKEYS_EMPTY
        Cba()->SetCommandSetL( R_MENU_SOFTKEYS_EMPTY__EMPTY );
        }
    else
        {
        if ( IsEditMode() )
            {
            Cba()->SetCommandSetL( AknLayoutUtils::PenEnabled() ?
                R_AVKON_SOFTKEYS_OPTIONS_DONE :
                R_AVKON_SOFTKEYS_OK_EMPTY__OK );
            }
        else if ( IsRootdisplayedL() )
            {
            Cba()->SetCommandSetL( AknLayoutUtils::PenEnabled() ?
                 R_AVKON_SOFTKEYS_OPTIONS_EXIT :
                 R_AVKON_SOFTKEYS_OPTIONS_EXIT__SELECT );
            }
        else if ( iCurrentSuiteModel &&
                iCurrentSuiteModel->SuiteName().Compare(KParamFolderSuite) )
            {
            Cba()->SetCommandSetL( AknLayoutUtils::PenEnabled() ?
                 R_MENU_SOFTKEYS_OPTIONS_BEXIT :
                 R_MENU_SOFTKEYS_OPTIONS_BEXIT__SELECT );
            }
        else
            {
            Cba()->SetCommandSetL( AknLayoutUtils::PenEnabled() ?
                R_AVKON_SOFTKEYS_OPTIONS_BACK :
                R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TKeyResponse CMmAppUi::HandleKeyPressedL( const TKeyEvent &aKeyEvent,
            TEventCode aType )
    {
    TKeyResponse resp = EKeyWasNotConsumed;

    // handling enter key - touch & non touch
    if ( ( aKeyEvent.iScanCode == EStdKeyEnter ||
            aKeyEvent.iScanCode == EStdKeyNkpEnter ||
           aKeyEvent.iScanCode == EStdKeyDevice3 ) && aType == EEventKeyDown )
        {
        if ( iCurrentContainer->IsHighlightVisible() )
            {
            DEBUG(("_Mm_:CMmAppUi::HandleKeyPressedL - enter"));
            ForwardEventToHNL( KKeyIdSelect );
            resp = EKeyWasConsumed;
            }
        }

    //handling enter key - non touch / edit mode
    if ( ( aKeyEvent.iScanCode == EStdKeyEnter ||
            aKeyEvent.iScanCode == EStdKeyDevice3 ) &&
            aType == EEventKeyDown &&
            IsEditMode() && !Layout_Meta_Data::IsPenEnabled() )
        {
        DEBUG(("_Mm_:CMmAppUi::HandleKeyPressedL - enter in edit mode"));
		HandleCommandL( EAknSoftkeyOk );
        resp = EKeyWasConsumed;
        }

    // handle the key exactly as container does (the same conditions)
    if ( ( aKeyEvent.iRepeats > 0 && aType == EEventKey ) ||
            ( aKeyEvent.iRepeats == 0 && aType == EEventKeyUp ) )
        {
        TBool navigationEvent =
            aKeyEvent.iScanCode == EStdKeyRightArrow ||
            aKeyEvent.iScanCode == EStdKeyLeftArrow ||
            aKeyEvent.iScanCode == EStdKeyUpArrow ||
            aKeyEvent.iScanCode == EStdKeyDownArrow;

        if ( navigationEvent )
            {
            DEBUG(("_Mm_:CMmAppUi::HandleKeyPressedL - navi event: %d",
                aKeyEvent.iScanCode));
            MMPERF(("Rocker navigation - START"));
            UpdateToolbarL();
            SetMiddleSoftKeyL();
            resp = EKeyWasConsumed;
            MMPERF(("Rocker navigation - END"));
            }
        else if ( aKeyEvent.iScanCode == EStdKeyBackspace )
            {
            // handle clear key event
            DEBUG(("_Mm_:CMmAppUi::HandleKeyPressedL - clear key"));
            ForwardEventToHNL( KKeyIdClear );
            resp = EKeyWasConsumed;
            }
        }

    if( aType == EEventUser )
        {
        if ( aKeyEvent.iScanCode == EStdKeyNull &&
                iCurrentContainer->IsHighlightVisible() )
            {
            DEBUG(("_Mm_:CMmAppUi::HandleKeyPressedL - User press -> forward key:select to HN"));
            UpdateToolbarL();
            SetMiddleSoftKeyL();
            ForwardEventToHNL( KKeyIdSelect );
            resp = EKeyWasConsumed;
            }
        }

    return resp;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmAppUi::HandleDragStartL( TInt aModelItemIndex )
    {
    MMPERF(("CMmAppUi::HandleDragStartL - START"));

    if ( IsEditMode() )
        {

        TInt modelId = iCurrentSuiteModel->IdByIndex( aModelItemIndex );
        if (modelId != KErrNotFound)
            {
            CHnItemModel *itModel = iCurrentSuiteModel->GetItemModel( modelId );
            if (itModel->GetItemType() == EItemTypeParentFolder)
                {
                iCurrentContainer->CancelDragL( EFalse );
                }
            else
                {
                iItemDragged = aModelItemIndex;
                iIdDragged = iCurrentSuiteModel->IdByIndex( aModelItemIndex );
                }
            }
        }
    MMPERF(("CMmAppUi::HandleDragStartL - STOP"));
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void CMmAppUi::HandleDragOverL( TInt /* aModelItemIndex */ )
    {
    MMPERF(("CMmAppUi::HandleDragOverL - START"));

    MMPERF(("CMmAppUi::HandleDragOverL - STOP"));
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmAppUi::HandleDraggedIndexUpdatedL( TInt  aModelItemIndex  )
    {
    MMPERF(("CMmAppUi::HandleDraggedIndexUpdatedL - START"));
    iItemDragged = aModelItemIndex;
    MMPERF(("CMmAppUi::HandleDraggedIndexUpdatedL - STOP"));
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmAppUi::HandleDragStopL( TInt aModelItemIndex )
    {
    MMPERF(("CMmAppUi::HandleDragStopL - START"));
    if ( IsEditMode() )
        {
        TInt itemId = iCurrentSuiteModel->IdByIndex( aModelItemIndex );
        TMcsItemType typeCurr =
            iCurrentSuiteModel->GetItemType( aModelItemIndex );
        TBool isOverFolder = ( AknLayoutUtils::PenEnabled() &&
            ( typeCurr == EItemTypeParentFolder || typeCurr == EItemTypeFolder ) );
        TBool isDeleteLocked = (itemId != KErrNotFound) ?
                    iCurrentSuiteModel->GetItemModel( itemId )->IsDeleteLocked():
                    EFalse;

        TBuf8< KMaxLength > beforeCustomId;
        beforeCustomId.Num( KErrNotFound );
        if ( aModelItemIndex + 1 < iCurrentContainer->NumberOfItems() )
            {
            beforeCustomId.Num( iCurrentSuiteModel->GetItemModel(
                    iCurrentSuiteModel->IdByIndex( aModelItemIndex + 1 ) )->CustomId() );
            }

        TBuf8< KMaxLength > draggedCustomId;
        draggedCustomId.Num( KErrNotFound );
        CHnItemModel* draggedModel = iCurrentSuiteModel->GetItemModel( iIdDragged );
        if (draggedModel)
            {
            draggedCustomId.Num( iCurrentSuiteModel->GetItemModel( iIdDragged )->CustomId() );
            }

        CLiwGenericParamList* eventParameters = CLiwGenericParamList::NewL();
        CleanupStack::PushL( eventParameters );
        CHnMdBaseKey* tempKeys = HnMdKeyFactory::CreateL(
                HnEvent::KEventArgNamespace8(), KKeyTypeMap(), KNullDesC8() );
        CleanupStack::PushL( tempKeys );

        CHnMdBaseKey* baseKey = HnMdKeyFactory::CreateL(
                HnEvent::KEventArgNamespace8(), KKeyTypeMap(), KNullDesC8() );
        CleanupStack::PushL( baseKey );
        baseKey->AddSubKeyL( HnMdKeyFactory::CreateL( HnEvent::KDroppedBefore8(),
                KKeyTypeInteger(), beforeCustomId ) );
        baseKey->AddSubKeyL( HnMdKeyFactory::CreateL( HnEvent::KDragged8(),
                KKeyTypeInteger(), draggedCustomId ) );
        CleanupStack::Pop( baseKey );
        tempKeys->AddSubKeyL( baseKey );

        tempKeys->ToGenericParamListL( *eventParameters );
        CleanupStack::PopAndDestroy( tempKeys );

        if ( iIdDragged != itemId
                && isOverFolder && !isDeleteLocked)
            {
            iCurrentSuiteModel->RemoveItemL( iIdDragged );
            if ( iItemDragged < aModelItemIndex )
                {
                // indices of all items after iItemDragged have been
                // decreased when the dragged item got removed from the
                // current suite; prevent highlighted item change
                iCurrentSuiteModel->SetSuiteHighlightL( aModelItemIndex - 1 );
                iCurrentContainer->SetManualHighlightL(
                        iCurrentContainer->GetHighlight() - 1, EFalse );
                }

            iCurrentContainer->HandleItemRemovalL();
            iCurrentContainer->CancelDragL( EFalse );

            if( typeCurr == EItemTypeParentFolder )
                {
                StartLayoutSwitchFullScreen( EMenuCloseFolderEffect );
                }
            else
                {
                StartLayoutSwitchFullScreen( EMenuOpenFolderEffect );
                }
            iHNInterface->TriggerHnEventL( KKeyIdMoveInto, itemId, eventParameters );
            iMakeHightlightedItemFullyVisible = ETrue;
            }
        else if ( iIdDragged == itemId )
            {
            iCurrentContainer->CancelDragL( ETrue );
            TBool result = iHNInterface->TriggerHnEventL( KKeyIdMove, itemId, eventParameters );
            // fix for: edit->move, then LSK->Up->LSK->LSK FAST... (short options show up)
            if ( result != KErrNone && !AknLayoutUtils::PenEnabled() )
				{
				CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
				TLiwGenericParam command(KHnRequest, TLiwVariant( KEvaluateMdModel));
				paramList->AppendL(command);
				HandleRequestL(*paramList);
				CleanupStack::PopAndDestroy(paramList);
				}
            }
        else
            {
            iCurrentContainer->CancelDragL( ETrue );
            if (!AknLayoutUtils::PenEnabled())
                {
                CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
                TLiwGenericParam command(KHnRequest, TLiwVariant( KEvaluateMdModel));
                paramList->AppendL(command);
                HandleRequestL(*paramList);
                CleanupStack::PopAndDestroy(paramList);
                }
            }

        CleanupStack::PopAndDestroy( eventParameters );
        }
    MMPERF(("CMmAppUi::HandleDragStopL - STOP"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleEventDraggingActionedL()
    {
    SetMiddleSoftKeyL();
    UpdateToolbarL();
    iKeyClickLocked = ETrue;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleListBoxEventL( CEikListBox* /*aListBox*/,
        MEikListBoxObserver::TListBoxEvent aEventType )
    {
    DEBUG(("_Mm_:CMmAppUi::HandleListBoxEventL IN"));
    DEBUG(("\t_Mm_: aEventType = %d", (TInt) aEventType ) );

    TInt currentHighlight = iCurrentContainer->GetHighlight();

    DEBUG(("\t_Mm_: CurrentHighlight = %d", currentHighlight ) );

    if ( aEventType == MEikListBoxObserver::EEventItemDraggingActioned )
        {
        DEBUG8(("\t_Mm_:CMmAppUi::HandleListBoxEventL EEventItemDraggingActioned"));
        HandleEventDraggingActionedL();
        }
    else if ( aEventType == MEikListBoxObserver::EEventPenDownOnItem )
        {
        DEBUG(("\t_Mm_:CMmAppUi::HandleListBoxEventL EEventItemClicked"));
        HandleHighlightItemPenDownL( iCurrentContainer->GetHighlight() );
        }
    else if ( aEventType == MEikListBoxObserver::EEventItemDoubleClicked )
        {
        DEBUG(("\t_Mm_:CMmAppUi::HandleListBoxEventL EEventItemDoubleClicked"));
        HandleHighlightItemDoubleClickedL( iCurrentContainer->GetHighlight() );
        }
    else if ( aEventType == MEikListBoxObserver::EEventItemSingleClicked )
        {
        DEBUG(("\t_Mm_:CMmAppUi::HandleListBoxEventL EEventItemSingleClicked"));
        HandleHighlightItemSingleClickedL(  iCurrentContainer->Widget()->CurrentItemIndex() );
        }
    else if ( aEventType == MEikListBoxObserver::EEventPanningStarted )
    	{
    	iKeyClickLocked = ETrue;
    	}

    DEBUG(("_Mm_:CMmAppUi::HandleListBoxEventL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleLongTapEventL( const TPoint& aPenEventLocation )
	{
    TBool popupMenuDisplayed(EFalse);
	if (iPopupMenu)
    	{
    	delete iPopupMenu;
    	iPopupMenu = NULL;
    	}
    iPopupMenu = CAknStylusPopUpMenu::NewL(this,aPenEventLocation);

    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iPopupMenu->SetPosition( aPenEventLocation,
                CAknStylusPopUpMenu::EPositionTypeRightBottom );
        }
    else
        {
        iPopupMenu->SetPosition( aPenEventLocation,
                CAknStylusPopUpMenu::EPositionTypeLeftBottom );
        }

    if( iCurrentSuiteModel == iHNInterface->GetLastSuiteModelL()
    		&& iCurrentContainer->IsHighlightVisible()
    		&& iCurrentContainer->GetSuiteModelL()->GetItemModelsCount() > 1 )
    	{
		MMPERF(("CMmAppUi::DynInitMenuPaneL - START"));
		//fill the main menu structure, look for cascade menus
		//reset the helper hash map
		iCascadeMenuMap.Close();

		MHnMenuItemModelIterator* menuIterator =
			iCurrentSuiteModel->GetMenuStructureL(
				iCurrentSuiteModel->IdByIndex(
						iCurrentContainer->GetHighlight() ) );

		// check if there is a menu structure available
		// for the specified item
		if ( menuIterator )
			{
			//create item sorting helper objects
			RArray<TInt> positionArray;
			CleanupClosePushL( positionArray );
			RHashMap<TInt, CEikMenuPaneItem::SData> menuItemMap;
			CleanupClosePushL( menuItemMap );

            while ( menuIterator->HasNextSpecific() )
                {
                CHnMenuItemModel* childItem = menuIterator->GetNextSpecific();
				CEikMenuPaneItem::SData childData;
				childData.iCommandId = childItem->Command();
				childData.iText = childItem->NameL().
					Left( CEikMenuPaneItem::SData::ENominalTextLength );
				childData.iFlags = 0;
				childData.iCascadeId = 0;

				positionArray.AppendL( childItem->Position() );
				menuItemMap.InsertL( childItem->Position(), childData );
                }

			positionArray.Sort();

			//add items in correct order
			for ( TInt i = 0; i < positionArray.Count(); ++i )
				{
				iPopupMenu->
					AddMenuItemL(
						menuItemMap.FindL( positionArray[i] ).iText,
						menuItemMap.FindL( positionArray[i] ).iCommandId );
				}

			if (positionArray.Count()>0)
				{
				iPopupMenu->ShowMenu();
				popupMenuDisplayed = ETrue;
				}
			CleanupStack::PopAndDestroy( &menuItemMap );
			CleanupStack::PopAndDestroy( &positionArray );
			}
		MMPERF(("CMmAppUi::DynInitMenuPaneL - STOP"));
		}

	if ( !popupMenuDisplayed && iCurrentContainer
			&& iCurrentContainer == TopFocusedControl() )
		{
		iCurrentContainer->HandleTopFocusL( EFalse );
		HandleHighlightItemSingleClickedL(
				iCurrentContainer->Widget()->CurrentItemIndex() );
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleHighlightItemDoubleClickedL( TInt aIndex )
    {
    if ( iKeyClickLocked )
    	return;

    if ( iCurrentSuiteModel->WidgetType() == EListWidget
        && iCurrentContainer->GetPreviousHighlight() == aIndex )
        {
        ForwardEventToHNL( KKeyIdSelect, aIndex );
        }
    else if ( iCurrentSuiteModel->WidgetType() == EGridWidget && FlipOpen() )
		{
		ForwardEventToHNL( KKeyIdSelect, aIndex );
		}
    else if ( iCurrentSuiteModel->WidgetType() == EGridWidget
            && IsEditMode() && iCurrentContainer->GetPreviousHighlight() == aIndex
            && !iCurrentContainer->IsDraggable())
        {
        ForwardEventToHNL( KKeyIdSelect, aIndex );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleHighlightItemPenDownL( TInt /* aIndex */ )
    {
    DEBUG(("CMmAppUi::HandleHighlightItemPenDownL - IN"));
    UpdateToolbarL();
    SetMiddleSoftKeyL();
    iKeyClickLocked = EFalse;
    DEBUG(("CMmAppUi::HandleHighlightItemPenDownL - OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleHighlightItemSingleClickedL( TInt aIndex )
    {
    if ( !iKeyClickLocked && !IsFaded() )
        {
        ForwardEventToHNL( KKeyIdSelect, aIndex );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ResetContainerMap()
    {
    THashMapIter< TInt, CMmWidgetContainer* > iterator( iContainerMap );
    while( iterator.NextKey() )
        {
        CMmWidgetContainer** container = iterator.CurrentValue();
        delete *container;
        iterator.RemoveCurrent();
        }
    iContainerMap.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ResetContainerMapToRootL()
    {
    TInt err( KErrNone );
    CHnSuiteModel* currentSuite = iHNInterface->GetLastSuiteModelL();

    // there is a loaded suite configuration
    if( currentSuite )
        {
        TInt id( currentSuite->GetItemsOrder()->GetSuiteId() );
        TRAP( err, iCurrentContainer = iContainerMap.FindL( id ) );

        // configuration loaded but there is no container
        if( err != KErrNone )
            {
            HandleSuiteModelInitializedL( currentSuite );
            }
        else if( iContainerMap.Count() > 0 )
            {
            THashMapIter< TInt, CMmWidgetContainer* > iterator( iContainerMap );
            while( iterator.NextKey() )
                {
                CMmWidgetContainer** container = iterator.CurrentValue();
                if( iCurrentContainer != *container )
                    {
                    RemoveFromStack( *container );
                    iGarbage.AppendL( *container );
                    iterator.RemoveCurrent();
                    }
                }
            }
        }
    else
        {
        ResetContainerMap();
        InitializeL();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleMessageL( const TDesC8& aMessage )
    {
    if ( IsEditMode() )
        {
        SetEditModeL( EFalse );
        }

    if ( aMessage.Find( KRootWithPref ) != KErrNotFound )
        {
        RemoveFromStack( iCurrentContainer );
        iDummyContainer->MakeVisible( ETrue );
        RefreshCbaL();
        iGarbage.ResetAndDestroy();
        ResetContainerMap();
	    iCurrentSuiteModel = NULL;
	    iCurrentContainer = NULL;
	    }

    TRAPD( err, iHNInterface->LoadSuitesFromUriL( aMessage ) );

    if ( err && iCurrentContainer && ( aMessage.Find( KSetFocusWithPref ) == KErrNotFound ) )
        {
        HandleSuiteEventL( ESuiteModelInitialized, iHNInterface->GetLastSuiteModelL() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::DynInitMenuPaneL( TInt aResourceId,
                                 CEikMenuPane* aMenuPane )
    {
    aMenuPane->EnableMarqueeL( ETrue );
    if( iCurrentSuiteModel == iHNInterface->GetLastSuiteModelL() )
        {
        switch ( aResourceId )
            {
            case R_MAIN_MENU_PANE:
                {
                MMPERF(("CMmAppUi::DynInitMenuPaneL - START"));
                //fill the main menu structure, look for cascade menus
                //reset the helper hash map
                iCascadeMenuMap.Close();

                MHnMenuItemModelIterator* menuIterator = NULL;
                TBool ignoreItemSpecific( EFalse );
                if ( iCurrentSuiteModel )
                    {
                    TInt itemId( KErrNotFound );
                    TBool suiteModelHasItems = iCurrentContainer->GetSuiteModelL()->GetItemModelsCount() > 1;
					TBool highlightVisible = iCurrentContainer->IsHighlightVisible();
					if ( !highlightVisible && suiteModelHasItems )
						{
//						if there is no highlight, but there are items, show menuitems for logically
//						current suite highlight fetched from suite model.
						TInt suiteHighlight = iCurrentSuiteModel->GetSuiteHighlight();
						ASSERT( suiteHighlight != KErrNotFound );
						itemId = iCurrentSuiteModel->IdByIndex( suiteHighlight );
						ignoreItemSpecific = ETrue;
						}
					else
						{
	                    TBool idByContainer = highlightVisible && suiteModelHasItems;
	                    itemId = idByContainer ?
	                        iCurrentSuiteModel->IdByIndex( iCurrentContainer->GetHighlight() ) :
	                        iCurrentSuiteModel->IdByIndex( KErrNotFound );
						}
                    menuIterator = iCurrentSuiteModel->GetMenuStructureL( itemId );
                }

                // check if there is a menu structure available
                // for the specified item
                if ( menuIterator )
                    {
                    //create item sorting helper objects
                    RArray<TInt> positionArray;
                    CleanupClosePushL( positionArray );
                    RHashMap<TInt, CEikMenuPaneItem::SData> menuItemMap;
                    CleanupClosePushL( menuItemMap );

                    //get original (rss defined) items positions
                    for ( TInt i = 0; i < aMenuPane->NumberOfItemsInPane(); ++i )
                        {
                        //multiply to make space for suite defined entries
                        //first item has 100th conceptual position
                        positionArray.AppendL( ( i + 1 ) * KMenuPosMultiplier );
                        //store items, position as the key
                        menuItemMap.InsertL( ( i + 1 ) * KMenuPosMultiplier,
                            aMenuPane->
                                ItemData( aMenuPane->MenuItemCommandId( i ) ) );
                        }

                    //get custom menu items and their positions
                    while ( menuIterator->HasNext() )
                        {
                        CHnMenuItemModel* menuItem = menuIterator->GetNext();
                        if ( !menuItem->IsItemSpecific() || !ignoreItemSpecific )
                        	{
                        	CEikMenuPaneItem::SData menuData;
							menuData.iCommandId = menuItem->Command();
							menuData.iText = menuItem->NameL().
								Left( CEikMenuPaneItem::SData::ENominalTextLength );
							menuData.iFlags = 0;

							//check for children
							MHnMenuItemModelIterator* childIterator =
								menuItem->GetMenuStructure();
							if ( childIterator->HasNext() )
								{
								//this is a cascade item
								//one menu item can contain only one cascade menu
								//check if there are available cascade menu containers
								TInt freeResource = GetNextCascadeMenuResourceId();
								if ( freeResource != KErrNotFound )
									{
									//error checking
									if( !iCascadeMenuMap.Insert( freeResource,
																childIterator ) )
										{
										//add item only if there is an
										//available resource
										menuData.iCascadeId = freeResource;
										}
									}
								}
							else
								{
								//normal entry
								menuData.iCascadeId = 0;
								}
							positionArray.AppendL( menuItem->Position() );
							menuItemMap.InsertL( menuItem->Position(), menuData );
                        	}
                        }

                    aMenuPane->Reset();
                    positionArray.Sort();

                    //add items in correct order
                    for ( TInt i = 0; i < positionArray.Count(); ++i )
                        {
                        aMenuPane->
                            AddMenuItemL(
                                ( menuItemMap.FindL( positionArray[i] ) ) );
                        }

                    CleanupStack::PopAndDestroy( &menuItemMap );
                    CleanupStack::PopAndDestroy( &positionArray );
                    }
                MMPERF(("CMmAppUi::DynInitMenuPaneL - STOP"));
                }
                break;

            case R_CASCADE_MENU_PANE_1:
            case R_CASCADE_MENU_PANE_2:
            case R_CASCADE_MENU_PANE_3:
                {
                //realod cascade menu map
                ReloadCascadeMenuMapL();
                //fill the cascade menu panes
                MHnMenuItemModelIterator* childIteratorPtr =
                    iCascadeMenuMap.FindL( aResourceId );

                // check if child iterator available
                if ( childIteratorPtr )
                    {
                    childIteratorPtr->Reset();

                    //create item sorting helper objects
                    RArray<TInt> positionArray;
                    CleanupClosePushL( positionArray );
                    RHashMap<TInt, CEikMenuPaneItem::SData> menuItemMap;
                    CleanupClosePushL( menuItemMap );

                    while ( childIteratorPtr->HasNext() )
                        {
                        CHnMenuItemModel* childItem = childIteratorPtr->GetNext();

                        CEikMenuPaneItem::SData childData;
                        childData.iCommandId = childItem->Command();
                        childData.iText = childItem->NameL().
                            Left( CEikMenuPaneItem::SData::ENominalTextLength );
                        childData.iFlags = 0;
                        childData.iCascadeId = 0;

                        positionArray.AppendL( childItem->Position() );
                        menuItemMap.InsertL( childItem->Position(), childData );
                        }

                    positionArray.Sort();
                    //add items in correct order
                    for ( TInt i = 0; i < positionArray.Count(); ++i )
                        {
                        aMenuPane->AddMenuItemL(
                            ( menuItemMap.FindL( positionArray[i] ) ) );
                        }

                    CleanupStack::PopAndDestroy( &menuItemMap );
                    CleanupStack::PopAndDestroy( &positionArray );
                    }
                }
                break;

            default:
                break;
            }
        }
    else if( aResourceId == R_MAIN_MENU_PANE )
        {
        aMenuPane->SetItemDimmed( EMmOptionsExit, ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ReloadCascadeMenuMapL()
    {
    iCascadeMenuMap.Close();
    MHnMenuItemModelIterator* menuIterator = NULL;
    if ( iCurrentSuiteModel )
        {
        TBool idByContainer = iCurrentContainer->IsHighlightVisible() &&
                iCurrentContainer->GetSuiteModelL()->GetItemModelsCount() > 1;
        TInt itemId = idByContainer ?
            iCurrentSuiteModel->IdByIndex( iCurrentContainer->GetHighlight() ) :
            iCurrentSuiteModel->IdByIndex( KErrNotFound );
        menuIterator = iCurrentSuiteModel->GetMenuStructureL( itemId );
        }

    if ( menuIterator )
        {
        while ( menuIterator->HasNext() )
            {
            CHnMenuItemModel* menuItem = menuIterator->GetNext();

            MHnMenuItemModelIterator* childIterator =
                menuItem->GetMenuStructure();
            if ( childIterator->HasNext() )
                {
                TInt freeResource = GetNextCascadeMenuResourceId();
                if ( freeResource != KErrNotFound )
                    {
                    iCascadeMenuMap.Insert( freeResource, childIterator );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmAppUi::GetNextCascadeMenuResourceId()
    {
    //if needed add additional placeholders in matrixmenu.rss
    //next add additional cases in this method and the
    //CMmAppUi::DynInitMenuPaneL

    TInt freeResource( KErrNotFound );
    switch ( iCascadeMenuMap.Count() )
        {
        case 0:
            {
            freeResource = R_CASCADE_MENU_PANE_1;
            }
            break;
        case 1:
            {
            freeResource = R_CASCADE_MENU_PANE_2;
            }
            break;
        case 2:
            {
            freeResource = R_CASCADE_MENU_PANE_3;
            }
            break;
        default:
            break;
        }
    return freeResource;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleWidgetChangeRefreshL(
        CMmWidgetContainer* aWidgetContainer )
    {
    DEBUG(("_Mm_:CMmAppUi::HandleWidgetChangeRefreshL IN"));

    CMmWidgetContainer* previousContainer = iCurrentContainer;
    if (previousContainer)
        {
        previousContainer->SetObserver(NULL);
        RemoveFromStack(previousContainer);
        }

    iCurrentContainer = aWidgetContainer;
    iCurrentContainer->Widget()->View()->SetDisableRedraw(ETrue);
    iDummyContainer->MakeVisible( ETrue );
    RefreshCbaL();
    iCurrentContainer->SetZoom( iZoom );
    iCurrentContainer->SetFlipOpenL( FlipOpen() );
    iCurrentContainer->SetEditModeL( IsEditMode() );
    iCurrentContainer->SetSuiteModelL( iCurrentSuiteModel );
    iCurrentContainer->HandleResourceChange( KAknsMessageSkinChange );
    iCurrentContainer->HandleResourceChange( KUidValueCoeColorSchemeChangeEvent );
    iCurrentContainer->SetEmptyTextL( iCurrentSuiteModel->EmptyText() );
    iCurrentContainer->SetHasFocusL( iHasFocus );
    iCurrentContainer->SetIsFaded( IsFaded() );
    iCurrentContainer->SetObserver( this );

    if ( previousContainer && previousContainer->WidgetType() != iCurrentContainer->WidgetType()
            && previousContainer->GetSuiteModelL() == iCurrentContainer->GetSuiteModelL() )
        {
        iCurrentSuiteModel->SetSuiteHighlightL( KErrNotFound );
        }

    AddToStackL( iCurrentContainer );
    if ( previousContainer )
        {
        previousContainer->MakeVisible( EFalse );
        }

    DEBUG(("_Mm_:CMmAppUi::HandleWidgetChangeRefreshL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleNoWidgetChangeRefreshL()
    {
    if ( iCurrentSuiteModel )
        {
        iCurrentContainer->Widget()->View()->SetDisableRedraw(ETrue);
        iCurrentContainer->SetZoom( iZoom );
        iCurrentContainer->SetFlipOpenL( FlipOpen() );
        iCurrentContainer->SetEditModeL( IsEditMode() );
        iCurrentContainer->SetSuiteModelL( iCurrentSuiteModel );
        iCurrentContainer->SetEmptyTextL( iCurrentSuiteModel->EmptyText() );
        iCurrentContainer->SetHasFocusL( iHasFocus );
        iCurrentContainer->SetIsFaded( IsFaded() );
        iCurrentContainer->SetObserver(this);
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::IsEditMode()
    {
    return (iEditModeStatus == EEditMode
            || iEditModeStatus == ETransitionToEditMode);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandlePresentationChangeL(
        CMmWidgetContainer* aWidgetContainer )
    {
    DEBUG(("_Mm_:CMmAppUi::HandlePresentationChangeL IN"));

    if ( iToolbar && iToolbar->IsShown() )
        {
        iToolbar->SetToolbarVisibility( EFalse );
        }

    if( iCurrentContainer != aWidgetContainer )
        {
        if( iCurrentContainer && aWidgetContainer &&
                iCurrentContainer->WidgetType() != aWidgetContainer->WidgetType() )
            {
            StartLayoutSwitchFullScreen();
            }
        HandleWidgetChangeRefreshL( aWidgetContainer );
        }
    else
        {
        HandleNoWidgetChangeRefreshL();
        }

    if( iCurrentContainer )
        {
        iCurrentContainer->SetRect(ClientRect());
        iDummyContainer->SetRect(ClientRect());
        if( IsForeground() )
            {
            // should be called before MakeVisible (and after SetRect or SetupLayout,
            // so that default highlight is displayed correctly when zoomed)
            ApplyHighlightFromModelL();

            // There is defferences in code because behaviour in non-touch edit mode
            // and non-touch edit mode is different.
            if (!AknLayoutUtils::PenEnabled())
                {
                // While starting non-touch edit mode we have to scroll view in case when
                // higlighted item is placed on top/bottow row of view and there
                // are more rows over/under one to show all needed indicator arrows.
                // DrawNow() method caused jumping both scrollbar and view to old positions.
                iCurrentContainer->MakeVisible(ETrue);
                iCurrentContainer->Widget()->View()->SetDisableRedraw(EFalse);
                iCurrentContainer->SetEditModeL(IsEditMode());
                iDummyContainer->MakeVisible(EFalse);
                }
            else
                {
                // For touch normal mode we have to redraw container in case
                // we back "parent folder" which was edited in edit mode.
                // Thanks to DrawNow() method we avoid drawing items's backdrop icons
                // and switching between edit mode's and normal mode's views
                iCurrentContainer->SetEditModeL(IsEditMode());
                iCurrentContainer->MakeVisible(ETrue);

                // cache widget position so that we can restore it after DrawNow(), which
                // changes it for no apparent reason when using list view.
                iCurrentContainer->CacheWidgetPosition();

                // draw container before effects gets snapshot
                iCurrentContainer->DrawNow();
                iCurrentContainer->Widget()->View()->SetDisableRedraw(EFalse);
                iDummyContainer->MakeVisible(EFalse);

                // restore the correct widget position
                iCurrentContainer->RestoreWidgetPosition();
                }

            // refresh changed items only
            iCurrentContainer->DrawView();
            RefreshUiPanesL();
            RefreshToolbarL();
            }
        else
            {
            iCurrentContainer->SetupWidgetLayoutL();
            iCurrentContainer->Widget()->View()->SetDisableRedraw( EFalse );
            ApplyHighlightFromModelL();
            }
        }

    MMPERF(("CMmAppUi::HandlePresentationChangeL - model swapped"));
    DEBUG(("_Mm_:CMmAppUi::HandlePresentationChangeL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::OfferToolbarEventL( TInt aCommand )
    {
    TInt itemId = iCurrentSuiteModel->IdByIndex( iCurrentContainer->GetHighlight() );
    CHnItemModel* itemModel = iCurrentSuiteModel->GetItemModel( itemId );
    ASSERT( itemModel );
    const CHnToolbarModel* toolbarModel = itemModel->GetToolbarModel();
    ASSERT( toolbarModel );
    const CHnButtonModel* buttonModel = toolbarModel->GetButton( aCommand );
    ASSERT( buttonModel );

    ForwardEventToHNL( buttonModel->GetEventId() );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmAppUi::ExecuteExtensionActionL( const TUid aUid , const TDesC& aCommand,
                CLiwGenericParamList* aEventParamList )
    {
    return iMmExtManager->ExecuteActionL( aUid, aCommand, aEventParamList);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleToolbarVisibilityL()
    {
    TBool suiteModelHasToolbar =
        iCurrentSuiteModel && iCurrentSuiteModel->HasToolbar();

    if ( suiteModelHasToolbar )
        {
        if ( iToolbar )
            {
            // could have gotten hidden during view change
            iToolbar->SetToolbarVisibility( ETrue );
            }
        else
            {
            iToolbar = CAknToolbar::NewL( R_GLOBAL_TOOLBAR );
            iToolbar->SetToolbarObserver( this );
            iToolbar->SetToolbarVisibility( ETrue );

            // substract the toolbar from display's visible area
            TRect remainingArea( ClientRect() );
            if ( !Layout_Meta_Data::IsLandscapeOrientation() )
                remainingArea.Resize( 0, -iToolbar->Size().iHeight );
            iCurrentContainer->SetRect( remainingArea );
            }
        }
    else if ( !suiteModelHasToolbar && iToolbar )
        {
        iToolbar->SetToolbarVisibility( EFalse );
        delete iToolbar;
        iToolbar = NULL;
        iCurrentContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::UpdateToolbarL()
    {
    // drop toolbar handling if there should be none
    if ( iToolbar && iCurrentSuiteModel->HasToolbar() )
        {
        iToolbar->RemoveItem( EMmToolbarControl1 );
        iToolbar->RemoveItem( EMmToolbarControl2 );
        iToolbar->RemoveItem( EMmToolbarControl3 );

        TInt itemId = iCurrentSuiteModel->IdByIndex( iCurrentContainer->Widget()->CurrentItemIndex() );
        CHnItemModel* itemModel = iCurrentSuiteModel->GetItemModel( itemId );
        if ( !itemModel )
            {
            return;
            }
        const CHnToolbarModel* toolbarModel = itemModel->GetToolbarModel();

        CAknButton* button( NULL );
        for ( TInt i = 0; i < KNumOfButtonsInToolbar; ++i )
            {
            const CHnButtonModel* buttonModel = toolbarModel->GetButton( i );
            // get button icon and help text from model
            if ( buttonModel )
                {
                CGulIcon* originalIcon = buttonModel->GetIcon();
                CGulIcon* iconCopy( NULL );
                if ( originalIcon && originalIcon->Bitmap() )
                    {
                    // create a copy of the icon if exists,
                    // CAknButton takes ownership of the icon but we
                    // want to reuse it,
                    // also duplicate bitmaps for proper scaling
                    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
                    CleanupStack::PushL( bitmap );
                    bitmap->Duplicate( originalIcon->Bitmap()->Handle() );

                    CFbsBitmap* mask( NULL );
                    if ( originalIcon->Mask() )
                        {
                        mask = new (ELeave) CFbsBitmap;
                        CleanupStack::PushL( mask );
                        mask->Duplicate( originalIcon->Mask()->Handle() );
                        }

                    iconCopy = CGulIcon::NewL( bitmap, mask );
                    if ( mask )
                        {
                        CleanupStack::Pop( mask );
                        }
                    CleanupStack::Pop( bitmap );
                    CleanupStack::PushL( iconCopy );
                    }
                button = CAknButton::NewL( iconCopy,
                        NULL,
                        NULL,
                        NULL,
                        KNullDesC,
                        buttonModel->GetButtonText(),
                        0,
                        0 );
                if ( iconCopy )
                    {
                    CleanupStack::Pop( iconCopy );
                    }
                CleanupStack::PushL( button );
                button->SetDimmed( buttonModel->GetDimmed() );
                }
            else
                {
                button = CAknButton::NewLC();
                }
            iToolbar->AddItemL( button,
                    EAknCtButton,
                    EMmToolbarControl1 + i,
                    0 );
            CleanupStack::Pop( button );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SetEditModeL( TBool aIsEditMode )
	{
	MMPERF(("CMmAppUi::SetEditModeL %d - START",aIsEditMode));
    if ( IsEditMode() != aIsEditMode && iCurrentSuiteModel )
        {

        // stops moving items. This must be called when leaving non-touch edit mode to save
        // the current visible order of the items. In non-touch it is called only once when completing editing

        iHNInterface->SetEditModeL(aIsEditMode);
        iCurrentContainer->StopMovingL();

        if ( aIsEditMode )
        	{
        	iEditModeStatus = ETransitionToEditMode;
        	}
        else
        	{
        	if ( iEditModeStatus == ETransitionToEditMode )
        		{
        		iEditModeStatus = EFastTransitionFromEditMode;
        		}
        	else
        		{
        		iEditModeStatus = ETransitionFromEditMode;
        		}
        	}

        HandleHighlightOffsetL( aIsEditMode ? EOffsetNext : EOffsetPrevious );
        iCurrentSuiteModel->SetSuiteHighlightL(
                AdjustEditModeHighlightL( iCurrentSuiteModel->GetSuiteHighlight() ) );

        // update the container at the end when highlight is proper,
        // this will ensure correct focus/unfocus action handling
        iCurrentContainer->CacheWidgetPosition();
        iCurrentContainer->SetEditModeL( IsEditMode() );

        if ( iCurrentContainer->IsHighlightVisible() )
            {
            iMakeHightlightedItemFullyVisible = ETrue;
            }

        RefreshUiPanesL();

        //disable redraw so that no redrawing events coming from window server cause redrawing until
        //everything is ready to draw. Then iCurrentContainer->DrawNow in HandlePresentationChangeL is called
        if (IsEditMode())
            {
            iCurrentContainer->Widget()->View()->SetDisableRedraw(ETrue);
            }
        }

    MMPERF(("CMmAppUi::SetEditModeL - STOP"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleWsEventL( const TWsEvent& aEvent,
                               CCoeControl* aDestination )
    {
    DEBUG(("_Mm_:CMmAppUi::HandleWsEventL %d - IN", aEvent.Type()));

    if ( aEvent.Type() == KAknUidValueEndKeyCloseEvent )
        {
        // return when the red key was pressed
        MMPERF(("End key pressed"));
        DEBUG(("_Mm_:CMmAppUi::HandleWsEventL - End key"));
        ExitMatrix( EExitToIdle );
        return;
        }

    TEventCode type = static_cast< TEventCode >( aEvent.Type() );

    if ( ( type == EEventFocusLost || type == KAknFullOrPartialForegroundLost )
    		&& iCurrentContainer )
        {
        iCurrentContainer->CacheWidgetPosition();
        }

    // base's HandleWsEventL needs to be called before foreground handling,
    // otherwise the screen saver will be turned off
    CAknAppUi::HandleWsEventL( aEvent, aDestination );

    // refreshes toolbar when pen down event was invoked
    if ( type == EEventPointer )
        {
        if ( iCurrentSuiteModel )
            {
            RefreshToolbarL();
            }
        }
    else if ( type == EEventFocusGained )
        {
        HandleFocusGainedL();
        }
    else if ( type == EEventFocusLost )
    	{
    	HandleFocusLostL();
        }
    else if ( type == KAknFullOrPartialForegroundGained )
        {
        HandleFullOrPartialForegroundGainedL();
        }
    else if( type == KAknFullOrPartialForegroundLost )
        {
        HandleFullOrPartialForegroundLostL();
        }

    DetectOptionsMenuOpenDisplayChangeL();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TErrorHandlerResponse CMmAppUi::HandleError(TInt aError,
                                            const SExtendedError& /*aExtErr*/,
                                            TDes& /*aErrorText*/,
                                            TDes& /*aContextText*/ )
    {
    if( aError == KErrNoMemory )
        {
        TRAP_IGNORE( ResetToInitialStateL( ) );
        }
    return EErrorNotHandled;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleSuiteEventL (
        THnCustomSuiteEvent aCustomSuiteEvent, CHnSuiteModel* aModel )
    {

    DEBUG(("_Mm_:CMmAppUi::HandleSuiteEventL %d - IN",aCustomSuiteEvent));
    switch ( aCustomSuiteEvent )
        {
        case ESuitePushedToStack:
            {
            aModel->RegisterSuiteObserverL( this, EPriorityMuchLess );
            }
            break;
        case ESuitePoppedFromStack:
            {

            }
            break;
        case ESuiteModelInitialized:
            {
            HandleSuiteModelInitializedL( aModel );
            }
            break;
        default:
            break;
        }

    if ( iCurrentSuiteModel == aModel )
        {
        //We are interested in the following event
        //only if they come from the current suite.
        TBool redraw = (iScreenOn && IsForeground()) ? ETrue : EFalse;
        switch (aCustomSuiteEvent)
            {
            case ESuiteModelDestroyed:
                {
                iCurrentContainer->PrepareForGarbage();
                iGarbage.AppendL(iCurrentContainer);
                iContainerMap.Remove(
                        iCurrentSuiteModel->GetItemsOrder()->GetSuiteId());
                RemoveFromStack(iCurrentContainer);
                iDummyContainer->MakeVisible( ETrue );
                iCurrentContainer->SetObserver(NULL);
                iCurrentSuiteModel = NULL;
                iCurrentContainer = NULL;
                }
                break;
            case ESuiteItemsAdded:
                {
                //we need consistent view/state before item is added
                //so draggind should be cancelled
                if( IsEditMode() && iCurrentContainer->IsDraggable() )
                    {
                    iCurrentContainer->CancelDragL( EFalse );
                    iCurrentContainer->DrawNow();
                    }
                HandleNumberOfItemsChangedL( EItemsAdded );
                }
                break;
            case ESuiteItemsRemoved:
                {
                //we need consistent view/state before item is removed
                //so draggind should be cancelled
                if( IsEditMode() && iCurrentContainer->IsDraggable() )
                    {
                    iCurrentContainer->CancelDragL( EFalse );
                    iCurrentContainer->DrawNow();
                    }
                HandleNumberOfItemsChangedL( EItemsRemoved );
                }
                break;
            case ESuiteItemsUpdated:
            	{
				ClearTransitionFromEditModeFlag();
                iCurrentContainer->DrawView();
                }
                break;
            case ESuiteHighlightChanged:
                {
                iCurrentContainer->SetManualHighlightL(
                        iCurrentSuiteModel->GetSuiteHighlight(), redraw );
                }
                break;
            default:
                break;
            }
        }
    DEBUG(("_Mm_:CMmAppUi::HandleSuiteEventL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ClearTransitionFromEditModeFlag()
	{
	if ( iEditModeStatus == ETransitionFromEditMode ||
		 iEditModeStatus == EFastTransitionFromEditMode )
		{
		iEditModeStatus = ENoEditMode;
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmAppUi::AdjustEditModeHighlightL( TInt aOriginalHighlight )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( !iCurrentContainer->IsHighlightVisible()
                && iEditModeStatus == ETransitionToEditMode )
            {
            aOriginalHighlight = KErrNotFound ;
            }
        }
    return aOriginalHighlight;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleOutOfMemoryL()
    {
    if( !iOutOfMemoryHandler )
        {
        iOutOfMemoryHandler = CMmNoMemory::NewL();
        }
    iOutOfMemoryHandler->Start();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ResetToInitialStateL()
    {
    iDummyContainer->MakeVisible( ETrue );
    iDummyContainer->DrawNow();
    RefreshCbaL();

    if (iCurrentSuiteModel)
        {
        iCurrentSuiteModel->UnregisterSuiteObserver( this );
        }
    RemoveFromStack( iCurrentContainer );
    ResetContainerMap();
    iGarbage.ResetAndDestroy();
    delete iHNInterface;
    iHNInterface = NULL;
    delete iMmExtManager;
    iMmExtManager = NULL;
    delete iSliderEventSubscriber;
    iSliderEventSubscriber = NULL;
    delete iTemplateLibrary;
    iTemplateLibrary = NULL;

    iCurrentSuiteModel = NULL;
    iCurrentContainer = NULL;
    iContainerMap.Close();
    iEditModeStatus = ENoEditMode;

    GfxTransEffect::AbortFullScreen();
    iIsKastorEffectStarted = EFalse;

    RefreshUiPanesL();
    Cba()->MakeVisible( EFalse );
    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );

    iHNInterface = CHnEngine::NewL( *this );
    iTemplateLibrary = CMmTemplateLibrary::NewL();
    InitializeL();

    iMmExtManager = CMMExtensionManager::NewL( *this );

    iKeyboardFlipStatus = EPSHWRMFlipStatusUninitialized;

    iScreenOn = ETrue;
    iSkinChangeNeeded = EFalse;
    iSkinChangeInProgress = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleSuiteModelInitializedL( CHnSuiteModel* aModel )
    {
    StatusPane()->MakeVisible( ETrue );
    Cba()->MakeVisible( ETrue );

    if ( aModel == iHNInterface->GetLastSuiteModelL() )
        {
        TBool showOpenFolderEffect(iCurrentSuiteModel
                && (iCurrentSuiteModel->CustomId() != aModel->CustomId()
                        || iCurrentSuiteModel->WidgetType()
                                != aModel->WidgetType()));
        if( showOpenFolderEffect )
            {
            StartLayoutSwitchFullScreen( EMenuOpenFolderEffect );
            }

        if ( iCurrentSuiteModel )
            {
            iCurrentSuiteModel->SetVisibleL( EFalse );
            DEBUG16(("\t\t_Mm_:SetVisible EFalse - %S",
                    &(iCurrentSuiteModel->SuiteName())));
            }
        iCurrentSuiteModel = aModel;
        ShowSuiteL();
        iGarbage.ResetAndDestroy();

       	if ( iEditModeStatus == ETransitionToEditMode )
            {
            iEditModeStatus = EEditMode;
            }
        else if ( iEditModeStatus == ETransitionFromEditMode )
            {
            iEditModeStatus = ENoEditMode;
            }

        if (iScreenOn && IsForeground())
            {
            iCurrentSuiteModel->SetVisibleL( ETrue );
            DEBUG16(("\t\t_Mm_:SetVisible ETrue - %S",
                    &(iCurrentSuiteModel->SuiteName())));
            }

        HideMenuPaneIfVisibleL();
        EndFullScreen();
        }
    else
        {
        if( iHNInterface->SuiteModelLoadedL(
                aModel->GetItemsOrder()->GetSuiteId() ) )
            {
            CMmWidgetContainer* containerToLoad = NULL;
            TBool created( CreateNewContainerL( aModel, containerToLoad ) );
            if( created )
                {
                containerToLoad->Widget()->View()->SetDisableRedraw( ETrue );
                containerToLoad->SetFlipOpenL( FlipOpen() );
                containerToLoad->SetEditModeL( IsEditMode() );
                containerToLoad->SetSuiteModelL( aModel );
                containerToLoad->SetEmptyTextL( aModel->EmptyText() );
                containerToLoad->SetObserver( this );
                containerToLoad->SetRect( ClientRect() );
                containerToLoad->MakeVisible( EFalse );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HideMenuPaneIfVisibleL()
    {
    if ( iEikonEnv->AppUiFactory()->MenuBar()->IsDisplayed() )
        {
        iEikonEnv->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ExitMatrix( TExitType aExitType )
    {
    DEBUG(("_Mm_:CMmAppUi::ExitMatrix - IN"));
    DEBUG(("\t_Mm_:Exit type: %d",aExitType));

    if ( aExitType == EExitReally )
        {
        // Calling PrepareHomescreenForMatrixExitL instead of ShowHomescreenL
        // allows for avoiding a bug that would occur if appkey was pressed
        // immediately after exiting the menu via Options->Exit.
        // The bug would be that on pressing the appkey homescreen would be
        // displayed and then the menu would appear again on the screen for
        // a fraction of second causing an ugly and confusing flicker effect.
        TRAP_IGNORE( PrepareHomescreenForMatrixExitL() );
//        ShowHomescreenL( aExitType );
        //if we got exit cmd from OS, let's really exit.
        Exit();
        }
    else
        {
        GfxTransEffect::AbortFullScreen();
        iIsKastorEffectStarted = EFalse;
        StartLayoutSwitchFullScreen( AknTransEffect::EApplicationExit );

        TRAP_IGNORE( ShowHomescreenL( aExitType ) );
        TRAP_IGNORE( CleanupForExitL( EExitKeyRed ) );
        }

    DEBUG(("_Mm_:CMmAppUi::ExitMatrix - OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::CleanupForExitL( TExitKeyType aExitKey )
    {
    DEBUG(("_Mm_:CMmAppUi::CleanupForExitL - IN"));

    // closing all dialogs opened in ui extensions before exit
    iMmExtManager->ExecuteActionL( TUid::Null(), KCommandDeleteDialog, NULL );
    if( IsDisplayingDialog() )
        {
        AknDialogShutter::ShutDialogsL( *iEikonEnv );
        }

    if ( IsEditMode() && iCurrentContainer )
        {
        DEBUG(("\t_Mm_:Edit Mode turned off"));
        iCurrentContainer->CancelDragL( EFalse );
        SetEditModeL( EFalse );
        }

    // reset model - revert to root if current view is not a suite view
    CHnSuiteModel* model = iHNInterface->GetLastSuiteModelL();
    if ( model && aExitKey == EExitKeyApplication )
        {
        TBool topSuiteIsBeingEvaluated =
            !model->GetItemsOrder()->IsSuiteReadyToShow();
        TBool topSuiteChanged = ResetToRootL();
        model = NULL; // ResetToRootL might have deleted the model
        TBool presentationChangeExpected = topSuiteChanged || topSuiteIsBeingEvaluated;

        TBool mustDrawImmediately =
            !presentationChangeExpected || !iDummyContainer->IsVisible();

        if ( iCurrentContainer && mustDrawImmediately )
            {
            DEBUG(("\t_Mm_:Top item index reset"));
            iCurrentContainer->ResetWidgetPosition();
            iCurrentContainer->Widget()->UpdateScrollBarsL();
			iCurrentContainer->MakeVisible( ETrue );
			iCurrentContainer->DrawNow();
            }
        }

    DEBUG(("_Mm_:CMmAppUi::CleanupForExitL - OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ShowHomescreenL( TExitType  aExitType  )
    {
    TInt appToShowUid(0);
    TInt idleid(0);
    if (aExitType == EExitToIdle)
        {
        User::LeaveIfError(RProperty::Get(KPSUidAiInformation, KActiveIdleUid,
                appToShowUid));
        OpenAppL(TUid::Uid(appToShowUid));
        }
    else if (KErrNone == RProperty::Get(KPSUidUikon, KUikVideoCallTopApp,
            idleid))
        {
        // Possible error code not relevant, as we have valid id anyway
        if (idleid != KVideoCallAppUid) // idle or phone
            {
            OpenAppL(TUid::Uid(KPhoneAppUid));
            }
        else
            {
            CreateActivateViewEventL(TVwsViewId(TUid::Uid(idleid),
                    TUid::Uid(idleid)), KNullUid, KNullDesC8());
            }
        }
    else
        {
        OpenAppL(TUid::Uid(KPhoneAppUid));
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::PrepareHomescreenForMatrixExitL()
    {
    TBool succeeded( EFalse );

    CAknTaskList* taskList = CAknTaskList::NewL( iCoeEnv->WsSession() );
    TApaTask task = taskList->FindRootApp( TUid::Uid( KPhoneAppUid ) );
    delete taskList;

    if ( task.Exists() )
        {
        succeeded = !iCoeEnv->WsSession().SetWindowGroupOrdinalPosition(
                    task.WgId(), 1 );
        }

    if( !succeeded )
        {
        ShowHomescreenL( EExitReally );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::OpenAppL( const TUid aUid )
    {
    DEBUG(("_Mm_:CMmAppUi::OpenAppL - IN"));
    DEBUG(("\t_Mm_:aUid: 0x%x",aUid.iUid));

    // Get the correct application data
    CAknTaskList* taskList = CAknTaskList::NewL( iCoeEnv->WsSession() );
    TApaTask task = taskList->FindRootApp( aUid );
    delete taskList;

    if ( task.Exists() )
        {
        CAknSgcClient::MoveApp( task.WgId(), ESgcMoveAppToForeground );
        }
    else
        {
        // Task doesn't exist, launch a new instance of an application
        TApaAppInfo appInfo;
        TApaAppCapabilityBuf capabilityBuf;
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );
        CleanupClosePushL( lsSession );
        User::LeaveIfError( lsSession.GetAppInfo( appInfo, aUid ) );
        User::LeaveIfError(
                lsSession.GetAppCapability( capabilityBuf, aUid ) );

        TApaAppCapability& caps = capabilityBuf();
        TFileName appName = appInfo.iFullName;
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL( appName );

        if ( caps.iLaunchInBackground )
            {
            cmdLine->SetCommandL( EApaCommandBackground );
            }
        else
            {
            cmdLine->SetCommandL( EApaCommandRun );
            }

        User::LeaveIfError( lsSession.StartApp( *cmdLine ) );

        CleanupStack::PopAndDestroy( cmdLine );
        CleanupStack::PopAndDestroy( &lsSession );
        }

    DEBUG(("_Mm_:CMmAppUi::OpenAppL - OUT"));
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::InitializeL()
    {
    DEBUG(("_MM_:CMmAppUi::InitializeL IN"));
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

    HBufC* suiteName = StringLoader::LoadLC( R_MENU_TITLE );

    paramList->AppendL( TLiwGenericParam( KParamSuiteName8,
            TLiwVariant( KParamFolderSuite ) ) );
    map->InsertL( KParamSuiteName8, TLiwVariant( suiteName ) );
    map->InsertL( KParamFolderId8, TLiwVariant( KParamFolderId ) );
    map->InsertL( KParamRemoveLocked8, TLiwVariant( KParamFalse ) );
    map->InsertL( KParentFolderId8, TLiwVariant( KParamFolderId ) );
    map->InsertL( KTmpParentFolderId8, TLiwVariant( KParamFolderId ) );
    paramList->AppendL( TLiwGenericParam( KParams8, TLiwVariant( map ) ) );

    iHNInterface->InitializeL( *paramList );

    CleanupStack::PopAndDestroy( suiteName );
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( paramList );
    DEBUG(("_MM_:CMmAppUi::InitializeL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SetMiddleSoftKeyL()
    {
    DEBUG(("_Mm_:CMmAppUi::SetMiddleSoftKeyL - IN"));
    if ( iCurrentSuiteModel && !AknLayoutUtils::PenEnabled() && !IsEditMode() )
    	{
    	DEBUG(("\t_Mm_:suite highlight: %d",
    	            iCurrentSuiteModel->GetSuiteHighlight()));

		TBool idByContainer = iCurrentContainer->IsHighlightVisible() &&
				iCurrentContainer->GetSuiteModelL()->GetItemModelsCount() > 1;
		TInt itemId = idByContainer ?
			iCurrentSuiteModel->IdByIndex( iCurrentContainer->GetHighlight() ) :
			iCurrentSuiteModel->IdByIndex( KErrNotFound );
		CHnItemModel* itemModel = iCurrentSuiteModel->GetItemModel( itemId );

		CHnButtonModel* mskModel = NULL;

		if ( itemModel )
			{
			mskModel = itemModel->GetMiddleSoftKey();
			}

		if ( mskModel )
			{
			TInt event = (mskModel->GetEventId() == KErrNotFound) ?
					KKeyIdSelect : mskModel->GetEventId();
			Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
					event, mskModel->GetButtonText());
			}
		else
			{
			// reset to default
			RefreshCbaL();
			}
    	}
    DEBUG(("_Mm_:CMmAppUi::SetMiddleSoftKeyL - OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::StartLayoutSwitchFullScreen( TInt aKastorEffect )
    {
    if( ( !iIsKastorEffectStarted && iScreenOn && IsForeground() ) ||
            aKastorEffect == AknTransEffect::EApplicationStart ||
            aKastorEffect == AknTransEffect::EApplicationExit )
        {
        DEBUG(("_MM_:CMmAppUi::StartLayoutSwitchFullScreen Foreground"));
        TUid uid1( KUidMatrixMenuApp );
        TUid uid2( KUidMatrixMenuApp );
        if ( aKastorEffect == AknTransEffect::EApplicationExit )
            {
            uid2 = TUid::Null();
            }

        AknTransEffect::TParamBuffer params = AknTransEffect::GfxTransParam(
                uid1, uid2, AknTransEffect::TParameter::EFlagNone );

        GfxTransEffect::BeginFullScreen( aKastorEffect, TRect(),
                AknTransEffect::EParameterType, params );

        iIsKastorEffectStarted = ETrue;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::EndFullScreen()
    {
    if( iIsKastorEffectStarted && iScreenOn )
        {
        DEBUG(("_MM_:CMmAppUi::EndFullScreen Foreground"));
        GfxTransEffect::EndFullScreen();
        iIsKastorEffectStarted = EFalse;
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleRequestL( const CLiwGenericParamList& aParam,
                         CLiwGenericParamList* aOutput )
    {
    iHNInterface->HandleRequestL( aParam, aOutput );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::PropertyChangedL(TInt aValue)
    {
    iKeyboardFlipStatus = (EPSHWRMFlipStatus)aValue;

    if ( iCurrentContainer )
        {
        if( iKeyboardFlipStatus == EPSHWRMFlipOpen )
            {
            TBool wasHighlightVisible = iCurrentContainer->IsHighlightVisible();
            iCurrentContainer->SetFlipOpenL( ETrue );
            if ( IsForeground() && !IsEditMode() && !wasHighlightVisible )
                {
                iCurrentContainer->SetDefaultHighlightL( ETrue );
                }
            }
        else if( iKeyboardFlipStatus == EPSHWRMFlipClosed )
            {
            iCurrentContainer->SetFlipOpenL( EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::FlipOpen()
    {
    return iKeyboardFlipStatus == EPSHWRMFlipOpen;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SetZoom( TAknUiZoom aZoom )
    {
    if ( iZoom != aZoom)
        {
        if ( iCurrentContainer )
            {
            iCurrentContainer->HandleZoomChanged( aZoom );
            }
        }
    iZoom = aZoom;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMmAppUi::ResetToRootL()
    {
    TBool resetConsumed( EFalse );
    if( iHNInterface && iCurrentSuiteModel &&
            !IsRootdisplayedL() )
        {
        RemoveFromStack( iCurrentContainer );
        iDummyContainer->MakeVisible( ETrue );
        iHNInterface->HandleBackEventL(
                iCurrentSuiteModel->SuiteName(),
                iHNInterface->GetSuiteModelsCountL() - 1 );
        ResetContainerMapToRootL();
        if( iCurrentContainer )
            {
            iCurrentContainer->ResetWidgetPosition();
            iCurrentContainer->SetZoom( iZoom );
            AddToStackL( iCurrentContainer );
            }
        RefreshUiPanesL( ETrue );
        resetConsumed = ETrue;
        }
    return resetConsumed;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SkinContentChanged()
    {
    iSkinChangeNeeded = ETrue;
    iSkinChangeInProgress = ETrue;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SkinConfigurationChanged(
        const TAknsSkinStatusConfigurationChangeReason aReason)
    {
        DEBUG(("_Mm_:CMmAppUi::SkinConfigurationChanged IN - aReason:%d iSkinChangeNeeded:%d iSkinChangeInProgress:%d", aReason, iSkinChangeNeeded, iSkinChangeInProgress));
    switch (aReason)
        {
        case EAknsSkinStatusConfigurationMerged://      = 1,
            {
            iSkinChangeNeeded = ETrue;
            iSkinChangeInProgress = ETrue;
            }
            break;
        case EAknsSkinStatusConfigurationDeployed://    = 2,
            {
            iSkinChangeInProgress = EFalse;
            if (IsForeground())
                {
                TRAPD( err, RefreshIconsL());
                if (!err)
                    {
                    iSkinChangeNeeded = ETrue;
                    }
                }
            }
            break;
        default:
            {

            }
        }
        DEBUG(("_Mm_:CMmAppUi::SkinConfigurationChanged OUT - aReason:%d iSkinChangeNeeded:%d iSkinChangeInProgress:%d", aReason, iSkinChangeNeeded, iSkinChangeInProgress));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::SkinPackageChanged(const TAknsSkinStatusPackageChangeReason /*aReason*/)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::RefreshIconsL()
    {
    iSkinChangeNeeded = EFalse;
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
    TLiwGenericParam command(KHnRequest, TLiwVariant( KRefreshIcons));
    paramList->AppendL(command);
    HandleRequestL(*paramList);
    CleanupStack::PopAndDestroy(paramList);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleHighlightOffsetL( TInt aOffset )
    {
    if( AknLayoutUtils::PenEnabled() )
        {
        iHNInterface->TriggerHnEventL( KKeyIdMoveHighlight, aOffset, NULL );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleNumberOfItemsChangedL( TItemsChangeType aChange )
    {
    iCurrentContainer->NumberOfItemsChangedL( aChange );
    TBool scrollConsumed( EFalse );
    scrollConsumed =
        iCurrentContainer->ScrollToItemL( iCurrentSuiteModel->GetSuiteHighlight() );
    if ( !scrollConsumed || !AknLayoutUtils::PenEnabled() )
        {
        iCurrentContainer->DrawNow();
        }
    RefreshUiPanesL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::ApplyHighlightFromModelL()
    {
    if ( iCurrentSuiteModel && iCurrentContainer )
        {
        TInt highlightedItemIndex = iCurrentSuiteModel->GetSuiteHighlight();
        iCurrentContainer->RestoreWidgetPosition();

        iCurrentContainer->SetManualHighlightL(
                highlightedItemIndex, EFalse );
        if ( iMakeHightlightedItemFullyVisible &&
                !iCurrentContainer->ItemIsFullyVisible( highlightedItemIndex ) )
            {
            iCurrentContainer->ScrollToItemL( highlightedItemIndex );
            }
        iCurrentContainer->CacheWidgetPosition();
        iMakeHightlightedItemFullyVisible = EFalse;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleFocusGainedL()
	{
	DEBUG(("_Mm_:CMmAppUi::HandleWsEventL "
			"- EEventFocusGained"));

	iHasFocus = ETrue;

    // Tricky: lack of iCurrentSuiteModel indicates that suite evaluation is in
    // progress - do not call HandlePresentationChangeL if evalution has not
    // finished.
	if ( iCurrentContainer && !iCurrentContainer->IsVisible() &&
	        iCurrentSuiteModel )
		{
		HandlePresentationChangeL( iCurrentContainer );
		}

	if ( iCurrentSuiteModel && iCurrentContainer )
		{
		iDummyContainer->MakeVisible( EFalse );
		iCurrentContainer->MakeVisible( ETrue );
		iCurrentContainer->DrawNow();
		}
	if( IsForeground() )
		{
		RefreshUiPanesL();
		}
	if ( iCurrentContainer )
	    {
	    iCurrentContainer->SetHasFocusL( ETrue );
	    }
	if ( iCurrentSuiteModel )
        {
        iCurrentSuiteModel->SetVisibleL( ETrue );
        }
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleFocusLostL()
	{
	DEBUG(("_Mm_:CMmAppUi::HandleWsEventL "
			"- EEventFocusLost"));

	iHasFocus = EFalse;

	if( iCurrentContainer )
		{
		//This is needed in case some popup is displayed
		//while touching item in grid. Highlight
		//should be reset to normal then.
		if( AknLayoutUtils::PenEnabled() )
			{
			if( iCurrentContainer->WidgetType() == EGridWidget
				&& !FlipOpen() && !IsEditMode() )
				{
				iCurrentContainer->Widget()->View()->ItemDrawer()->
					SetFlags( CListItemDrawer::EDisableHighlight );
				}
			iCurrentContainer->Widget()->View()->
			ItemDrawer()->ClearFlags( CListItemDrawer::EPressedDownState );
			iCurrentContainer->Widget()->View()->DrawItem(
							  iCurrentContainer->Widget()->CurrentItemIndex() ) ;
			}
		if( IsEditMode() && iCurrentContainer->IsDraggable() )
			{
			iCurrentContainer->CancelDragL( EFalse );
			iCurrentContainer->DrawNow();
			}
		iCurrentContainer->SetHasFocusL( EFalse );
		}
	if ( iCurrentSuiteModel )
		{
		iCurrentSuiteModel->SetVisibleL( EFalse );
		}

	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleFullOrPartialForegroundGainedL()
	{
		DEBUG(("_Mm_:CMmAppUi::HandleWsEventL "
						"- KAknFullOrPartialForegroundGained"));
	if (iCurrentContainer && iCurrentSuiteModel )
		{
		iCurrentContainer->HandleForegroundGainedL();
		iDummyContainer->MakeVisible( EFalse );
		iCurrentContainer->MakeVisible( ETrue );
		RefreshUiPanesL();
		}
	if (iSkinChangeNeeded && !iSkinChangeInProgress)
		{
		MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
		if (skinInstance && !skinInstance->IsUpdateInProgress())
			{
			RefreshIconsL();
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::HandleFullOrPartialForegroundLostL()
	{
	DEBUG(("_Mm_:CMmAppUi::HandleWsEventL "
			"- KAknFullOrPartialForegroundLost"));
	iMmExtManager->ExecuteActionL(
			TUid::Null(), KCommandDeleteDialog, NULL );
	if ( iCurrentContainer )
		{
		iCurrentContainer->HandleBackgroundGainedL();
		if ( !IsRootdisplayedL() || IsEditMode() ||
		        iEditModeStatus == ETransitionFromEditMode )
			{
			iDummyContainer->MakeVisible( ETrue );
			RefreshUiPanesL( ETrue );
			iCurrentContainer->MakeVisible( EFalse );
			iDummyContainer->DrawNow();
			}
		if ( IsRootdisplayedL() )
			{
			iCurrentContainer->RestoreWidgetPosition();
			iCurrentContainer->CacheWidgetPosition();
			}
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmAppUi::DetectOptionsMenuOpenDisplayChangeL()
    {
    CEikMenuBar* menuBar = iAppUiFactory->MenuBar();
    TBool optionsMenuIsOpen = menuBar && menuBar->IsDisplayed();
    if ( !!iOptionsMenuIsOpen != !!optionsMenuIsOpen )
        {
        iOptionsMenuIsOpen = optionsMenuIsOpen;
        if ( iCurrentContainer )
        	{
            iCurrentContainer->HandleOptionsMenuVisibilityChangeL(
            		iOptionsMenuIsOpen );
        	}
        }
    }

// End of File
