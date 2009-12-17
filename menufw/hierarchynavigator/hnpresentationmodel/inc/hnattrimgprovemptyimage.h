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
* Description:   image provider for empty image
*
*/



#ifndef C_HNATTRIMAGEPROVEMPTY_H
#define C_HNATTRIMAGEPROVEMPTY_H

#include "hnattrimgprovider.h"


/**
 *  Image provider class for empty image.
 *  Image provider specialization that loads empty icon.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvEmptyImage ) : public CHnAttrImgProvider
    {
public:
    
    /**
     * Two-phase constructor.
     * 
     * @since S60 5.0
     * @return Fully constructed object
     */
    static CHnAttrImgProvEmptyImage* NewL();

    /**
     * Two-phase constructor.
     * 
     * @since S60 5.0
     * @return Fully constructed object
     */
    static CHnAttrImgProvEmptyImage* NewLC();

    /**
     * Standard destructor.
     * 
     * @since S60 5.0
     */
    virtual ~CHnAttrImgProvEmptyImage();

// from base class CHnAttrImgProvider
public:
    
    /**
     * Value property getter.
     *
     * @since S60 5.0
     * @return value
     */
    const TDesC8& Value( );

private:

    /**
     * Defualt constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvEmptyImage();

    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL( );

private: // data

    /**
     * Empty value.
     */
    TBuf8<1> iEmptyVal;
    };


#endif // C_HNATTRIMGPROVEMPTY_H

