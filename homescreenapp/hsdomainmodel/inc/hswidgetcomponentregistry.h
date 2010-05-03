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

#ifndef HSWIDGETCOMPONENTREGISTRY_H
#define HSWIDGETCOMPONENTREGISTRY_H

#include <QObject>

#include "hsdomainmodeldatastructures.h"
#include "hsdomainmodel_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestRuntimeServices)

class HsWidgetComponent;
class QSignalMapper;

class HSDOMAINMODEL_EXPORT HsWidgetComponentRegistry : public QObject
{
    Q_OBJECT

public:
    static HsWidgetComponentRegistry *instance();    
    ~HsWidgetComponentRegistry();
	
	HsWidgetComponent *component(const QString &uri);

private:
    Q_DISABLE_COPY(HsWidgetComponentRegistry)
    HsWidgetComponentRegistry(QObject *parent = 0);

private slots:
    void onAboutToUninstall(const QString &uri);

private:
	QHash<QString, HsWidgetComponent *> mRegistry;
	static QScopedPointer<HsWidgetComponentRegistry> mInstance;
    QScopedPointer<QSignalMapper> mSignalMapper;
    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};

#endif // HSWIDGETCOMPONENTREGISTRY_H
