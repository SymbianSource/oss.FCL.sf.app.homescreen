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


#include <hblabel.h>
#include <hsiwidgetprovider.h>
#include <hsshortcutservice.h>
#include <hscontentservice.h>
#include <hbmessagebox.h>

#include "hsaddtohomescreenstate.h"
#include "hsmenuevent.h"
#include "hsapp_defs.h"
#include "caservice.h"
#include "caquery.h"
#include "hsmenuservice.h"

const char SHORTCUT_WIDGET_URI[] = "hsshortcutwidgetplugin";
const char SHORTCUT_ID[] = "mcsId";

/*!
 \class HsAddToHomeScreenState
 \ingroup group_hsmenustateplugin
 \brief Application Library State.
 Parent state for Application Library functionality
 (browsing applications and collections)
 */

/*!
 Constructor
 \param parent: parent state
 \retval void
 */
HsAddToHomeScreenState::HsAddToHomeScreenState(QState *parent) :
    HsMenuBaseState("AddToHomeScreenState", parent)
{
    requestServices(QList<QVariant> () << SHORTCUT_SERVICE_KEY
                    << CONTENT_SERVICE_KEY);
}

/*!
 Destructor. Empty one.
 */
HsAddToHomeScreenState::~HsAddToHomeScreenState()
{
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
    HsMenuBaseState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    const QString entryTypeName = data.value(entryTypeNameKey()).toString();

    const int entryId = data.value(itemIdKey()).toInt();

    if (entryTypeName == widgetTypeName()) {
        const QString library =
            data.value(widgetLibraryAttributeName()).toString();

        const QString uri = data.value(widgetUriAttributeName()).toString();

        addWidget(*contentService(), library, uri, entryId);
        HsMenuService::touch(entryId);
    } else {
        addShortcut(*contentService(), entryId);
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
 \retval void
 */
void HsAddToHomeScreenState::addWidget(HsContentService &contentService,
                                       const QString &library, const QString &uri, int entryId)
{
    HSMENUTEST_FUNC_ENTRY("HsAddToHomeScreenState::addWidget");
    QVariantMap params;
    params["library"] = library;
    params["uri"] = uri;
    bool ok = contentService.createWidget(params);
    if (!ok) {
        showMessageWidgetCorrupted(entryId);
    }
    logActionResult("Adding widget", entryId, ok);
    HSMENUTEST_FUNC_EXIT("HsAddToHomeScreenState::addWidget");
}

/*!
 Shows message about corrupted widget library. Deletes widget eventually
 \param itemId entryId of widget (needed to delete it)
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAddToHomeScreenState::showMessageWidgetCorrupted(int itemId)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::showMessageWidgetCorrupted");
    QString message(hbTrId(
                        "txt_applib_dialog_file_corrupted_unable_to_use_wi"));
    if (HbMessageBox::question(message, hbTrId(
                                   "txt_common_button_ok"), hbTrId("txt_common_button_cancel"))) {
        HsMenuService::executeAction(itemId, removeActionIdentifier());
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionState::showMessageWidgetCorrupted");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Adds a shortcut to active page of home screen
 \param shortcutService: service for adding shortcuts
 \param entryId: menu entry id
 \param activePage: active page of home screen
 \retval void
 */
void HsAddToHomeScreenState::addShortcut(HsContentService &contentService,
        int entryId)
{
    //ShortcutData item_id;
    //item_id.mShortcutId = entryId;

    HSMENUTEST_FUNC_ENTRY("HsAddToHomeScreenState::addShortcut");
    CaQuery query;
    query.setEntryTypeNames(QStringList(widgetTypeName()));
    QList<CaEntry *> entries = CaService::instance()->getEntries(query);

    if (mLibraryPath.isEmpty()) {
        foreach(CaEntry *entry,entries) {
            QString uri = entry->attribute(widgetUriAttributeName());
            if (uri == SHORTCUT_WIDGET_URI) {
                mLibraryPath = entry->attribute(
                                   widgetLibraryAttributeName());
                break;
            }
        }
    }
    if (!mLibraryPath.isEmpty()) {
        QVariantMap params;
        params["library"] = mLibraryPath;
        params["uri"] = SHORTCUT_WIDGET_URI;
        QVariantMap preferences;
        preferences[SHORTCUT_ID] = QString::number(entryId);
        params["preferences"] = preferences;
        const bool result = contentService.createWidget(params);
        logActionResult("Adding shortcut", entryId, result);
    }
    HSMENUTEST_FUNC_EXIT("HsAddToHomeScreenState::addShortcut");
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
