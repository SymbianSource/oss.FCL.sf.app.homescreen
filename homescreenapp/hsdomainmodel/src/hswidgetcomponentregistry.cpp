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

#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include <QSignalMapper>
QScopedPointer<HsWidgetComponentRegistry> HsWidgetComponentRegistry::mInstance(0);

/*!

*/
HsWidgetComponentRegistry* HsWidgetComponentRegistry::instance()
{
    if (!mInstance) {
        mInstance.reset(new HsWidgetComponentRegistry);
    }
    return mInstance.data();
}

/*!

*/
HsWidgetComponentRegistry::~HsWidgetComponentRegistry()
{
    qDeleteAll(mRegistry);
}

/*!

*/
HsWidgetComponent *HsWidgetComponentRegistry::component(const QString &uri)
{
	if (mRegistry.contains(uri)) {
        return mRegistry.value(uri);
	}

	QScopedPointer<HsWidgetComponent> c(new HsWidgetComponent(uri));
	mRegistry.insert(uri, c.data());
 	connect(c.data(), SIGNAL(aboutToUninstall()), mSignalMapper.data(), 
        SLOT(map()));
    mSignalMapper->setMapping(c.data(),uri);
	return c.take();
}

/*!

*/
HsWidgetComponentRegistry::HsWidgetComponentRegistry(QObject *parent)
  : QObject(parent),mSignalMapper(new QSignalMapper())
{
    connect(mSignalMapper.data(),SIGNAL(mapped(QString)),SLOT(onAboutToUninstall(QString)));
}

/*!

*/
void HsWidgetComponentRegistry::onAboutToUninstall(const QString &uri)
{
    if (mRegistry.contains(uri)) {
        mRegistry.take(uri)->deleteLater();
    }
}
