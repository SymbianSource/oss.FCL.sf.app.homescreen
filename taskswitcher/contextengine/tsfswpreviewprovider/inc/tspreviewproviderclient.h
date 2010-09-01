/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*/

#ifndef TSFASTSWAPPREVIEWPROVIDERCLIENT_H
#define TSFASTSWAPPREVIEWPROVIDERCLIENT_H

#include <e32base.h>
#include <w32std.h>
#include "tspreviewobserver.h"

class CTsPropertyListener;

/**
 * CTsFastSwapPreviewProviderClient
 */
NONSHARABLE_CLASS( CTsFastSwapPreviewProviderClient ) : public CWsGraphic
    {
public:
    CTsFastSwapPreviewProviderClient( MTsFastSwapPreviewObserver& aObs );
    void ConstructL();
    void Register( TInt aWgId );
    void Unregister( TInt aWgId );
    void ChangeScreen( TInt aScreenNo );
    void SetPreviewParam( const TSize& aSize, TDisplayMode aMode );
    void TakePreview();
    void AckPreview( TInt aPreviewId );
    virtual ~CTsFastSwapPreviewProviderClient();
private:
    void HandleMessage( const TDesC8& aData );
    void OnReplace();
private:
    void DoHandleMessageL( const TDesC8& aData );
    void SetProperties( TInt aHandle, TInt aWgId, TInt aOp );
private:
    MTsFastSwapPreviewObserver& iObs;
    CTsPropertyListener* iListener;
    };

#endif // TSPREVIEWPROVIDERCLIENT_H

// End of file
