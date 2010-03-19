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
* Description:  Clock widget
*
*/

#include "hsclockwidgetdocumentloader.h"

#include "hsanalogclockwidget.h"
#include "hsdigitalclockwidget.h"

/*!
    \class HsClockWidgetDocumentLoader
    \ingroup group_hsclockwidgetplugin
    \brief Custom homescreen themable clock docml layout loader.
 */

/*!
    Creates the homescreen themable clock custom docml objects.
    \param type Static type name.
    \param name Object name.
    \retval Created object.
 */
QObject *HsClockWidgetDocumentLoader::createObject(const QString &type, const QString &name)
{
    if (type == HsAnalogClockWidget::staticMetaObject.className()) {
        QObject *object = new HsAnalogClockWidget();
        object->setObjectName(name);
        return object;
    }
    else if (type == HsDigitalClockWidget::staticMetaObject.className()) {
        QObject *object = new HsDigitalClockWidget();
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}
