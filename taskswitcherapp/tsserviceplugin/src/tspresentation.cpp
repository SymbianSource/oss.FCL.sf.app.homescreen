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
#include "tspresentation.h"

#include <hbinstance.h>
#include <hbview.h>
#include <hbdialog.h>

#include "tstasksgrid.h"
#include "tstasksgriditem.h"
#include "tstaskpopuphandler.h"

// FIXME: workaround for Orbit bug: not moving to foreground after QWidget::activateWindow();
#ifdef Q_OS_SYMBIAN
#include <APGTASK.H>
#include <eikenv.h>
#endif    

TsPresentation::TsPresentation(QObject *parent) : TsPresentationInterface(parent), mDialog(NULL), mGrid(NULL)
{
    HbMainWindow *mainWindow = HbInstance::instance()->allMainWindows().first();

    // load layout from XML
    bool ok(true);
    mLoader.load(":/xml/resource/layout.docml", &ok);
    Q_ASSERT(ok);
    loadOrientationSpecificLayoutData(mainWindow->orientation());

    // get all items
    HbView *view = qobject_cast<HbView*>(mLoader.findWidget("view"));
    mGrid = qobject_cast<TsTasksGrid*>(mLoader.findWidget("taskgrid"));
    mDialog = qobject_cast<HbDialog*>(mLoader.findWidget("dialog"));
    Q_ASSERT(view);
    Q_ASSERT(mGrid);
    Q_ASSERT(mDialog);

    mGrid->setItemPrototype(new TsTasksGridItem());

    // forward grid signals
    connect(mGrid, SIGNAL(activated(QModelIndex)), this, SIGNAL(openApplication(QModelIndex)));
    connect(mGrid, SIGNAL(activated(QModelIndex)), this, SIGNAL(hideTaskSwitcher()));
    qRegisterMetaType<QModelIndex>("QModelIndex");
    connect(mGrid, SIGNAL(deleteButtonClicked(QModelIndex)), this, SIGNAL(closeApplication(QModelIndex)), Qt::QueuedConnection);

    // task popup
    TsTaskPopupHandler *popupHandler = new TsTaskPopupHandler(this);
    connect(mGrid, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), popupHandler, SLOT(showTaskPopup(HbAbstractViewItem*, QPointF)));
    connect(popupHandler, SIGNAL(closeTask(QModelIndex)), this, SIGNAL(closeApplication(QModelIndex)),Qt::QueuedConnection);
    connect(popupHandler, SIGNAL(closeAllTasks()), this, SIGNAL(closeAllApplications()));

    // dismiss
    connect(mDialog, SIGNAL(aboutToHide()), this, SIGNAL(hideTaskSwitcher()));

    // orientation change handling
    connect(mainWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(loadOrientationSpecificLayoutData(Qt::Orientation)));

    mainWindow->addView(view);
}

void TsPresentation::setModel(QAbstractItemModel *model)
{
    mGrid->setModel(model);
}

void TsPresentation::hide()
{
    mDialog->hide();
    HbInstance::instance()->allMainWindows().first()->hide();
}

void TsPresentation::show()
{
    HbMainWindow *window = HbInstance::instance()->allMainWindows().first();
    window->show();
    window->activateWindow();
    
#ifdef Q_OS_SYMBIAN
    // FIXME: workaround for Orbit bug: not moving to foreground after QWidget::activateWindow();
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(RProcess().Type()[2]);
    if (task.Exists()) {
        task.BringToForeground();
    } else {
        qWarning("Can't bring TaskSwitcher to foreground: task not found");
    }    
#endif    
   
    mDialog->show();
}

void TsPresentation::loadOrientationSpecificLayoutData(Qt::Orientation orientation)
{
    bool ok(true);
    if (orientation == Qt::Horizontal) {
        mLoader.load(":/xml/resource/layout.docml", "landscape", &ok);
    } else {
        mLoader.load(":/xml/resource/layout.docml", "portrait", &ok);
    }
    Q_ASSERT(ok);
}
