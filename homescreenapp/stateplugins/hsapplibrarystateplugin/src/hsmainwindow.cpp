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
 * Description: Menu view.
 *
 */

#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include "hsmainwindow.h"
#include "hsmenuview.h"

#include <qservicemanager.h>
/*!
 Constructor
 */
QTM_USE_NAMESPACE


/*
 * A word about HsMainWindow class.
 * This is not an utility class. It was designed to isolate HbMainWindow connected code, so
 * it can be substitued in MT for application library. Unfortunately it seems to be the best
 * way to cope with HbMainWindow problem in MT.  
 */


HsMainWindow::HsMainWindow() : mActivityClient(NULL)
{
}

/*!
 Destructor
 */
HsMainWindow::~HsMainWindow()
{
}

/*!
 Add the \menuView to first instance of HbMainWindow registered in HbInstance.
 \param view View to be added to HbMainWindow.
*/
void HsMainWindow::setCurrentView(const HsMenuView &menuView)
{
    HbMainWindow *const hbW(
        HbInstance::instance()->allMainWindows().value(0));
    
    connect( hbW, SIGNAL(viewReady()), SIGNAL(viewIsReady()),
        Qt::UniqueConnection );

    HbView *const view = menuView.view();
    
    if (!hbW->views().contains(view)) {
        hbW->addView(view);
    }
    hbW->setCurrentView(view);
}


/*!
 Grabs screenshot from actual main window
 \retval QPixmap& containing screenshot
*/
QPixmap HsMainWindow::grabScreenshot()
	{
	HbMainWindow *const hbW(
	    HbInstance::instance()->allMainWindows().value(0));
	return QPixmap::grabWidget(hbW, hbW->rect()); 
	}

/*!
 Saves applib activity with current view screenshot
*/
void HsMainWindow::saveActivity()
{
#ifdef Q_OS_SYMBIAN

    if (!mActivityClient) {
        QServiceManager serviceManager;
        mActivityClient = serviceManager.loadInterface("com.nokia.qt.activities.ActivityClient");
        if (!mActivityClient) {
            qWarning("Cannot initialize critical com.nokia.qt.activities.ActivityClient service.");
        }
    }

    if (mActivityClient) {
        bool retok;
        bool ok = QMetaObject::invokeMethod(mActivityClient, "removeActivity",
            Q_RETURN_ARG(bool, retok), Q_ARG(QString, appLibActivity()));
        if (!ok) {
            qWarning("remove appLibActivity failed");
        }

        QVariant variant;
        QVariantHash metadata;
        metadata.insert("screenshot", grabScreenshot());

        ok = QMetaObject::invokeMethod(mActivityClient, "addActivity", Q_RETURN_ARG(bool, retok),
            Q_ARG(QString, appLibActivity() ), Q_ARG(QVariant, QVariant(variant)),
            Q_ARG(QVariantHash, metadata));
        if (!ok) {
            qWarning("add appLibActivity failed");
        }
    }
#endif//Q_OS_SYMBIAN   
}

