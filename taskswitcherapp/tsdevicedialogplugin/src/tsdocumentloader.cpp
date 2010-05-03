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
* Description: Screensaver custom document loader.
*
*/

#include "tsdocumentloader.h"
#include "tstasksgrid.h"
#include "tsdevicedialog.h"

/*!
    \class TsDocumentLoader
    \ingroup group_tsdevicedialogplugin
    \brief Document loader.

    TS implementation of document loader. It is creating TS objects used in UI
*/

QObject *TsDocumentLoader::createObject(const QString &type, const QString &name)
{
    if (type == TsTasksGrid::staticMetaObject.className()) {
        QObject *object = new TsTasksGrid();
        object->setObjectName(name);
        return object;
    } else if (type == TsDeviceDialog::staticMetaObject.className()) {
        QObject *object = new TsDeviceDialog(*this);
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}
