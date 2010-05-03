/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef HSVIEWAPPSETTINGSSTATE_H
#define HSVIEWAPPSETTINGSSTATE_H

#include <QState>

#include "hsmenustates_global.h"
#include "hbmainwindow.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuService;
class HbView;
class HbAction;
class CaNotifier;

class HsViewAppSettingsState : public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param parent Owner.
     */
    HsViewAppSettingsState(QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsViewAppSettingsState();
    
private slots:
    
    /**
     * Slot connected trrigger action of secondary soft key of check list box.
     * It is called when done button is selected.
     *
     * @since S60 ?S60_version.
     */
    void settingsDone();
    
signals:
        void initialize(const QString &aUid);      

protected:

    void onEntry(QEvent *event);

private:

    /**
     * Constructs contained objects.
     *
     * @since S60 ?S60_version.
     */
    void construct();
    
    /**
     * Returns pointer to tha main window.
     *
     * @since S60 ?S60_version.
     * @return Pointer to the main window.
     */
    HbMainWindow *mainWindow() const; 
    
    void subscribeForMemoryCardRemove(int entryId);

private:
    /**
     * View. Owned.
     */
    HbView *mView;

    /**
     * Previous view. Not owned.
     */
    HbView *mPreviousView;
    
    /**
     * Confirm action. Owned.
     */
    HbAction *mActionConfirm;
    
    CaNotifier *mNotifier;

};



#endif //HSVIEWAPPSETTINGSSTATE_H
