/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*/


#ifndef TSPROPERTYLISTENER_H
#define TSPROPERTYLISTENER_H

#include <e32base.h>
#include <e32property.h>

#include "tspreviewobserver.h"

// Publish & subscribe UID (akncapserver.exe SID so that it can define the keys)
const TUid KPSUidPreviewProvider = { 0x10207218 };

// P&S keys
const TUint32 KPSKeyWgId = 0x20016BE1;
const TUint32 KPSKeyFbsBitmapId = 0x20016BE2;
const TUint32 KPSKeyOperation = 0x20016BE3;
/**
 * Operation which will be done after P&S keys have changed.
 */
enum TPreviewOperation
    {
    EOperationNone,
    EOperationUnregister,
    EOperationBitmapUpdated,
    EOperationBitmapRotation90,
    EOperationBitmapRotation270
    };

/**
 * CPropertyListener
 */
NONSHARABLE_CLASS( CTsPropertyListener ) : public CActive
    {
public:
    static CTsPropertyListener* NewL( MTsFastSwapPreviewObserver& aObs );
    virtual ~CTsPropertyListener();
    
protected:
    CTsPropertyListener( MTsFastSwapPreviewObserver& aObs );
    void ConstructL();  

private: // From CActive
    void RunL();
    void DoCancel();
    
private:    
    void ReadPropertiesL();
    void DefinePropertyL( TInt aPSKey ); 
    void Subscribe();

private:
    MTsFastSwapPreviewObserver& iObs;
    RProperty iProperty;
   
    TInt iWgId;
    TInt iFbsBitmapId;
    
    TPreviewOperation iOperation;
    };

#endif // TSPROPERTYLISTENER_H

// End of file
