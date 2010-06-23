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
#ifndef TSDEVICEDIALOGCONTAINER_H
#define TSDEVICEDIALOGCONTAINER_H

#include <QObject>

#include <HbDeviceDialogInterface>

#include "tsdocumentloader.h"

class QAbstractListModel;

class TsDeviceDialogContainer : public QObject, public HbDeviceDialogInterface
{
Q_OBJECT
public:
    explicit TsDeviceDialogContainer(QAbstractListModel *model, QObject *parent = 0);
    ~TsDeviceDialogContainer();

public:
    virtual bool setDeviceDialogParameters(const QVariantMap &parameters);
    virtual int deviceDialogError() const;
    virtual void closeDeviceDialog(bool byClient);
    virtual HbPopup *deviceDialogWidget() const;
    virtual QObject *signalSender() const;

signals:
    void deviceDialogClosed();

public slots:
    void changeOrientation(Qt::Orientation orientation);

private:
    TsDocumentLoader mLoader;

};

#endif // TSDEVICEDIALOGCONTAINER_H
