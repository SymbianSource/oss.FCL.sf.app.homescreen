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

#ifndef TSDEVICEDIALOG_H
#define TSDEVICEDIALOG_H

#include <HbDialog>
#include <HbDeviceDialogInterface>

class QGraphicsLinearLayout;
class HbDocumentLoader;

class TsDeviceDialog : public HbDialog, public HbDeviceDialogInterface
{
    Q_OBJECT

public:
    TsDeviceDialog(HbDocumentLoader &loader, QGraphicsItem *parent = 0);
    ~TsDeviceDialog();

    virtual bool setDeviceDialogParameters(const QVariantMap &parameters);
    virtual int deviceDialogError() const;
    virtual void closeDeviceDialog(bool byClient);
    virtual HbPopup *deviceDialogWidget() const;

signals:
    void deviceDialogClosed();

public slots:
    void changeOrientation(Qt::Orientation orientation);

private:
    HbDocumentLoader &mLoader;

};

#endif // TSDEVICEDIALOG_H
