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
* Description:  image provider for empty image
*
*/


#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include "hnattrimgprovemptyimage.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnAttrImgProvEmptyImage::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvEmptyImage* CHnAttrImgProvEmptyImage::NewL( )
    {
    CHnAttrImgProvEmptyImage* self = NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvEmptyImage* CHnAttrImgProvEmptyImage::NewLC( )
    {
    CHnAttrImgProvEmptyImage* self = new( ELeave ) CHnAttrImgProvEmptyImage;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvEmptyImage::CHnAttrImgProvEmptyImage()
{
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnAttrImgProvEmptyImage::~CHnAttrImgProvEmptyImage()
{
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnAttrImgProvEmptyImage::Value( )
    {
    return iEmptyVal;
    }

