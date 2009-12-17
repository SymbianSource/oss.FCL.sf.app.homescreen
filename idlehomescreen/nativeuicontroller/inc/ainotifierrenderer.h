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
* Description:  Soft notifier renderer.
*
*/


#ifndef C_AISOFTNOTIFERRENDERER_H
#define C_AISOFTNOTIFERRENDERER_H


#include <e32base.h>
#include "ainativerenderer.h"
#include <AknSoftNotifier.h>

class CAknSoftNotifier;

namespace AiNativeUiController
{

/**
 *  Dialog renderer.
 *
 *  Used to show CAknSoftNotifier on screen.
 *
 *  @since S60 3.2
 */
class CAiNotifierRenderer : public CAiNativeRenderer
    {
public:

    static CAiNotifierRenderer* NewLC();

    virtual ~CAiNotifierRenderer();

// from base class CAiNativeRenderer

    void DoPublishL( MAiPropertyExtension& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    void DoCleanL( MAiPropertyExtension& aPlugin, TInt aContent );

private:

    /**
     * Add a notification
     */
    void AddNotification( TAknSoftNotificationType aType );

    /**
     * Remove a notification
     */
    void RemoveNotification( TAknSoftNotificationType aType );

    void ConstructL();

    static TInt NWLostDelayCallBack(TAny* aParam);

    CAiNotifierRenderer();

private: // data

    /**
     * Notifier service
     * Own
     */
    CAknSoftNotifier* iSoftNotifier;

    /**
     * Timer to delay the showing of the notification if needed
     * Own
     */
    CPeriodic *iTimer;
    };

} // namespace AiNativeUiController

#endif // C_AISOFTNOTIFERRENDERER_H
