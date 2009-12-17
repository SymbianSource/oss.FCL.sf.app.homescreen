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
* Description:  Navipane renderer.
*
*/


#include <aipropertyextension.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>

#include "ainavipanerenderer.h"
#include "ainavipaneanimator.h"
#include "aistatuspanel.h"
#include "ainativeuimodel.h"
#include "ainativeuiplugins.h"
#include "contentprioritymap.h"
#include <aicontentrequest.h>
#include "debug.h"

using namespace AiNativeUiController;


void CAiNaviPaneRenderer::ConstructL()
    {
    iAnimator = CAiNaviPaneAnimator::NewL( iStatusPanel, 
                                           iRenderingPriorities, 
                                           *this );
    iProfileText = HBufC::NewL(0);
    iSilentIndicator = HBufC::NewL(0);
    }


CAiNaviPaneRenderer* CAiNaviPaneRenderer::NewLC( 
        CAiStatusPanel& aStatusPanel )
    {
    CAiNaviPaneRenderer* self = new( ELeave ) CAiNaviPaneRenderer( aStatusPanel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CAiNaviPaneRenderer::~CAiNaviPaneRenderer()
    { 
    delete iAnimator;
    delete iProfileText;
    delete iSilentIndicator; 
    delete iTimedProfileIndicator;
    }


CAiNaviPaneRenderer::CAiNaviPaneRenderer( CAiStatusPanel& aStatusPanel )
    : iStatusPanel( aStatusPanel ), 
    iRenderingPriorities( AiNativeUiModel::RenderingPriorities( AiNativeUiModel::KNaviPaneId ) ),
    iCurrentContent(KAiNullContentId),
    iCurrentPriority(AiNativeUiModel::KNoPriority)
    {
    }


void CAiNaviPaneRenderer::DoPublishL( MAiPropertyExtension& aPlugin,
                                      TInt aContent, 
                                      const TDesC16& aText,
                                      TInt /*aIndex*/ )
    {
    if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
           {
        switch( aContent )
            {
            // FALLTHROUGH
            case EAiDeviceStatusContentDate:
            case EAiDeviceStatusContentProfileName:
            case EAiDeviceStatusContentGeneralProfileName:
                {
                __PRINT(__DBG_FORMAT("XAI: Render navi pane - %d, %S"), aContent, &aText );
                
                const TInt priority = iRenderingPriorities.RenderingPriority( aContent );
                
                // Get active profile ID
                CRepository* profileApi;
                TInt         profileId;
                profileApi = CRepository::NewL( KCRUidProfileEngine );
                TInt err = profileApi->Get( KProEngActiveProfile, profileId );
                delete profileApi;

                if ( aContent == EAiDeviceStatusContentGeneralProfileName )
                    {
                    const TInt KGeneralProfileId( 0 );
                    if (profileId == KGeneralProfileId) 
                        {
                        iAnimator->AddItemL( EAiDeviceStatusContentGeneralIndicator, aText );
                        }
//                    iAnimator->AddItemL( EAiDeviceStatusContentGeneralIndicator, aText );
//                    iAnimator->AddItemL( EAiDeviceStatusContentGeneralProfileName, aText );

                    iAnimator->AnimateL();
                    }

          
                if( (priority >= iCurrentPriority) || !iIsSilent || !iIsTimed )
                    {
                    if( priority >= iCurrentPriority )
                        {
                        delete iProfileText;
                        iProfileText = NULL;
                        iProfileText = aText.AllocL();
                        iIsOldTextChanged = ETrue;

                        iCurrentPriority = priority;
                        iCurrentContent = aContent;
                        }

                    HBufC* newText = ConstructProfileTextLC();

                    if( !iAnimator->IsActive() )
                        {
                        iStatusPanel.SetNaviPaneTextL( *newText );
                        iStatusPanel.RenderNaviPaneL();
                        }
                    
                    CleanupStack::PopAndDestroy( newText );
                    }            
                
                __PRINTS( "XAI: Render navi pane - priority check failed");
                break;
                }

            case EAiDeviceStatusContentSilentIndicator:
                {
                __PRINT(__DBG_FORMAT("XAI: Render navi pane - %d, %S"), aContent, &aText );

                const TInt priority = iRenderingPriorities.RenderingPriority( aContent );
                
                if( priority >= iCurrentPriority && iIsOldTextChanged )
                    {
                    delete iSilentIndicator;
                    iSilentIndicator = NULL;
                    iSilentIndicator = aText.AllocL();
                    iIsSilent = ETrue;
                    if( !iAnimator->IsActive() )
                        {    
                        HBufC* newText = ConstructProfileTextLC();
                        iStatusPanel.SetNaviPaneTextL( *newText );
                        iStatusPanel.RenderNaviPaneL();                        
                        CleanupStack::PopAndDestroy( newText );
                        }
     
                    

                    
                    // iCurrentContent is not assigned because silent indicator is 
                    // rendered within the navi pane text
                    
                    __PRINTS( "XAI: Render navi pane - done");
                    }                
                __PRINTS( "XAI: Render navi pane - priority check failed");
                break;
                }
            case EAiDeviceStatusContentTimedProfileIndicator:
                {
                __PRINT(__DBG_FORMAT("XAI: Render navi pane - %d, %S"), aContent, &aText );

                const TInt priority = iRenderingPriorities.RenderingPriority( aContent );
                
                if( priority >= iCurrentPriority && iIsOldTextChanged )
                    {
                    delete iTimedProfileIndicator;
                    iTimedProfileIndicator = NULL;
                    iTimedProfileIndicator = aText.AllocL();
                    iIsTimed = ETrue;
                    
                    if( !iAnimator->IsActive() )
                        {    
                        HBufC* newText = ConstructProfileTextLC();
                        iStatusPanel.SetNaviPaneTextL( *newText );
                        iStatusPanel.RenderNaviPaneL();                        
                        CleanupStack::PopAndDestroy( newText );
                        }
     

                    // iCurrentContent is not assigned because silent indicator is 
                    // rendered within the navi pane text
                    
                    __PRINTS( "XAI: Render navi pane - done");
                    }                
                __PRINTS( "XAI: Render navi pane - priority check failed");
                break;
                }                

            // FALLTHROUGH
            case EAiDeviceStatusContentMCNIndicator:
            case EAiDeviceStatusContentVHZIndicator:
            case EAiDeviceStatusContentCUGIndicator:
                {
                iAnimator->AddItemL( aContent, aText );
                if( !iAnimator->IsActive() )
                    {
                    iAnimator->AnimateL();
                    }
                iIsChanged = ETrue;
                break;
                }

            default:
                {
                User::Leave( KErrNotFound );
                break;
                }
            };
           }       
       else
           {
           User::Leave( KErrNotFound );    
           }
    }

HBufC* CAiNaviPaneRenderer::ConstructProfileTextLC()
    {
    TInt bufLen = iProfileText->Length();
    if ( iIsSilent )
        {
        bufLen += iSilentIndicator->Length();
        }
    if ( iIsTimed )
        {
        bufLen += iTimedProfileIndicator->Length();
        }
        
    HBufC *temp = HBufC::NewLC( bufLen );
     
    if ( iIsSilent )
        {
        temp->Des().Append( *iSilentIndicator );
        }
    if ( iIsTimed )
        {
        temp->Des().Append( *iTimedProfileIndicator );
        }
        
    temp->Des().Append( *iProfileText );
    return temp;
    }

void CAiNaviPaneRenderer::DoCleanL( MAiPropertyExtension& aPlugin, TInt aContent )
    {
    if( aPlugin.PublisherInfoL()->iUid == KDeviceStatusPluginUid )
        {    

        switch( aContent )
            {
            case EAiDeviceStatusContentNetworkIdentity:
                {
                iAnimator->RemoveItem( EAiDeviceStatusContentGeneralIndicator );
                break;
                }

            // FALLTHROUGH
            case EAiDeviceStatusContentDate:
            case EAiDeviceStatusContentProfileName:
            case EAiDeviceStatusContentGeneralProfileName:
            case EAiDeviceStatusContentSilentIndicator:
            case EAiDeviceStatusContentTimedProfileIndicator:
                {
                __PRINT(__DBG_FORMAT("XAI: Cleaning navi pane - %d"), aContent );
                
                if ( aContent == EAiDeviceStatusContentSilentIndicator )
                    {
                    iIsSilent = EFalse;
                    }
                if ( aContent == EAiDeviceStatusContentTimedProfileIndicator )
                    {
                    iIsTimed = EFalse;
                    }
                if ( aContent == EAiDeviceStatusContentGeneralProfileName )
                    {
                    iAnimator->RemoveItem( EAiDeviceStatusContentGeneralIndicator );
//                    iAnimator->RemoveItem( EAiDeviceStatusContentGeneralProfileName );
                    }
                if ( aContent == EAiDeviceStatusContentProfileName )
                    {
                    iAnimator->RemoveItem( EAiDeviceStatusContentProfileName );
                    }
           
                TInt currentPriority = iCurrentPriority;

                if( iCurrentContent == aContent )
                    {
                    iCurrentContent = KAiNullContentId;
                    iCurrentPriority = AiNativeUiModel::KNoPriority;
                    
                    // Publish empty text
                    iStatusPanel.SetNaviPaneTextL( KNullDesC );
                    iStatusPanel.RenderNaviPaneL();

                    MAiContentRequest* contentReq = static_cast<MAiContentRequest*>(
                        aPlugin.GetPropertyL( EAiContentRequest ) );
                        
                    if ( contentReq )
                        {
                        TInt nextContentId;
                        while ( iRenderingPriorities.GetNextContentId( 
                                                        nextContentId, 
                                                        currentPriority ) )
                            {
                            const TBool refreshed = contentReq->RefreshContent( nextContentId );
                            if ( refreshed )
                                {
                                break;
                                }
                            }
                        }
                                    
                    }                    
                __PRINTS( "XAI: Cleaning navi pane - done");    
                break;
                }

            case EAiDeviceStatusContentMCNIndicator:
            case EAiDeviceStatusContentVHZIndicator:
            case EAiDeviceStatusContentCUGIndicator:
                {
                iAnimator->RemoveItem( aContent );
                break;
                }

            default:
                {
                User::Leave( KErrNotFound );
                break;
                }
            };
        }
    else
        {
        User::Leave( KErrNotFound );    
        }    
    
    }


void CAiNaviPaneRenderer::FocusObtainedL()
    {
    if( !iStatusPanel.IsKeyLockEnabled() )
        {
        iAnimator->AnimateL();
        }
    iFocusObtained = ETrue;    
    }


void CAiNaviPaneRenderer::FocusLostL()
    {
    iAnimator->CancelAnimationL();
    iFocusObtained = EFalse;
    iCommitted = EFalse;
    }


void CAiNaviPaneRenderer::KeylockDisabledL()
    {
    iAnimator->AnimateL();
    }
 
    
void CAiNaviPaneRenderer::KeylockEnabledL()
    {
    iAnimator->CancelAnimationL();
    }
    

void CAiNaviPaneRenderer::TransactionCommittedL()
    {
    if( iIsChanged )
        {
        if( iFocusObtained && !iCommitted && !iStatusPanel.IsKeyLockEnabled() )
            {
            iAnimator->AnimateL();
            iCommitted = ETrue;
            }
        iIsChanged = EFalse;
        }
    }
    
    
void CAiNaviPaneRenderer::AnimationCompleteL()
    {
    // Navi pane animation is now completed -> show original text
    HBufC* newText = NULL;

    if ( iProfileText->Length() > 0 )
        {
        newText = ConstructProfileTextLC();
        }
    else
        {
        return;
        }

    iStatusPanel.SetNaviPaneTextL( *newText );
    iStatusPanel.RenderNaviPaneL();
    
    CleanupStack::PopAndDestroy( newText );
    }

