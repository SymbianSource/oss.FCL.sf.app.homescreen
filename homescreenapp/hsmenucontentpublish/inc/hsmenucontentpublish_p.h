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
 * Description: hsmenucontentpublish_p.h
 *
 */

#ifndef HSMENUCONTENTPUBLISH_PRIVATE_H
#define HSMENUCONTENTPUBLISH_PRIVATE_H

// hidden
const QString hsItemId("id");

class HsMenuContentPublish;

class HsMenuContentPublishPrivate
{

public:

    explicit HsMenuContentPublishPrivate(HsMenuContentPublish *publishPublic);
    ~HsMenuContentPublishPrivate();
    bool add(const QVariantMap &entryPreference = QVariantMap());
    bool remove(const QVariantMap &queryPreference = QVariantMap()) const;    
    QList<QVariantMap > getList(const QVariantMap &queryPreference = QVariantMap());
    
private:
    /**
     * Points to the HsMenuContentPublish instance that uses this private implementation.
     */
    HsMenuContentPublish *const m_q;    
};

#endif //HSMENUCONTENTPUBLISH_PRIVATE_H
