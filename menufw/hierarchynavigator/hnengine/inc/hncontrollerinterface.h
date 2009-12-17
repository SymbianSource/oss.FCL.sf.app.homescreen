/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef HNCONTROLLERINTERFACE_H
#define HNCONTROLLERINTERFACE_H

#include "hnsuiteobserver.h"

class CLiwGenericParamList; 

/**
 * @ingroup group_hnengine
 * User interface refresh type 
 */
enum THnUiRefreshType
    {
        ERefreshSuite,
        ERefreshToolbar,
        EStartEditMode,
        EStopEditMode,
        EZoomLarge,
        EZoomSmall,
        EZoomNormal,
        EForegroundGain,
        EBackgroundGain,
        ELightOn,
        ELightOff,
        ERemoveLiwObjects
    };


/**
 * @ingroup group_hnengine
 * UI refresh observer. The implementing class can be notified about the ui being refreshed.
 *
 * @lib hnengine
 * @since S60 v3.2
 */
class MHnControllerInterface : public MHnSuiteObserver
    {
public:

    /**
     * Called when the ui needs to be refreshed,
     * for a reason indicated in aRefreshType.
     *
     * @since S60 v3.2
     * @param aRefreshType Defines reason for refresh.
     */
    virtual void NotifyUiRefreshL( const THnUiRefreshType aRefreshType ) = 0;
    
    /**
     * Trigger appropriate action on suite model.
     * 
     * @param aCustomSuiteEvent Event that is associated
     *                          with the lifecycle of a suite
     * @param aModel Suite model on that custom event should be performed.
     */
    virtual void HandleSuiteEventL ( THnCustomSuiteEvent aCustomSuiteEvent, 
            CHnSuiteModel *aModel ) = 0;
    
    /**
     * Whenever an action for extension manager occurs it invokes
     * this method to execute one, passing the information needed
     * by extension manager about the action as a parameters.
     * 
     * @param aUid UID of the ECom plugin's.
     * @param aCommand Command which has to be executed on the plugin.
     *                 Request to the Service.
     * @param aEventParamList List of the event parameters needed to
     *                        execute action. Extention Manager is its owner.
     * @return Number representing error code.
     */
    virtual TInt ExecuteExtensionActionL( const TUid aUid , const TDesC& aCommand,
            CLiwGenericParamList* aEventParamList ) = 0;
    };

#endif //HNCONTROLLERINTERFACE_H
