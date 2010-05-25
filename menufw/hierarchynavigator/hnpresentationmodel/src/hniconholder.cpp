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
#include "hniconholder.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnIconHolder::SetGulIcon( CGulIcon* aGulIcon )
    {
    ASSERT( !iGulIcon ); // icon may be set only once
    iGulIcon = aGulIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CHnIconHolder::GetGulIcon() const
    {
    return iGulIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnIconHolder::~CHnIconHolder()
    {
    delete iGulIcon;
    }
