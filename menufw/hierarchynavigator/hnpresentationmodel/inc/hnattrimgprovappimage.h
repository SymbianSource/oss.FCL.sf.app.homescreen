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
* Description:   image provider for application uid
*
*/


#ifndef HNATTRIMGPROVAPPIMAGE_H_
#define HNATTRIMGPROVAPPIMAGE_H_

#include <AknUtils.h>
#include "hnattrimgprovider.h"
#include "hnglobals.h" 

/**
 *  Image provider class for application uid.
 *
 *  Image provider specialization that loads the image 
 *  based on the application uid.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvAppImage ) : public CHnAttrImgProvider
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aApplicationUid Application uid.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvAppImage* NewL( TUid aApplicationUid );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aApplicationUid Application uid.
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttrImgProvAppImage* NewLC( TUid aApplicationUid );

    /**
     * Virtual destructor.
     * 
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvAppImage();

// from base class CHnAttrImgProvider
public:
    
    /**
     * Returns value.
     *
     * @since S60 5.0
     * @return Value.
     */
    const TDesC8& Value( );

    /**
     * This method should create a bitmap and its mask, 
     * by retrieving it form the application by a given uid (passed
     * in the constructor)
     *
     * @param aId          Parameter not used.
     * @param aBitmap      Output pointer to a bitmap.     
     * @param aMaskBitmap  Output pointer to a mask.
     * @since S60 5.0
     */
     void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, 
             CFbsBitmap*& aMaskBitmap);

private:

    /**
     * Default constructor.
     */
    CHnAttrImgProvAppImage();

    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     * @param aApplicationUid Application uid.
     */
    void ConstructL( TUid aApplicationUid );

private: // data
    
    /**
     * Value
     */
    TBuf8<KMaxLength> iValue;

    /**
     * Application UID
     */
    TUid iApplicationUid;
    };
    
#endif // HNATTRIMGPROVAPPIMAGE_H_
    
