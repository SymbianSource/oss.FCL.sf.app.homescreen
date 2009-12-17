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
 * Description:  Presents publisher info map
 *
 */

// User includes
#include "publisherinfo.h"
#include "publishermap.h"

namespace cpswrapper
    {
    // ======== MEMBER FUNCTIONS ========

    // -----------------------------------------------------------------------
    // CPublisherMap::CPublisherMap()
    // -----------------------------------------------------------------------
    //
    CPublisherMap::CPublisherMap()
        {
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::ConstructL()
    // -----------------------------------------------------------------------
    //
    void CPublisherMap::ConstructL()
        {
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::NewL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherMap* CPublisherMap::NewL()
        {
        CPublisherMap* self = CPublisherMap::NewLC();
        CleanupStack::Pop( self );
        return self;
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::NewLC()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherMap* CPublisherMap::NewLC()
        {
        CPublisherMap* self = new ( ELeave ) CPublisherMap;
        CleanupStack::PushL( self );
        self->ConstructL();
        return self;
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::~CPublisherMap()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherMap::~CPublisherMap()
        {
        iPublisherInfo.ResetAndDestroy();
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::AddPublisherInfoL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherMap::AddPublisherInfoL(
        CPublisherInfo* aPublisherInfo )
        {
        User::LeaveIfError(
            iPublisherInfo.InsertInAddressOrder( aPublisherInfo ));
        }

    // -----------------------------------------------------------------------
    // CPublisherMap::PublisherInfo()
    // -----------------------------------------------------------------------
    //
    EXPORT_C RPointerArray<CPublisherInfo>& CPublisherMap::PublisherInfo() const
        {
        return iPublisherInfo;
        }
    }

// End of file
