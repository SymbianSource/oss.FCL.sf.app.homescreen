/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <gulicon.h>
#include <liwbufferextension.h>
#include "hnextbmpiconholder.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnExtBmpIconHolder::SetGulIcon( CGulIcon* aGulIcon,
        CLiwBitmapBuffer* aBmpBuffer, CLiwBitmapBuffer* aMaskBuffer )
    {
    __ASSERT_DEBUG( aGulIcon && aBmpBuffer, User::Invariant() );

    if ( aGulIcon )
        {
        __ASSERT_DEBUG( aGulIcon->BitmapsOwnedExternally(), User::Invariant() );
        CHnIconHolder::SetGulIcon( aGulIcon );
        aGulIcon->SetBitmapsOwnedExternally( ETrue );
        if ( aBmpBuffer )
            {
            iBmpBuffer = aBmpBuffer;
            iBmpBuffer->IncRef();
            }
        if ( aMaskBuffer )
            {
            iMaskBuffer = aMaskBuffer;
            iMaskBuffer->IncRef();
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnExtBmpIconHolder::~CHnExtBmpIconHolder()
    {
    if ( iBmpBuffer )
        {
        iBmpBuffer->DecRef();
         }
    if ( iMaskBuffer )
        {
        iMaskBuffer->DecRef();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnExtBmpIconHolder::CleanBmpBuffer()
    {
    if ( iBmpBuffer )
        {
        iBmpBuffer->DecRef();
        iBmpBuffer = NULL;
        }
    if ( iMaskBuffer )
        {
        iMaskBuffer->DecRef();
        iMaskBuffer = NULL;
        }
    }
