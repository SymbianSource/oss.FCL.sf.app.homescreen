/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dialog renderer.
*
*/


#ifndef C_AIDIALOGRENDERER_H
#define C_AIDIALOGRENDERER_H

// System includes
#include <e32base.h>
#include "ainativerenderer.h"
#include <MPsetNetworkInfoObs.h> // for MPsetNetworkInfoObserver 
#include <MPsetNetworkSelect.h>  // for MPsetNetworkSelect

// User includes
#include "ainativerenderer.h"

// Forward declarations
class CAknNoteDialog;
class CHsContentPublisher;
class CPsetNetwork;
class CAknWaitDialog;
class CPsetContainer;
class CAknPopupList;

namespace AiNativeUiController
{

/**
 *  Dialog renderer.
 *
 *  Used to show CAknNoteDialog on screen.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiDialogRenderer ) : public CAiNativeRenderer, public MPsetNetworkInfoObserver
    {
public:

    static CAiDialogRenderer* NewLC();

    virtual ~CAiDialogRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    void DoCleanL( CHsContentPublisher& aPlugin, TInt aContent );

    void FocusObtainedL();
    
    // from base class MPsetNetworkInfoObserver
    
        /**
         * @@see MPsetNetworkInfoObserver::HandleNetworkInfoReceivedL
         */
        void HandleNetworkInfoReceivedL( 
                        const CNetworkInfoArray* aInfoArray, const TInt aResult );
            
        /**
         * @@see MPsetNetworkInfoObserver::HandleSearchingNetworksL
         */
        void HandleSearchingNetworksL( TServiceRequest aRequest );
            
        /**
         * @@see MPsetNetworkInfoObserver::HandleRequestingSelectedNetworkL
         */
        void HandleRequestingSelectedNetworkL( TBool aOngoing );
    
        /**
         * @@see MPsetNetworkInfoObserver::HandleNetworkChangedL - not used
         */
        void HandleNetworkChangedL(
                        const MPsetNetworkSelect::TNetworkInfo& aCurrentInfo,
                        const RMobilePhone::TMobilePhoneRegistrationStatus& aStatus,
                        const TInt aResult );
     
        /**
         * @@see MPsetNetworkInfoObserver::HandleNetworkErrorL
         */
        void HandleNetworkErrorL( const TServiceRequest aRequest,
                         const TInt aError );
    
        /**
         * @@see MPsetNetworkInfoObserver::HandleCurrentNetworkInfoL - not used
         */
        void HandleCurrentNetworkInfoL( 
                const MPsetNetworkSelect::TCurrentNetworkInfo& /* aCurrentInfo */, 
                const TInt /* aResult */ ) {};
            
        /**
         * @@see MPsetNetworkInfoObserver::HandleNetworkChangedL - not used
         */
        void HandleNetworkChangedL( 
                const MPsetNetworkSelect::TNetworkInfo& /* aCurrentInfo */, 
                const MPsetNetworkSelect::TCurrentNetworkStatus /* aStatus */, 
                const TInt /* aResult */ ) {}; 
    
         /**
         * @@see MPsetNetworkInfoObserver::HandleCallActivatedL
         */
        void HandleCallActivatedL() {};
    

private:

    CAiDialogRenderer();

    /**
     * Show dialog.
     *
     * @since S60 3.2
     */
    void ShowDialogL();

    void ConstructL();

    void PopulatePLMNListL();

    /**
     * Provides the network operator list.
     */
    void NetworkListL();

    /**
     * Sets title to a popup list.
     * @param aList pointer to list
     * @param aTitleID resource ID of the title
     */
    void SetTitleToPopupL( CAknPopupList& aList, TInt aTitleID );

    /** 
     * Creates a waiting note (member variable).
     * @param aDelayOff is the note shown immediately or not
     */
    void CheckAndCreateDlgL( TBool aDelayOff );

    /*
     * Empties fetched network provider's list.
     */
    void PurgeNetworkList();

    /**
     * Shows a requested note.
     * @param aResourceId reource ID for note text
     * @param aType type of note
     */
    void ShowNoteL( TInt aResourceId, TInt aType );
 

private: // data

    /**
     * Dialog.
     * Own.
     */
    CAknNoteDialog* iDialog;
   
    /**
     * Text to be rendered in the dialog.
     * Own.
     */
    HBufC* iText;

    /// Indicates SIM registration failure.
    TBool iShowDialog;

#ifdef __COVER_DISPLAY
    /**
     * Dialog ID to be published.
     * Own.
     */
    TInt iDialogId;
#endif


    //PhoneSettings engine object
    CPsetNetwork*           iNetwork;
    //PhoneSettings container
    CPsetContainer*         iSettingsContainer;
    //Requesting note
    CAknWaitDialog*         iDlg;        
    //Array of networks
    CNetworkInfoArray*      iNetworkArray;
    //Network selection list
    CAknPopupList*          iNetworkPopupList;
    //Has the user selected a valid network
    TBool                   iApprovedNetwork;
    TBool                   iSearchForNetworksActive;
    TBool                   iPLMNListPopulateShowIsHandling;
   
    // Resource index
    TInt                    iRscIndex;

    };

} // namespace AiNativeUiController

#endif // C_AIDIALOGRENDERER_H
