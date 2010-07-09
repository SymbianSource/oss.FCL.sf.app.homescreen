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
 * Description: Base for menu view states.
 *
 */

#ifndef HSBASEVIEWSTATE_H
#define HSBASEVIEWSTATE_H

#include <QState>
#include <QPointer>
#include <QModelIndex>

#include "hsmenustates_global.h"
#include "hsmenuservice.h"
#include "hsmenuview.h"

class HbMenu;
class HbAction;
class HbMessageBox;
class HbAbstractViewItem;
class CaNotifier;
class HsMenuModeWrapper;
class HsMainWindow;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsBaseViewState: public QState
{
    Q_OBJECT
    
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
    
public:
    
    HsBaseViewState(HsMainWindow &mainWindow, QState *parent);
    HsBaseViewState(HsMainWindow &mainWindow, 
        HsMenuModeWrapper& menuMode, QState *parent);
    ~HsBaseViewState();
    void scrollToBeginning();

private slots:

    virtual void applicationLaunchFailMessageFinished(HbAction*);
    virtual void openAppLibrary();

protected slots:
    
    virtual void stateEntered();
    virtual void stateExited();
    virtual void addModeEntered();
    virtual void normalModeEntered();
    virtual void normalModeExited();
    virtual void launchItem(const QModelIndex &index);
    virtual void openCollection(const QModelIndex &index);
    virtual void showContextMenu(HbAbstractViewItem *item, const QPointF &coords);
    virtual int checkSoftwareUpdates();
    virtual bool openTaskSwitcher();
protected:
    
    void initialize(HsMenuViewBuilder &menuViewBuilder, HsViewContext viewContext);
    void createApplicationLaunchFailMessage(int errorCode,int itemId);
    void subscribeForMemoryCardRemove();

    void defineTransitions();

private:
    
    void cleanUpApplicationLaunchFailMessage();

    virtual void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags) = 0;
    virtual void setMenuOptions() = 0;


private:
    
    CaNotifier *mNotifier;
    int mMessageRelatedItemId;
    HbMessageBox *mApplicationLaunchFailMessage;

protected:
    HsMenuItemModel *mModel;
    QPointer<HbMenu> mContextMenu;
    QModelIndex mContextModelIndex;
    HbAction *mBackKeyAction;
    QScopedPointer<HsMenuView> mMenuView;
    HsMenuModeWrapper *mMenuMode;
    HsMainWindow &mMainWindow;
};


#endif // HSBASEVIEWSTATE_H
