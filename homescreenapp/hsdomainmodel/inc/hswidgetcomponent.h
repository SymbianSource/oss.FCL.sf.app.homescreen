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

#ifndef HSWIDGETCOMPONENT_H
#define HSWIDGETCOMPONENT_H

#include <QObject>
#include <QTranslator>

#include "hsdomainmodel_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestRuntimeServices)

#include "cadefs.h"

class CaEntry;

class HSDOMAINMODEL_EXPORT HsWidgetComponent : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString rootPath READ rootPath)
	Q_PROPERTY(QString uri READ uri)

public:
    ~HsWidgetComponent();

	QString rootPath() const;
	QString uri() const;

signals:
	void aboutToUninstall();
	
private:
    Q_DISABLE_COPY(HsWidgetComponent)
	HsWidgetComponent(const QString &uri, QObject *parent = 0);
	void listenChangeEvents();
	void resolveRootPath();
    void installTranslator();
	void uninstallTranslator();

private slots:
    void onEntryChanged(const CaEntry &entry, ChangeType changeType);

private:	
	QTranslator mTranslator;
	QString mUri;
	QString mRootPath;
 
	friend class HsWidgetComponentRegistry;

    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};

#endif // HSWIDGETCOMPONENT_H
