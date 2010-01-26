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
* Description:  Task list entry
 *
*/


#ifndef TSFSWENTRY_H
#define TSFSWENTRY_H

#include <e32base.h>
#include <e32cmn.h>

class CTsFswEntry;
class RReadStream;
class RWriteStream;

typedef RPointerArray<CTsFswEntry> RTsFswArray;

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsFswEntry ) : public CBase
    {
public:
    IMPORT_C static CTsFswEntry* NewL();
    IMPORT_C static CTsFswEntry* NewLC();
    ~CTsFswEntry();

    /**
     * Window group id.
     * In case of wrt widgets this is negative and is not a valid window group id.
     * Use this when calling CloseApp or SwitchToApp. (these work with widgets too)
     */
    IMPORT_C TInt WgId() const;
    
    /**
     * Application uid.
     */
    IMPORT_C TUid AppUid() const;
    
    /**
     * Application name.
     */
    IMPORT_C const TDesC& AppName() const;
    
    /**
     * Bitmap handle to the screenshot or 0 if not available.
     */
    IMPORT_C TInt ScreenshotHandle() const;
    
    /**
     * ETrue if the application is always shown in the fast swapper.
     * (no use to call CloseApp for such an application)
     */
    IMPORT_C TBool AlwaysShown() const;

    /**
     * ETrue if the application is a system application.
     * (no use to call CloseApp for such an application)
     */
    IMPORT_C TBool SystemApp() const;

    /**
     * ETrue if this is a wrt widget.
     */
    IMPORT_C TBool Widget() const;
    
    /**
     * Application icon bitmap handle or 0.
     * If may not be set if the screenshot is available, i.e. it is
     * only guaranteed to be non-zero when ScreenshotHandle() returns 0.
     */
    IMPORT_C TInt AppIconBitmapHandle() const;
    
    /**
     * Application icon mask handle.
     * @see AppIconBitmapHandle
     * The mask handle may be 0 even when the bitmap handle is non-zero.
     */
    IMPORT_C TInt AppIconMaskHandle() const;


    IMPORT_C void SetWgId( TInt aWgId );
    IMPORT_C void SetAppUid( const TUid& aUid );
    IMPORT_C void SetAppNameL( const TDesC& aAppName );
    IMPORT_C void SetScreenshotHandle( TInt aHandle );
    IMPORT_C void SetAlwaysShown( TBool aValue );
    IMPORT_C void SetSystemApp( TBool aValue );
    IMPORT_C void SetWidget( TBool aValue );
    IMPORT_C void SetAppIconHandles( TInt aBitmapHandle, TInt aMaskHandle );

    IMPORT_C void ExternalizeL( RWriteStream& aStream );
    IMPORT_C void InternalizeL( RReadStream& aStream );

    IMPORT_C static void ExternalizeArrayL( RWriteStream& aStream,
        const RTsFswArray& aArray );
    IMPORT_C static void InternalizeArrayL( RReadStream& aStream,
        RTsFswArray& aArray );

private:
    TInt iWgId;
    TUid iAppUid;
    HBufC* iAppName;
    TInt iScreenshotHandle;
    TBool iAlwaysShown;
    TBool iSystemApp;
    TBool iWidget;
    TInt iAppIconBitmapHandle;
    TInt iAppIconMaskHandle;
    
    TInt iReserved0;
    TInt iReserved1;
    TInt iReserved2;
    TInt iReserved3;
    };

#endif
