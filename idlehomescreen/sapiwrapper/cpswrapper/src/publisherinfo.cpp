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

// User includes
#include "publisherinfo.h"

namespace cpswrapper
    {
    // ======== MEMBER FUNCTIONS ========

    // -----------------------------------------------------------------------
    // CPublisherInfo::CPublisherInfo()
    // -----------------------------------------------------------------------
    //
    CPublisherInfo::CPublisherInfo()
        {        
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::ConstructL()
    // -----------------------------------------------------------------------
    //
    void CPublisherInfo::ConstructL()
        {
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::NewL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherInfo* CPublisherInfo::NewL()
        {
        CPublisherInfo* self = CPublisherInfo::NewLC();
        CleanupStack::Pop(self);
        return self;
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::NewLC()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherInfo* CPublisherInfo::NewLC()
        {
        CPublisherInfo* self = new ( ELeave ) CPublisherInfo;
        CleanupStack::PushL( self );
        self->ConstructL();
        return self;
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::~CPublisherInfo()
    // -----------------------------------------------------------------------
    //
    EXPORT_C CPublisherInfo::~CPublisherInfo()
        {
        delete iContentId;
        delete iPublisherId;
        delete iWidgetName;
        delete iContentType;
        delete iTemplateType;
        delete iLogoIcon;
        delete iDescription;
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetContentIdL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetContentIdL( const TDesC& aContentId )
        {
        delete iContentId;
        iContentId = NULL;

        iContentId = aContentId.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::ContentId()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::ContentId() const
        {
        return iContentId ? *iContentId : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetPublisherIdL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetPublisherIdL( const TDesC& aPublisherId )
        {
        delete iPublisherId;
        iPublisherId = NULL;

        iPublisherId = aPublisherId.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::PublisherId()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::PublisherId() const
        {
        return iPublisherId ? *iPublisherId : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetPublisherNameL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetWidgetNameL( const TDesC& aPublisherName )
        {
        delete iWidgetName;
        iWidgetName = NULL;

        iWidgetName = aPublisherName.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::PublisherName()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::WidgetName() const
        {
        return iWidgetName ? *iWidgetName : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetContentTypeL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetContentTypeL( const TDesC& aContentType )
        {
        delete iContentType;
        iContentType = NULL;

        iContentType = aContentType.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::ContentType()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::ContentType() const
        {
        return iContentType ? *iContentType : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetTemplateTypeL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetTemplateTypeL( const TDesC& aTemplateType )
        {
        delete iTemplateType;
        iTemplateType = NULL;

        iTemplateType = aTemplateType.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::TemplateType()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::TemplateType() const
        {
        return iTemplateType ? *iTemplateType : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetLogoIconL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetLogoIconL( const TDesC& aLogoIcon )
        {
        delete iLogoIcon;
        iLogoIcon = NULL;

        iLogoIcon = aLogoIcon.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::LogoIcon()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::LogoIcon() const
        {
        return iLogoIcon ? *iLogoIcon : KNullDesC();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::SetDescriptionL()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetDescriptionL( const TDesC& aDescription )
        {
        delete iDescription;
        iDescription = NULL;

        iDescription = aDescription.AllocL();
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::Description()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TDesC& CPublisherInfo::Description() const
        {
        return iDescription ? *iDescription : KNullDesC();
        }
        
    // -----------------------------------------------------------------------
    // CPublisherInfo::SetMaxWidgets()
    // -----------------------------------------------------------------------
    //
    EXPORT_C void CPublisherInfo::SetMaxWidgets( const TInt32& aMaxWidget )
        {
        iMaxInst = aMaxWidget;
        }

    // -----------------------------------------------------------------------
    // CPublisherInfo::MaxWidgets()
    // -----------------------------------------------------------------------
    //
    EXPORT_C const TInt32& CPublisherInfo::MaxWidgets() const
        {
        return iMaxInst;
        }
    }

// End of file

