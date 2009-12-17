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
 * Description:  Presents publisher info
 *
 */

#ifndef PUBLISHERINFO_H
#define PUBLISHERINFO_H

#include <e32base.h>

namespace cpswrapper
    {
    /**
     * Content publishing service wrapper namespace
     */

    /**
     *  Presents publisher info
     *
     *
     *  @code
     *
     *  @endcode
     *
     *  @lib cpswrapper.lib
     *  @since S60 v5.0
     */
    NONSHARABLE_CLASS( CPublisherInfo ) : public CBase
        {
    public:
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPublisherInfo* NewL();
        IMPORT_C static CPublisherInfo* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C ~CPublisherInfo();

    protected:
        // constructors
        CPublisherInfo();

        void ConstructL();

    public:
        // new functions

        /**
         * Set content id
         *
         * @param aContentId content id
         */
        IMPORT_C void SetContentIdL( const TDesC& aContentId );

        /**
         * Get content id
         *
         * @return content id or KNullDesC8
         */
        IMPORT_C const TDesC& ContentId() const;

        /**
         * Set publisher id
         *
         * @param aPublisherId publisher id
         */
        IMPORT_C void SetPublisherIdL( const TDesC& aPublisherId );

        /**
         * Get Publisher id
         *
         * @return Publisher id or KNullDesC8
         */
        IMPORT_C const TDesC& PublisherId() const;

        /**
         * Set Widget name
         *
         * @param aWidgetName widget name
         */
        IMPORT_C void SetWidgetNameL( const TDesC& aWidgetName );

        /**
         * Get Widget name
         *
         * @return widget name or KNullDesC8
         */
        IMPORT_C const TDesC& WidgetName() const;

        /**
         * Set content type
         *
         * @param aContentType content type
         */
        IMPORT_C void SetContentTypeL( const TDesC& aContentType );

        /**
         * Get Content type
         *
         * @return Content type or KNullDesC8
         */
        IMPORT_C const TDesC& ContentType() const;

        /**
         * Set Template type
         *
         * @param aTemplateType template type
         */
        IMPORT_C void SetTemplateTypeL( const TDesC& aTemplateType );

        /**
         * Get Template type
         *
         * @return Template type or KNullDesC
         */
        IMPORT_C const TDesC& TemplateType() const;

        /**
         * Set Logo icon
         *
         * @param aLogoIcon logo icon
         */
        IMPORT_C void SetLogoIconL( const TDesC& aLogoIcon );

        /**
         * Get Logo icon
         *
         * @return LogoIcon logo icon or KNullDesC
         */
        IMPORT_C const TDesC& LogoIcon() const;

        /**
         * Set widget description
         *
         * @param aDescription widget description
         */
        IMPORT_C void SetDescriptionL( const TDesC& aDescription );

        /**
         * Get widget description
         *
         * @return Description widget description or KNullDesC
         */
        IMPORT_C const TDesC& Description() const;
        
        /**
         * Set Maximum number of widgets
         *
         * @param aMaxWidget maximum number of widgets
         */
        IMPORT_C void SetMaxWidgets( const TInt32& aMaxWidget );

        /**
         * Get Maximum number of widgets
         *
         * @return number of widgets or defaule value 1
         */
        IMPORT_C const TInt32& MaxWidgets() const;

    private:
        // data
        // Content id, owned
        HBufC* iContentId;
        // Publisher id, owned
        HBufC* iPublisherId;
        // Publisher name, owned
        HBufC* iWidgetName;
        // Content type, owned
        HBufC* iContentType;
        // Template type, owned
        HBufC* iTemplateType;
        // Logo icon, owned
        HBufC* iLogoIcon;
        // Widget description, owned
        HBufC* iDescription;
        // Maximum instances
        TInt32 iMaxInst;
        };
    }

#endif // PUBLISHERINFO_H
// End of file
