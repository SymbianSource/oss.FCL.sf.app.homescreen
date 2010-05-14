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
* Description:  Menu Application Library state.
 *
*/


#ifndef HSADDSHORTCUTSTATE_H
#define HSADDSHORTCUTSTATE_H

#include <hsmenubasestate.h>

#include "hsmenustates_global.h"
#include "hsapp_defs.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbAction;
class CaEntry;
class HbMessageBox;


class HsAddToHomeScreenState: public  HsMenuBaseState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAddToHomeScreenState(QState *parent = 0);

    virtual ~HsAddToHomeScreenState();

private slots:

    void onEntry(QEvent *event);

    void cleanUp();

    void messageWidgetCorruptedFinished(HbAction* finishedAction);

signals:

    void exit();

private:

    void showMessageWidgetCorrupted();

    bool addWidget(HsContentService &service, const QString &uri);

    bool addShortcut(HsContentService &contentService);

    bool addApplication(HsContentService &contentService, CaEntry &entryId);

    void logActionResult(QString operationName, int entryId,
                         bool operationSucceded);

private:

    // keep path in memory so it wont
    QString mLibraryPath;

    int mEntryId;

    HbMessageBox *mCorruptedMessage;

    HbAction *mConfirmAction;
    HsMenuMode mMenuMode;

};

#endif //HSADDSHORTCUTSTATE_H
