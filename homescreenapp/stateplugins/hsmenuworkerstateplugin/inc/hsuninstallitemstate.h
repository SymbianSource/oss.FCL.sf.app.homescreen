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

#ifndef HSUNINSTALLITEMSTATE_H
#define HSUNINSTALLITEMSTATE_H

#include <QState>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbAction;
class HbMessageBox;
class HsShortcutService;
class HsMenuService;
class HbDialog;
class CaNotifier;

class HsUninstallItemState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    enum UninstallDialogType {
        UninstallDialogDefinition02 = 1,
        UninstallDialogDefinition03,
        UninstallDialogDefinition04
    };

    HsUninstallItemState(QState *parent = 0);

    virtual ~HsUninstallItemState();

protected:

    void onEntry(QEvent *event);

private slots:

    void uninstallMessageFinished(HbAction* finishedAction);

    void cleanUp();

signals:

    void exit();

private:

    void construct();
    
    bool getApplicationsNames(QString &componentName,
        QStringList &applicationsNames,
        QString &confirmationMessage);
    
    void createSimpleUninstallMessage(bool isJava=false);
    void createUninstallJavaMessage();

private:

    int mItemId;
    
    HbMessageBox *mUninstallMessage; // deletes itself automatically on close
    HbDialog *mUninstallJavaMessage;
    UninstallDialogType mDialogType;

    QAction *mConfirmAction; // child for mConfirmMessage

};

#endif //HSUNINSTALLITEMSTATE_H
