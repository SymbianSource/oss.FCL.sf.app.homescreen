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

#ifndef PUBLISHERMAP_H
#define PUBLISHERMAP_H

#include <e32base.h>


namespace cpswrapper
    {
    // Forward declarations
    class CPublisherInfo;

    /**
     *  Presents publisher info map
     *
     *
     *  @code
     *
     *  @endcode
     *
     *  @lib cpswrapper.lib
     *  @since S60 v5.0
     */
    NONSHARABLE_CLASS( CPublisherMap ) : public CBase
        {
    public:
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPublisherMap* NewL();
        IMPORT_C static CPublisherMap* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C ~CPublisherMap();

    protected:
        // constructors
        CPublisherMap();

        void ConstructL();

    public:
        // new functions

        /**
         * Adds publisher info
         *
         * @param aPublisherInfo publisher info
         */
        IMPORT_C void AddPublisherInfoL(CPublisherInfo* aPublisherInfo);

        /**
         * Gets publisher infos
         *
         * @return Array of publisher infos
         */
        IMPORT_C RPointerArray<CPublisherInfo>& PublisherInfo() const;

    private:
        // data
        // Publisher info array, owned
        mutable RPointerArray<CPublisherInfo> iPublisherInfo;
        };
    }

#endif // PUBLISHERMAP_H
// End of file
