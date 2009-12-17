/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       It is the control container for the AI plugins
*
*/


// INCLUDE FILES
#include <bldvariant.hrh> //feature flags
#include <barsread.h>
#include <coecntrl.h>
#include <e32std.h>
#include <bautils.h>
#include <e32property.h>
#include <connect/sbdefs.h> // pub&sub keys and categories for backup and restore
#include <aknappui.h>
#include <AknsDrawUtils.h>
#include <AknsLayeredBackgroundControlContext.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <AIContainerDomainCRKeys.h>
#include <activeidle2domainpskeys.h>
#include "aicontainer.hrh"
#include "caiplugincontainercontrol.h"
#include "ExtrStandbyScr.h"


const TInt KActOnSendKey = 1;

// ================= MEMBER FUNCTIONS =======================

CAIPluginContainerControl* CAIPluginContainerControl::NewL(CCoeControl *aContainer)
    {
    CAIPluginContainerControl* self =
            new (ELeave) CAIPluginContainerControl() ;

    CleanupStack::PushL( self );
    self->ConstructL(aContainer);
    CleanupStack::Pop(self);

    return self;
    }

CAIPluginContainerControl::~CAIPluginContainerControl()
    {

    AknsUtils::DeregisterControlPosition(this);

    delete iBackupObserver;
    iBackupObserver = NULL;

    if(iNotifyHandler)
        {
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;
    delete iAIRepository;
    delete iBgContext;

    DestroyPlugins() ;
    iPlugins.ResetAndDestroy();
    iPlugins.Close();
    REComSession::FinalClose() ;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::SetDimmed
// ---------------------------------------------------------
//
void CAIPluginContainerControl::SetDimmed(TBool aDimmed)
    {
    MakeVisible(!aDimmed);
    if (iContentStatusChangedNotification)
        {
        iContentStatusChangedNotification = EFalse;
        CalculateRectPlugins();
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::CountComponentControls
// Counts the component controls, so it returns one.
// ---------------------------------------------------------
//
TInt CAIPluginContainerControl::CountComponentControls() const
    {
    TInt pluginsCount = 0;

    for ( TInt index = 0; index < iPlugins.Count(); index++ )
        {
        TPluginInfo* plugin = iPlugins[index];

        if (plugin->iPlugin) pluginsCount++ ;
        }

    return pluginsCount;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::ComponentControl
// Returns components by index.
// ---------------------------------------------------------
//
CCoeControl* CAIPluginContainerControl::ComponentControl(TInt aIndex) const
    {
    if (iPlugins.Count())
        {
        TInt pluginsPos = -1;

        for ( TInt index = 0; index < iPlugins.Count(); index++ )
            {
            TPluginInfo* plugin = iPlugins[index];

            if (plugin->iPlugin) pluginsPos++ ;

            if (pluginsPos == aIndex) return iPlugins[index]->iPlugin;
            }
        }
    return NULL ;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::SizeChanged
// Changes the sizes of the components.
// ---------------------------------------------------------
//
void CAIPluginContainerControl::SizeChanged()
    {
    AknsUtils::RegisterControlPosition(this);

    if(iBgContext)
        {
        iBgContext->SetRect(Rect());
        iBgContext->SetParentPos(PositionRelativeToScreen());
        }


    if (!CountComponentControls())
        {
        return ;
        }

    if ((iPrevRect.IsEmpty() || iPrevRect != Rect()))
        {
        CalculateRectPlugins() ;
        iPrevRect = Rect() ;
        }
    else
        {
//        FocusOnDefaultFocusingPlugin(EDrawNow);
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::FocusChanged
// ---------------------------------------------------------
//
void CAIPluginContainerControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if (iCurrentPlugin)
        {
        if (IsFocused())
            {
            TRAP_IGNORE(iCurrentPlugin->iPlugin->PrepareForFocusGainL());
            }
        else
            {
            TRAP_IGNORE(iCurrentPlugin->iPlugin->PrepareForFocusLossL());
            }
        // The flag must be drawnow because
        // the plugin focusing have to be happer/disapper
        iCurrentPlugin->iPlugin->SetFocus(IsFocused(),EDrawNow) ;
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::Draw
// ---------------------------------------------------------
//
void CAIPluginContainerControl::Draw( const TRect& /*aRect*/ ) const
    {
     if (IsDimmed())
        {
        return ;
        }
    TRect rect( Rect() );

    CWindowGc& gc = SystemGc();

    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this);
    MAknsSkinInstance* aknsInst = AknsUtils::SkinInstance();

    // Draw background starting from correct offset
    AknsDrawUtils::Background( aknsInst, cc, this, gc, rect );
    }


// -----------------------------------------------------------------------------
// CAIPluginContainerControl::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CAIPluginContainerControl::MopSupplyObject(TTypeUid aId)
    {
    if( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::OfferKeyEventL
// Processes the key event for the incall view.
// ---------------------------------------------------------
//
TKeyResponse CAIPluginContainerControl::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {

    if (iCurrentPlugin)
        {
        if (CountComponentControls() > 1)
            {
            switch( aKeyEvent.iScanCode )
                {
                case EStdKeyUpArrow:
                case EStdKeyDownArrow:
                    {
                    if( aKeyEvent.iCode == EKeyUpArrow )
                        {
                        ChangeFocusPluginL(EAIFocusUp);
                        }
                    else if( aKeyEvent.iCode == EKeyDownArrow )
                        {
                        ChangeFocusPluginL(EAIFocusDown);
                        }
                    return EKeyWasConsumed;
                    }
                }
            }

        if( aKeyEvent.iCode == EKeyPhoneEnd )
            {
            FocusOnDefaultFocusingPlugin(EDrawNow);
            }
        return iCurrentPlugin->iPlugin->OfferKeyEventL(aKeyEvent,aType);
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HandleResourceChange
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandleResourceChange( TInt aType )
    {
    switch(aType)
        {
        case KAknsMessageSkinChange:
            {
            iPrevRect.SetSize(TSize(0,0));
            }
            break;

        case KEikDynamicLayoutVariantSwitch:
            {
            TRAP_IGNORE(UpdateBackgroundL());
            SetRect(static_cast<CAknAppUi*>(iEikonEnv->AppUi())->ClientRect());
            }
            break;

         default:
            break;
         }

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::StartL
// ---------------------------------------------------------
//
void CAIPluginContainerControl::StartL()
    {
    // Avoid double start calls
    if(!iStarted)
        {
        iStarted = ETrue;
        ReadPluginsConfigurationsL() ;
        LoadPluginsL();
        SizeChanged();
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::SetFullScreenMode
// ---------------------------------------------------------
//
void CAIPluginContainerControl::SetFullScreenMode(TBool aFullScreenMode)
    {
    iIsFullScreenMode = aFullScreenMode;
    TRAP_IGNORE(UpdateBackgroundL());
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HandleControlEventL
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
    {
    if ( aEventType == EEventStateChanged )
        {
        if (IsDimmed())
            {
            iContentStatusChangedNotification = ETrue ;
            return ;
            }
        CalculateRectPlugins() ;
        //DrawNow();
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HandlePropertyChange
// From PropertyObserver
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandlePropertyChange(const TUid& /*aCategory*/,
                                                     const TUint& /*aKey*/,
                                                     const TInt& aValue)
    {
    // see \epoc32\include\connect\sbdefs.h for enum descriptions

    const TUint backupMask( conn::KBURPartTypeMask ^ conn::EBURNormal );

    if( aValue & backupMask ) // any type of backup or restore operation
        {
        // backup or restore started
        // -> unload plugins
        if (CountComponentControls())
            {
            DestroyPlugins();
            DrawNow() ;
            }
        }
    else // aValue == conn::EBURUnset || aValue & conn::EBURNormal
        {
        // backup or restore ended
        // -> load plugin again
        if (CountComponentControls() == 0)
            {
            TRAP_IGNORE(
                    ReadPluginsConfigurationsL();
                    LoadPluginsL()
            );
            CalculateRectPlugins();
            }
        }
    }

// -----------------------------------------------------------------------------
// CAIPluginContainerControl::HandleNotifyGeneric
// -----------------------------------------------------------------------------
//
void CAIPluginContainerControl::HandleNotifyGeneric(TUint32 aId)
    {
    if(aId != NCentralRepositoryConstants::KInvalidNotificationId)
        {
        if(aId != KAINumOfOperatorPlugins)
            {
            RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_LSKLocked, KNullDesC );
            RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_RSKLocked, KNullDesC );
            DestroyPlugins();
            TRAP_IGNORE(ReadPluginsConfigurationsL(); LoadPluginsL(););            
            CalculateRectPlugins();
            RProperty::Set( KPSUidAiInformation, KActiveIdleExtHS_PluginConfChange, 1 );
            }
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::ReadPluginsConfigurationsL
// ---------------------------------------------------------
//
void CAIPluginContainerControl::ReadPluginsConfigurationsL()
    {
    TInt numOfPlugins = 0;
    TInt error(KErrNone);
    error = iAIRepository->Get( KAINumOfOperatorPlugins, numOfPlugins );

    if( error == KErrNone )
        {
        // Reset old array
        iPlugins.ResetAndDestroy();

        // Load operator plug-in UIDs
        TInt uid = 0;
        for(TInt i = 1; i <= numOfPlugins; i++)
            {
            error = iAIRepository->Get( KAINumOfOperatorPlugins+i, uid );

            if( error == KErrNone )
                {
                TPluginInfo* pluginItem = new ( ELeave ) TPluginInfo;
                CleanupStack::PushL( pluginItem );

                // Define values
                ResetToDefault( *pluginItem );
                pluginItem->iPluginId.iUid = TUid::Uid(uid).iUid;

                iPlugins.AppendL( pluginItem );
                CleanupStack::Pop(); // pluginItem
                }
            }
        }

    iIndexDefaultFocusingPlugin = 0;
    }

// -----------------------------------------------------------------------------
// CAIPluginContainerControl::LoadPluginsL
// -----------------------------------------------------------------------------
//
void CAIPluginContainerControl::LoadPluginsL()
    {
    // Set send key functionality to default
    RProperty::Set(
            KPSUidAiInformation,
            KActiveIdleActOnSendKey,
            KActOnSendKey );
    for ( TInt index = 0; index < iPlugins.Count() ; index++)
        {
        LoadPluginL(index) ;
        }
    }

// -----------------------------------------------------------------------------
// CAIPluginContainerControl::LoadPluginL
// -----------------------------------------------------------------------------
//
void CAIPluginContainerControl::LoadPluginL(TInt aIndex)
    {
    CExtrStandbyScr* currentPlugin = NULL;

    TPluginInfo *pluginItem = iPlugins[aIndex];

    if (pluginItem->iState == EAIPluginOn)
        {
        TRAPD(err, currentPlugin = CExtrStandbyScr::NewL(pluginItem->iPluginId,this)) ;

        if ( err != KErrNone )
            {
            pluginItem->iState = EAIPluginError ;
            }
        else
            {

            if ( err == KErrNone )
                {
                TSize MinSize = currentPlugin->GetMinSize() ;
                TSize MaxSize = currentPlugin->GetMaxSize() ;
                pluginItem->iResizeable = MinSize != MaxSize ;

                currentPlugin->SetObserver(this) ;

                TRAP_IGNORE(currentPlugin->LoadEngineL()) ;
                }

            if ( err != KErrNone )
                {
                delete currentPlugin ;
                pluginItem->iState = EAIPluginError ;
                }
            else
                {
                //currentPlugin->SetDimmed(ETrue) ;
                pluginItem->iPlugin = currentPlugin ;
                pluginItem->iPlugin->ActivateL();
                }
            }
        }

    }

// -----------------------------------------------------------------------------
// CAIPluginContainerControl::DestroyPluginsL
// -----------------------------------------------------------------------------
//
void CAIPluginContainerControl::DestroyPlugins()
    {
    TInt index;

    for ( index = 0; index < iPlugins.Count(); index++ )
        {
        TPluginInfo* plugin = iPlugins[index];

        if (plugin->iPlugin)
            {
            delete plugin->iPlugin;
            plugin->iPlugin = NULL;
            }
        }
    iPlugins.ResetAndDestroy();
    iIndexPlugin = -1;
    iCurrentPlugin = NULL ;
    }

// ----------------------------------------------------------------------------
// CAIPluginContainerControl::ResetToDefault
// ----------------------------------------------------------------------------
//
void CAIPluginContainerControl::ResetToDefault( TPluginInfo& aPluginInfo )
    {
    aPluginInfo.iPlugin = NULL;
    aPluginInfo.iResizeable = EFalse;
    aPluginInfo.iOnScreen = EFalse;
    aPluginInfo.iPluginId.iUid = 0;
    aPluginInfo.iAlignment = EAITop;
    aPluginInfo.iWeight = 0;
    aPluginInfo.iCRKeyId = 0;
    aPluginInfo.iState = EAIPluginOn;
    }


// ----------------------------------------------------------------------------
// CAIPluginContainerControl::IsInFullScreenModeL
// ----------------------------------------------------------------------------
//
TBool CAIPluginContainerControl::IsInFullScreenModeL()
    {
    return iIsFullScreenMode;
    }

// ----------------------------------------------------------------------------
// CAIPluginContainerControl::UpdateBackgroundL
// ----------------------------------------------------------------------------
//
void CAIPluginContainerControl::UpdateBackgroundL()
    {
    delete iBgContext;
    iBgContext = NULL;

    CAknAppUi* appui = static_cast<CAknAppUi*>(iEikonEnv->AppUi());
    TRect rect = appui->ClientRect();
    TAknsItemID backSkinId;

    if(IsInFullScreenModeL() && !Layout_Meta_Data::IsLandscapeOrientation())
        {
        backSkinId = KAknsIIDQsnBgScreen;
        }
    else
        {
        backSkinId = KAknsIIDQsnBgAreaMainIdle;
        }

    iBgContext = CAknsLayeredBackgroundControlContext::NewL(
        backSkinId,
        rect,
        ETrue,
        EAIBgLayersN );

    iBgContext->SetLayerImage(
        EAIBgLayerWallpaper,
        KAknsIIDWallpaper );

    TSize wallpapersize;
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EWallpaperPane, wallpapersize );

    TRect wallpaperrect(wallpapersize);
    wallpaperrect.Move(0, rect.Size().iHeight - wallpapersize.iHeight);

    iBgContext->SetLayerRect(
        EAIBgLayerWallpaper,
        wallpaperrect );

    iBgContext->SetParentPos(PositionRelativeToScreen());
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::CAIPluginContainerControl
// ---------------------------------------------------------
//
CAIPluginContainerControl::CAIPluginContainerControl()
    {
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::ConstructL
// ---------------------------------------------------------
//
void CAIPluginContainerControl::ConstructL( CCoeControl* aContainer )
    {
    iContainer = aContainer;
    CreateWindowL();
    SetParent(iContainer);

    UpdateBackgroundL();

    iAIRepository = CRepository::NewL( KCRUidActiveIdle );
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iAIRepository);
    iNotifyHandler->StartListeningL();

        // backup/restore observer
    iBackupObserver = CPropertyObserver::NewL(*this, KUidSystemCategory, conn::KUidBackupRestoreKey);

    iIndexPlugin = -1;
    ActivateL();
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::ChangeFocusPluginL
// ---------------------------------------------------------
//
void CAIPluginContainerControl::ChangeFocusPluginL(TInt aDirection)
    {
    if (!iCurrentPlugin)
        {
        return ;
        }
    iCurrentPlugin->iPlugin->PrepareForFocusLossL();
    iCurrentPlugin->iPlugin->SetFocus(EFalse,EDrawNow) ;
    switch(aDirection)
        {
        case EAIFocusDown:
            iIndexPlugin = NextVisiblePlugin(iIndexPlugin+1) ;
            break ;
        case EAIFocusUp:
            iIndexPlugin = PreviousVisiblePlugin(iIndexPlugin-1) ;
            break ;
        default:
            User::Leave(KErrNotSupported) ;
        }
    if (iIndexPlugin == -1)
        {
        iCurrentPlugin = NULL ;
        }
    else
        {
        iCurrentPlugin = iPlugins[iIndexPlugin];
        iCurrentPlugin->iPlugin->PrepareForFocusGainL();
        iCurrentPlugin->iPlugin->SetFocus(ETrue,EDrawNow) ;
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::FocusOnDefaultFocusingPlugin
// ---------------------------------------------------------
//
void CAIPluginContainerControl::FocusOnDefaultFocusingPlugin(TDrawNow aDrawNow)
    {
    if (!CountComponentControls())
        {
        return ;
        }
    TInt index = NextVisiblePlugin(iIndexDefaultFocusingPlugin) ;
    if (index != iIndexPlugin)
        {
        if (iCurrentPlugin)
            {
            TRAP_IGNORE(iCurrentPlugin->iPlugin->PrepareForFocusLossL()); //TRAPPED
            iCurrentPlugin->iPlugin->SetFocus(EFalse,aDrawNow) ;
            }
        iIndexPlugin = index;
        }
    if (iIndexPlugin == -1)
        {
        iCurrentPlugin = NULL ;
        }
    else
        {
        /*if (iCurrentPlugin && iCurrentPlugin->iPlugin->IsFocused())
            {
            return;
            }
        */
        iCurrentPlugin = iPlugins[iIndexPlugin];
        TRAP_IGNORE(iCurrentPlugin->iPlugin->PrepareForFocusGainL()); //TRAPPED
        iCurrentPlugin->iPlugin->ResetToDefaultState();
        iCurrentPlugin->iPlugin->SetFocus(ETrue,aDrawNow) ;
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::CalculateRectPlugins
// ---------------------------------------------------------
//
void CAIPluginContainerControl::CalculateRectPlugins()
    {
    if (CountComponentControls() == 0)
        {
        DrawNow();
        return ;
        }

    TInt paneHeight = Rect().Height();

    TInt topPluginX=0;
    TInt bottomPluginX=paneHeight;

    for ( TInt index = 0; index < iPlugins.Count(); index++)
        {
        iPlugins[index]->iOnScreen = EFalse ;
        }

    HandleTopPlugins(topPluginX,bottomPluginX) ;
    HandleBottomPlugins(topPluginX,bottomPluginX) ;

    if (iIndexPlugin == -1)
        {
        FocusOnDefaultFocusingPlugin();
        }

    DrawNow();
    }


// ---------------------------------------------------------
// CAIPluginContainerControl::HandleTopplugins
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandleTopPlugins(TInt& aTopPluginX, TInt& aBottomPluginX)
    {

    for ( TInt index = 0; index < iPlugins.Count(); index++)
        {
        HandlePlugin(index,EAITop,aTopPluginX,aBottomPluginX);
        } // for all plugins

    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HandleBottomPlugins
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandleBottomPlugins(TInt& aTopPluginX, TInt& aBottomPluginX)
    {
    for ( TInt index = iPlugins.Count() -1 ; index >= 0 ; index--)
        {
        HandlePlugin(index,EAIBottom,aTopPluginX,aBottomPluginX);
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HandlePlugin
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HandlePlugin(TInt aIndex,
                                             TAIPluginAlignment aAlign,
                                             TInt& aTopPluginX,
                                             TInt& aBottomPluginX)
    {
    TRect rectPlugin ; //Rect of the current plugin

    TPluginInfo* pluginItem = iPlugins[aIndex];

    if ((pluginItem->iPlugin) && (pluginItem->iAlignment == aAlign))
        {
        TSize plugSize = pluginItem->iPlugin->GetPreferedSize();

        if ((plugSize.iHeight>0) && (plugSize.iWidth >0))
            {
            pluginItem->iOnScreen = ETrue;

            TInt overRunHeight = 0 ;

            if (pluginItem->iAlignment == EAITop)
                {
                if ((aTopPluginX+plugSize.iHeight) > aBottomPluginX)
                    {
                    overRunHeight = aTopPluginX+plugSize.iHeight-aBottomPluginX ;
                    }
                }
            else
                {

                if ((aBottomPluginX-plugSize.iHeight) < aTopPluginX)
                    {
                    overRunHeight = aTopPluginX-(aBottomPluginX-plugSize.iHeight) ;
                    }
                }
            if (overRunHeight)
                {

                while (overRunHeight > 0)
                    { // loop finding reduceable plugins from the first to the current
                    TInt reduceablePluginIndex = FindReduceablePlugin(overRunHeight,aIndex) ;
                    if (reduceablePluginIndex == -1)
                        {
                        // no plugin can be reduced.
                        // get the plugin with the lowerest Weight and hide it
                        TInt hideblePluginIndex = FindLowerWeightPlugin(aIndex) ;
                        if ((hideblePluginIndex == -1) || (hideblePluginIndex == aIndex))
                            {
                            HidePlugin(aIndex) ;
                            return ;
                            }
                        else
                            {
                            TInt hideblePluginHeight = iPlugins[hideblePluginIndex]->iPlugin->Rect().Size().iHeight ;
                            // Hide the found plugin and move the other before the current
                            ReducePlugin(hideblePluginIndex,aIndex,hideblePluginHeight) ;
                            if (iPlugins[hideblePluginIndex]->iAlignment == EAITop)
                                {
                                // recalculate position the the top current plugin
                                aTopPluginX -= hideblePluginHeight ;
                                }
                            else
                                {
                                // recalculate position the the bottom current plugin
                                aBottomPluginX += hideblePluginHeight;
                                }
                            overRunHeight -= hideblePluginHeight;
                            }
                        }
                    else
                        {
                        // get the height that the found reduaceble plugin can reduce itself
                        TInt reduceablePluginHeight = iPlugins[reduceablePluginIndex]->iPlugin->GetNextSizeDecrement().iHeight ;
                        if (reduceablePluginIndex == aIndex)
                            {
                            // if it has to be reduced the current plugin
                            // just recalculate the size of itself
                            plugSize -= TSize(0,reduceablePluginHeight) ;
                            if (plugSize.iHeight <= 0)
                                {
                                HidePlugin(aIndex) ;
                                return ;
                                }
                            else
                                {
                                if (pluginItem->iAlignment == EAITop)
                                    {
                                    rectPlugin.SetRect(TPoint(0,aTopPluginX),plugSize) ;
                                    }
                                else
                                    {
                                    rectPlugin.SetRect(TPoint(0,aBottomPluginX-plugSize.iHeight),plugSize) ;
                                    }

                                pluginItem->iPlugin->SetRect(rectPlugin);
                                }
                            }
                        else
                            {
                            // reduce the found plugin and move the other before the current
                            ReducePlugin(reduceablePluginIndex,aIndex,reduceablePluginHeight) ;
                            if (iPlugins[reduceablePluginIndex]->iAlignment == EAITop)
                                {
                                // recalculate position the the top current plugin
                                aTopPluginX -= reduceablePluginHeight ;
                                }
                            else
                                {
                                // recalculate position the the bottom current plugin
                                aBottomPluginX += reduceablePluginHeight;
                                }
                            }
                        overRunHeight -= reduceablePluginHeight;
                        }
                    }

                if (overRunHeight > 0)
                    {
                    //no plugin can be reduced go to the next one
                    return;
                    }
                } // while finding reduceable plugin(s)

            if (pluginItem->iAlignment == EAITop)
                {
                rectPlugin.SetRect(TPoint(0,aTopPluginX),plugSize) ;
                aTopPluginX += plugSize.iHeight;
                }
            else
                {
                rectPlugin.SetRect(TPoint(0,aBottomPluginX-plugSize.iHeight),plugSize) ;
                aBottomPluginX -= plugSize.iHeight;
                }
            pluginItem->iPlugin->SetRect(rectPlugin);
            //pluginItem->iPlugin->SetDimmed(EFalse);
            }
        else
            {
            // No on the screen
            HidePlugin(aIndex) ;
            }
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::NextVisiblePlugin
// ---------------------------------------------------------
//
TInt CAIPluginContainerControl::NextVisiblePlugin(TInt aStartIndex)
    {
    for ( TInt index = aStartIndex; index < iPlugins.Count(); index++)
        {
        TPluginInfo* plug = iPlugins[index];
        if (plug->iPlugin && (plug->iOnScreen) && (plug->iPlugin->IsNonFocusing() == EFalse))
            {
            return index ;
            }
        }
    return aStartIndex > 0 ? NextVisiblePlugin(0) : -1 ;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::PreviousVisiblePlugin
// ---------------------------------------------------------
//
TInt CAIPluginContainerControl::PreviousVisiblePlugin(TInt aStartIndex)
    {
    for ( TInt index = aStartIndex; index >= 0; index--)
        {
        TPluginInfo* plug = iPlugins[index];
        if (plug->iPlugin  && (plug->iOnScreen) && (plug->iPlugin->IsNonFocusing() == EFalse))
            {
            return index ;
            }
        }
    return (aStartIndex < (iPlugins.Count()-1)) ? PreviousVisiblePlugin(iPlugins.Count()-1) : -1 ;
    }


// ---------------------------------------------------------
// CAIPluginContainerControl::FindLowerWeightPlugin
// ---------------------------------------------------------
//
TInt CAIPluginContainerControl::FindLowerWeightPlugin(TInt aCurrentIndex)
    {
    TInt foundIndex = -1;
    TInt plugWeight = -1;
    TInt currentWeight = iPlugins[aCurrentIndex]->iWeight ;

    for ( TInt index = 0 ; index <= iPlugins.Count()-1; index++)
        {
        TPluginInfo* plug = iPlugins[index];
        if ((plug->iOnScreen) && (plug->iWeight<=currentWeight) &&
            ((foundIndex==-1) || (plug->iWeight<=plugWeight)))
            {
            foundIndex = index ;
            plugWeight = plug->iWeight;
            }
        }
    return foundIndex ;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::FindReduceablePlugin
// ---------------------------------------------------------
//
TInt CAIPluginContainerControl::FindReduceablePlugin(TInt aOverRunHeight, TInt aCurrentIndex)
    {
    TInt foundIndex = -1;
    TInt plugWeight = -1;

    for ( TInt index = 0 ; index <= iPlugins.Count()-1; index++)
        {
        TPluginInfo* plug = iPlugins[index];
        //When resize I don't check the Weight with the current plugin
        // before we try to reduce a plugin
        // if any plugin can be reduced then we have to hide a plugin with a weight less of the current
        if ((plug->iResizeable) && (plug->iOnScreen)) //&& (plug->iWeight <= currentWeight))
            {
            TInt reduceablePluginHeight = iPlugins[index]->iPlugin->GetNextSizeDecrement().iHeight ;
            if (reduceablePluginHeight > 0)
                {
                // the plugin can still be reduced
                if (index == aCurrentIndex)
                    {
                    if ((plugWeight == -1) || (plug->iWeight <= plugWeight))
                        {
                        foundIndex = index ;
                        plugWeight = plug->iWeight ;
                        }
                    }
                else
                    {
                    TSize plugMinSize = plug->iPlugin->GetMinSize() ;
                    TSize plugSize = plug->iPlugin->Rect().Size();
                    TInt plugOverRunHeight = plugSize.iHeight-plugMinSize.iHeight;
                    if ((plugOverRunHeight >=aOverRunHeight) &&
                        ((plugWeight == -1) || (plug->iWeight <= plugWeight))) // before I check the weight
                        {
                        foundIndex = index ;
                        plugWeight = plug->iWeight ;
                        }
                    }
                }
            }
        }
    return foundIndex ;
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::ReducePlugin
// ---------------------------------------------------------
//
void CAIPluginContainerControl::ReducePlugin(TInt aPluginToBeReducedIndex,
                                             TInt aCurrentIndex,
                                             TInt aReduceHeight)
    {
    TInt startIndex;
    TInt lastIndex;
    TInt lastTopPluginIndex ;
    TInt index;

    TAIPluginAlignment reduceAlign = iPlugins[aPluginToBeReducedIndex]->iAlignment ;

    //find the last top plugin in the list
    lastTopPluginIndex = -1;
    for ( index = 0 ;  index < iPlugins.Count() ; index++)
          {
          if (iPlugins[index]->iAlignment==EAITop)
             {
              lastTopPluginIndex = index;
              }
          }

    if (reduceAlign == EAITop)
        {
        __ASSERT_ALWAYS(aPluginToBeReducedIndex <= lastTopPluginIndex, User::Invariant());
        startIndex = aPluginToBeReducedIndex;
        // we don't have to handle the current if is in the top list
        // otherwise if the current index is in the bottom we handle all the top plugin
        lastIndex = (aCurrentIndex <= lastTopPluginIndex) ? (aCurrentIndex > aPluginToBeReducedIndex ? aCurrentIndex -1 : aCurrentIndex  ): lastTopPluginIndex;
        }
    else
        {
        // if lastTopPluginIndex==-1 means all the plugin have a botton alignment
        startIndex = lastTopPluginIndex+1;
        if (startIndex == aCurrentIndex)
            {//we don't have to handle the current plugin
            startIndex++;
            }
        __ASSERT_ALWAYS(aPluginToBeReducedIndex >= startIndex, User::Invariant());
        lastIndex = aPluginToBeReducedIndex;
        }

    for (  index = startIndex ; index <= lastIndex; index++)
        {
        TPluginInfo* plug = iPlugins[index];

        if (plug->iOnScreen && plug->iPlugin)
            {
            TRect rectPlugin = plug->iPlugin->Rect() ;
            if (index == aPluginToBeReducedIndex)
                {
                TSize newSize(0, aReduceHeight) ;
                rectPlugin.Resize(-newSize) ;
                if (rectPlugin.Height() == 0)
                    {
                    HidePlugin(index) ;
                    }
                else
                    {
                    if (reduceAlign == EAIBottom)
                        {
                        rectPlugin.Move(0,+aReduceHeight);
                        }
                    plug->iPlugin->SetRect(rectPlugin) ;
                    }
                }
            else
                {
                if (reduceAlign == EAITop)
                    {
                    rectPlugin.Move(0,-aReduceHeight);
                    plug->iPlugin->SetRect(rectPlugin);
                    }
                else
                    {
                    rectPlugin.Move(0,+aReduceHeight);
                    plug->iPlugin->SetRect(rectPlugin);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CAIPluginContainerControl::HidePlugin
// ---------------------------------------------------------
//
void CAIPluginContainerControl::HidePlugin(TInt aIndex)
    {
    TPluginInfo* plug = iPlugins[aIndex];

    //plug->iPlugin->SetDimmed(ETrue) ;
    plug->iPlugin->SetRect(TRect(0,0,0,0)) ;
    plug->iOnScreen = EFalse ;

    if (iIndexPlugin == aIndex)
        { // the plugin that is going to dissappear have the focus
        TRAP_IGNORE(ChangeFocusPluginL(EAIFocusDown)); //TRAPPED
        }
    }

//  End of File
