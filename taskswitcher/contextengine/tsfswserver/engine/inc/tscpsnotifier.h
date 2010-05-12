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
* Description:  Task monitor engine
 *
*/

#ifndef TSCPSNOTIFIER_H
#define TSCPSNOTIFIER_H

#include <e32base.h>
#include <liwcommon.h>

class CLiwGenericParamList;
class CLiwServiceHandler;
class MLiwInterface;
class CLiwDefaultMap;
class CLiwMap;
class CTsFswEngine;

NONSHARABLE_CLASS(CTSCpsNotifier) : 
    public CBase, 
    private MLiwNotifyCallback
    {
public:
    /**
     * Two-phased constructor.
     */
    static CTSCpsNotifier* NewL( CTsFswEngine& aEngine );

    /**
     * Destructor.
     */
    virtual ~CTSCpsNotifier();

private:
    /**
     * Constructor.
     */
    CTSCpsNotifier( CTsFswEngine& aEngine );

    /**
     * 
     */
    void ConstructL();
private:
    // Form base class MLiwNotifyCallback.
    /**
     * Callback from LIW framework to process an event that was expected.
     * @see MLiwNotifyCallback::HandleNotifyL
     */
    TInt HandleNotifyL( TInt aCmdId, 
            TInt aEventId,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& aInParamList );

private:
    /**
     * Create and return map filled with widgets identifiers
     */
    CLiwDefaultMap* WidgetFilterLC();
    
    /**
     * Initalise CPS service interface
     */
    void InitCpsInterfaceL();

    /**
     * Request for CPS notifications abou widgets
     */
    void ExecuteRegistrationCommandL( TUint aOption );

    /**
     * Obtain widget information and
     * forward widgets data change handling to iEngine
     */
    void HandleWidgetUpdateL( const CLiwMap& aChangeMap );

    /**
     * Obtain from CPS information about widget
     */
    void GetWidgetDataL( const TDesC& aContentId,
            TInt& aWidgetId,
            TInt& aBitmapHandle );

    /**
     * Obtain from CPS information about widget from data map
     */
    void GetWidgetDataFromDataMapL( const CLiwMap& aDataMap,
            TInt& aWidgetId,
            TInt& aBitmapHandle );

private:

    MLiwInterface* iCpsInterface;//own

    CLiwServiceHandler* iServiceHandler;//own

    CTsFswEngine& iEngine;//not own
    };

#endif //TSCPSNOTIFIER_H
