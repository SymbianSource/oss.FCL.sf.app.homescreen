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

#ifndef HS_INPUT_DIALOG_H
#define HS_INPUT_DIALOG_H

#include <hbinputdialog.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HsCollectionNameDialog: public HbInputDialog
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsCollectionNameDialog(const int &itemId = 0);

    virtual ~HsCollectionNameDialog();

    HbAction *exec();

    QString newName(const QString &name, bool addLastName = false);

private:

    /**
     * Connects edit line signals to slots.
     */
    void makeConnect();

    /**
     * Disconnects edit line signals from slots.
     */
    void makeDisconnect();

private slots:

    /**
     * This slot is received whenever the text changes.
     * The text argument is the new text.
     * This slot is also received when the text is
     *  changed programmatically,
     * for example, by calling setText().
     */
    void onTextChanged(const QString &text);

    void onContentsChanged();

private:

    /*
     * Collections names list from data model.
     */
    QStringList mCollectionsNames;

    /*
     * Last found collection name.
     */
    QString mLastCollectionName;

};

#endif /* HS_INPUT_DIALOG_H */
