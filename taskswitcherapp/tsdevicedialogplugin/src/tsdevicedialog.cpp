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
* Description: Default Screensaver runtime.
*
*/

#include <HbMainWindow>
#include <HbDocumentLoader>

#include "tsdevicedialog.h"

/*!
    \class TsDeviceDialog
    \ingroup group_tsdevicedialogplugin
    \brief TaskSwitcher Device Dialog.
 */

/*!
    Constructor.
    \param parent Parent.
 */
TsDeviceDialog::TsDeviceDialog(HbDocumentLoader &loader, QGraphicsItem *parent) : HbDialog(parent), mLoader(loader)
{
    connect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(changeOrientation(Qt::Orientation)));
    connect(this, SIGNAL(aboutToClose()), this, SIGNAL(deviceDialogClosed()));
}

TsDeviceDialog::~TsDeviceDialog()
{
    qDebug("Destruction");
}

/*!
    Sets device dialog parameters.
    \param parameters Device Dialog's parameters to be set.
    \ret return true if parameters are correct.
 */
bool TsDeviceDialog::setDeviceDialogParameters(const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    return false;
}

/*!
    Returns error code.
    \ret error code.
 */
int TsDeviceDialog::deviceDialogError() const
{
    return 0;
}

/*!
    Closes device dialog.
    \param byClient If dialog is close by client.
 */
void TsDeviceDialog::closeDeviceDialog(bool byClient)
{
    Q_UNUSED(byClient)
    hide();
    emit deviceDialogClosed();
}

/*!
    Returns device dialog instance.
    \ret Device dialog instance.
 */
HbPopup *TsDeviceDialog::deviceDialogWidget() const
{
    return const_cast<TsDeviceDialog *>(this);
}

void TsDeviceDialog::changeOrientation(Qt::Orientation orientation)
{
    qDebug("Loading DOCML data for orientation %d", orientation);
    bool ok(true);
    if (orientation == Qt::Horizontal) {
        mLoader.load(":/xml/resource/layout.docml", "landscape", &ok);
    } else {
        mLoader.load(":/xml/resource/layout.docml", "portrait", &ok);
    }
    Q_ASSERT(ok);
}
