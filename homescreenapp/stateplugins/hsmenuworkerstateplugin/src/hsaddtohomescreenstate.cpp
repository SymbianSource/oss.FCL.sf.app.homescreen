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

#include <qstatemachine.h>
#include <hblabel.h>
#include <hsshortcutservice.h>
#include <hscontentservice.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <hbnotificationdialog.h>
#include <hsmenueventfactory.h>
#include "hsmenudialogfactory.h"
#include "hsmenuentryremovedhandler.h"

#include "hsaddtohomescreenstate.h"
#include "hsmenuevent.h"
#include "canotifier.h"
#include "canotifierfilter.h"
#include "caservice.h"
#include "caquery.h"
#include "hsmenuservice.h"

const char SHORTCUT_WIDGET_URI[] = "hsshortcutwidgetplugin";
const char SHORTCUT_ID[] = "caEntryId";



/*!
 \class HsAddToHomeScreenState
 \ingroup group_hsmenustateplugin
 \brief Application Library State.
 \see StateMachine
 \lib ?library
 Parent state for Application Library functionality
 (browsing applications and collections)
 */

/*!
 Constructor
 \param parent Parent state.
 \retval void
 */
HsAddToHomeScreenState::HsAddToHomeScreenState(QState *parent) :
    QState(parent), 
    mCorruptedMessage(NULL), mConfirmAction(NULL),
    mMenuMode(Hs::NormalHsMenuMode)
{
    setObjectName("/AddToHomeScreenState");
    
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Destructor. Empty one.
 */
HsAddToHomeScreenState::~HsAddToHomeScreenState()
{
    cleanUp();
}

/*!
 onEntry method invoked on entering the state
 Adds a widget or shortcut to the active page of home screen.
 It reads "data" property which value is a string map.
 The map can keep either data specific for shortcut (item id)
 or widget (entry type name, widget library name, widget uri).
 Depending on the property values it calls method for adding widget
 or shortcut.
 \param contentService: service for adding widgets
 \retval void
 */
void HsAddToHomeScreenState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsAddToHomeScreenState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mEntryId = data.value(Hs::itemIdKey).toInt();
    QSharedPointer<CaEntry> entry = CaService::instance()->getEntry(mEntryId);
    const QString entryTypeName = entry->entryTypeName();
    
    mMenuMode = static_cast<Hs::HsMenuMode>(data.value(Hs::menuModeType).toInt());
    mToken = data.value(Hs::homescreenData);

    bool success = false;
    if (entryTypeName == Hs::widgetTypeName) {
        const QString uri = entry->attribute(Hs::widgetUriAttributeName);
        success = addWidget(*HsContentService::instance(), uri);        
    } else {
        success = addApplication(*HsContentService::instance(), *entry);
    }
    
    if (success && (mMenuMode == Hs::NormalHsMenuMode)) {
        HbNotificationDialog *notificationDialog = new HbNotificationDialog();
        notificationDialog->setAttribute(Qt::WA_DeleteOnClose);
        notificationDialog->setSequentialShow(false);
        notificationDialog->setTitle(hbTrId(
                                     "txt_applib_dpophead_added_to_homescreen") );
        notificationDialog->show();
    }

    HSMENUTEST_FUNC_EXIT("HsAddToHomeScreenState::onEntry");
}

/*!
 Adds a widget to active page of home screen
 \param contentService: service for adding widgets
 \param library: widget library path and file name
 \param uri: widget uri
 \param entryId: widget entry id (ignored)
 \param activePage: active page of home screen
 \retval true if successful
 */
bool HsAddToHomeScreenState::addWidget(HsContentService &contentService, 
        const QString &uri)
{
    HSMENUTEST_FUNC_ENTRY("HsAddToHomeScreenState::addWidget");
    QVariantHash params;
    params[Hs::uri] = uri;
    params[Hs::homescreenData] = mToken;
    bool success = contentService.createWidget(params);
    if (!success) {
        showMessageWidgetCorrupted();
    } else {
        emit exit();
        if (mMenuMode == Hs::AddHsMenuMode) {
            machine()->postEvent(
                HsMenuEventFactory::createOpenHomeScreenEvent());
        }
    }
    logActionResult("Adding widget", mEntryId, success);
    HSMENUTEST_FUNC_EXIT("HsAddToHomeScreenState::addWidget");
    return success;
}

/*!
 Shows message about corrupted widget library. Deletes widget eventually
 \param itemId entryId of widget (needed to delete it)
 \retval void
 */
