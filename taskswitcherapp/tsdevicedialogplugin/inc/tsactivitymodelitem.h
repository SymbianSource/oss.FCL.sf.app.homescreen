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
* Description: tsactivitymodelitem.h
*
*/

#ifndef TSACTIVITYMODELITEM_H
#define TSACTIVITYMODELITEM_H
#include "tsmodelitem.h"
#include <qvariant.h>
#include <qicon.h>
/*!
    Implements model item which represents Activity entry
*/
class TsActivityModelItem : public TsModelItem
{
    Q_OBJECT
public:
    TsActivityModelItem(QAbstractListModel &model, QObject &service, const QVariantHash &activity);
    ~TsActivityModelItem();
    QVariant data(int role) const;
    void close();
    void open();
    static QString applicationKeyword();
    static QString visibilityKeyword();
    static QString screenshotKeyword();

private:
    QVariant decorationRole() const;
    QVariant displayRole() const;
    QVariant visibilityRole() const;

private slots:
    void thumbnailReady(QPixmap, void *);
signals:
    void entryChanged(TsModelItem *);

private:
    QObject &mService;
    const QVariantHash mActivity;
    QIcon mIcon;
    bool mRequetPending;
};

#endif // TSACTIVITYMODELITEM_H
