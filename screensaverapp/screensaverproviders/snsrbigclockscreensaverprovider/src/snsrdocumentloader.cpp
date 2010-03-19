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

#include "snsrdocumentloader.h"

#include "snsranalogclockwidget.h"
#include "snsroledclockwidget.h"
#include "snsrlabel.h"

/*!
    \class SnsrDocumentLoader
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Custom screensaver docml layout loader.
    
    Used mainly to create the analog clock widget from docml.
 */

/*!
    Creates the analog clock.
    \param type Static type name.
    \param name Object name.
    \retval Created object.
 */
QObject *SnsrDocumentLoader::createObject(const QString &type, const QString &name)
{
    if (type == SnsrAnalogClockWidget::staticMetaObject.className()) {
        QObject *object = new SnsrAnalogClockWidget();
        object->setObjectName(name);
        return object;
    }
	
    if (type == SnsrOledClockWidget::staticMetaObject.className()) {
        QObject *object = new SnsrOledClockWidget();
        object->setObjectName(name);
        return object;
    }

    if (type == SnsrLabel::staticMetaObject.className()) {
        QObject *object = new SnsrLabel();
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}