void HsAddToHomeScreenState::showMessageWidgetCorrupted()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::showMessageWidgetCorrupted");

    mCorruptedMessage = HsMenuDialogFactory().create(
            hbTrId("txt_applib_dialog_file_corrupted_unable_to_use_wi"));
    mConfirmAction = mCorruptedMessage->actions().value(0);
    
    QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
        new HsMenuEntryRemovedHandler(mEntryId, 
            mCorruptedMessage, SLOT(close())));
    
    entryObserver.take()->setParent(mCorruptedMessage); 
    
    mCorruptedMessage->open(this, SLOT(messageWidgetCorruptedFinished(HbAction*)));

    HSMENUTEST_FUNC_EXIT("HsCollectionState::showMessageWidgetCorrupted");
}

/*!
 Slot launched on dismissing the corrupted widget error note
 \retval void
 */
void HsAddToHomeScreenState::messageWidgetCorruptedFinished
        (HbAction* finishedAction)
{
    if (static_cast<QAction*>(finishedAction) == mConfirmAction) {
        HsMenuService::executeAction(mEntryId, Hs::removeActionIdentifier);
    }
    emit exit();
    if (mMenuMode == Hs::AddHsMenuMode) {
        machine()->postEvent(
            HsMenuEventFactory::createOpenHomeScreenEvent());
    }
    mConfirmAction = NULL;
}


/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsAddToHomeScreenState::cleanUp()
{
    // Close popups if App key was pressed
    if (mCorruptedMessage != NULL) {
        mCorruptedMessage->close();
        mCorruptedMessage = NULL;
    }
    
    mToken = NULL;
}


/*!
 Adds a shortcut to active page of home screen
 \param shortcutService: service for adding shortcuts
 \param entryId: menu entry id
 \param activePage: active page of home screen
 \retval true if successful
 */
bool HsAddToHomeScreenState::addShortcut(HsContentService &contentService)
{
    HSMENUTEST_FUNC_ENTRY("HsAddToHomeScreenState::addShortcut");
    QVariantHash params;
    params[Hs::uri] = SHORTCUT_WIDGET_URI;
    QVariantHash preferences;
    preferences[SHORTCUT_ID] = QString::number(mEntryId);
    params[Hs::preferences] = preferences;
    params[Hs::homescreenData] = mToken;
    const bool result = contentService.createWidget(params);
    logActionResult("Adding shortcut", mEntryId, result);
    HSMENUTEST_FUNC_EXIT("HsAddToHomeScreenState::addShortcut");
    return result;
}

/*!
 Adds a tapplication to active page of home screen
 \param shortcutService: service for adding shortcuts
 \param entryId: menu entry id
 \param data: data from event
 \retval true if successful
 */
bool HsAddToHomeScreenState::addApplication(HsContentService &contentService,
                                             CaEntry& entry)
{
    bool success = false;
    if (entry.attributes().contains(Hs::widgetUriAttributeName)) {
        QVariantHash params;
        const QString uri = entry.attribute(Hs::widgetUriAttributeName);
        params[Hs::uri] = uri;       
        
        QVariantHash preferences;
        QMap<QString, QString> attributes = entry.attributes();
        QMapIterator<QString, QString> i(attributes);
        while (i.hasNext()) {
            i.next();
            QString key(i.key());
            QString value(i.value());
            if (key.contains(Hs::widgetParam)) {
                preferences.insert(key.remove(Hs::widgetParam),value);
            }
        }
        params[Hs::preferences] = preferences;
        params[Hs::homescreenData] = mToken;

        success = contentService.createWidget(params);
        if (!success) {
            success = addShortcut(contentService);
        }
    } else {
            success = addShortcut(contentService);
    }
    emit exit();
    if (mMenuMode == Hs::AddHsMenuMode) {
        machine()->postEvent(
            HsMenuEventFactory::createOpenHomeScreenEvent());
    }
    return success;
}

/*!
 Provides debug logging of add widget/shortcut action
 \param operation name: results of which operation is logged
 \param entryId: menu entry id that was added to home screen
 \param activePage: active page of home screen
 \param operationSucceded: true always and only when
 logged operation succeded
 \retval void
 */
void HsAddToHomeScreenState::logActionResult(QString operationName,
        int entryId, bool operationSucceded)
{
    const QString
    message =
        QString("\t%1 %2 item ID: %3, active page index: %4") .arg(
            operationName) .arg(operationSucceded ? "succeded"
                                : "failed") .arg(entryId);

    if (operationSucceded) {
        qDebug() << message;
    } else {
        qWarning() << message;
    }
}




