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
//#include <hsiwidgetprovider.h>
#include <hbmessagebox.h>
#include <qstatemachine.h>
#include <hbaction.h>
#include <HbDocumentLoader>
#include <hbdialog.h>
#include <hbwidget.h>
#include <hbscrollarea.h>
#include <hbscrollbar.h>
#include <QtAlgorithms>

#include "hsmenueventfactory.h"
#include "hsmenuservice.h"
#include "hswidgethost.h"
#include "hspreviewhswidgetstate.h"
#include "hsmenuevent.h"
#include "hsapp_defs.h"
#include "hsscene.h"
#include "hspage.h"

#include "canotifier.h"
#include "canotifierfilter.h"
#include "caservice.h"

#include "hsdomainmodel_global.h"
#include <hscontentservice.h>


const char HS_PREVIEW_HS_WIDGET_STATE[] = "HsPreviewHSWidgetState";

/*!
 Constructor
 \param parent: parent state
 \retval void
 */
HsPreviewHSWidgetState::HsPreviewHSWidgetState(QState *parent) :
    HsMenuBaseState(HS_PREVIEW_HS_WIDGET_STATE, parent),
    mPopupDialog(0),
    mNotifier(0),
    mScrollArea(0),
    mWidget(0),
    mEntryId(0)
{
    requestServices(QList<QVariant> () << CONTENT_SERVICE_KEY);
}

/*!
 Destructor.
 */
HsPreviewHSWidgetState::~HsPreviewHSWidgetState()
{
    if (mNotifier) {
        delete mNotifier;
    }
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
 \param shortcutService: service for adding shortcuts
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsPreviewHSWidgetState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsPreviewHSWidgetState::onEntry");
    HsMenuBaseState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mEntryId = data.value(itemIdKey()).toInt();

    QVariantHash widgetData;
    widgetData.insert(LIBRARY, data.value(widgetLibraryAttributeName()).toString());
    widgetData.insert(URI, data.value(widgetUriAttributeName()).toString());

    QScopedPointer<HsWidgetHost> widget(contentService()->createWidgetForPreview(widgetData));

    if (widget) {
        widget->setMinimumSize(widget->preferredWidth(),widget->preferredHeight());

        HbDocumentLoader loader;

        bool loadStatusOk = false;
        mObjectList = 
            loader.load(HS_WIDGET_PREVIEW_DIALOG_LAYOUT, &loadStatusOk);
        Q_ASSERT_X(loadStatusOk,
            HS_WIDGET_PREVIEW_DIALOG_LAYOUT,
               "Error while loading docml file.");

        mPopupDialog = 
            qobject_cast<HbDialog*>(
                loader.findWidget(HS_WIDGET_PREVIEW_DIALOG_NAME));

        mScrollArea =
            qobject_cast<HbScrollArea*>(
                loader.findWidget(HS_WIDGET_PREVIEW_SCROLL_AREA_NAME));

        // set parent to actions to delete them together with dialog
        mPopupDialog->primaryAction()->setParent(mPopupDialog);
        mPopupDialog->secondaryAction()->setParent(mPopupDialog);

        if (mPopupDialog != NULL && mScrollArea != NULL) {
            mPopupDialog->setContentWidget(mScrollArea); // ownership transferred
            mPopupDialog->setTimeout(HbPopup::NoTimeout);
            mPopupDialog->setAttribute(Qt::WA_DeleteOnClose, true);
            mScrollArea->verticalScrollBar()->setInteractive(true);
            mScrollArea->horizontalScrollBar()->setInteractive(true);
            mWidget = widget.take(); // ownership transfered
            mScrollArea->setContentWidget(mWidget); // ownership transferred
            
            subscribeForMemoryCardRemove();
            mWidget->initializeWidget();
            mWidget->showWidget();
            // Launch popup asyncronously
            mPopupDialog->open(this, SLOT(previewDialogFinished(HbAction*)));    
        }

    } else {
        showMessageWidgetCorrupted();
    }

    HSMENUTEST_FUNC_EXIT("HsPreviewHSWidgetState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


/*!
 Memory card with instaled widget was removed.
 \retval void
 */
void HsPreviewHSWidgetState::memoryCardRemoved()
{
    if (mPopupDialog) {
        mPopupDialog->close();
    }
}

/*!
 Slot launched on dismissing the preview dialog
 \retval void
 */
void HsPreviewHSWidgetState::previewDialogFinished(HbAction* finishedAction)
{
    if (mPopupDialog != NULL) { 
        // (work-around for crash if more then one action is selected in HbDialog)
        disconnect(mNotifier,
                   SIGNAL(entryChanged(CaEntry,ChangeType)),
                   this, SLOT(memoryCardRemoved()));

        if (finishedAction == mPopupDialog->primaryAction()) {
            mWidget->hideWidget();
            mScrollArea->takeContentWidget();
            HsScene::instance()->activePage()->addNewWidget(
                mWidget); // ownership transferred
        } else {
            mWidget->uninitializeWidget();
            mWidget->deleteFromDatabase();
        }
        mPopupDialog = NULL;
        mScrollArea = NULL;
        mWidget = NULL;

        delete mNotifier;
        mNotifier = NULL;

    } else {
        // (work-around for crash if more then one action is selected in HbDialog)
        qWarning("Another signal finished was emited.");
        }
}



/*!
 Subscribe for memory card remove.
 \retval void
 */
void HsPreviewHSWidgetState::subscribeForMemoryCardRemove()
{
    CaNotifierFilter filter;
    QList<int> entryIds;
    entryIds.append(mEntryId);
    filter.setIds(entryIds);
    mNotifier = CaService::instance()->createNotifier(filter);
    mNotifier->setParent(this);
    connect(mNotifier,
            SIGNAL(entryChanged(CaEntry,ChangeType)),
            SLOT(memoryCardRemoved()));
}


/*!
 Shows message about corrupted widget library. Deletes widget eventually
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsPreviewHSWidgetState::showMessageWidgetCorrupted()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::showMessageWidgetCorrupted");

    QString message(hbTrId("txt_applib_dialog_file_corrupted_unable_to_use_wi"));
    HbMessageBox::question(message, this,
                SLOT(messageWidgetCorruptedFinished(HbAction*)),
                hbTrId("txt_common_button_ok"), hbTrId("txt_common_button_cancel"));

    HSMENUTEST_FUNC_EXIT("HsCollectionState::showMessageWidgetCorrupted");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


/*!
 Slot launched on dismissing the corrupted widget error note
 \retval void
 */
void HsPreviewHSWidgetState::messageWidgetCorruptedFinished(HbAction* finishedAction)
{
    if (finishedAction && qobject_cast<HbMessageBox*>(finishedAction->parent())->primaryAction() == finishedAction) {
        HsMenuService::executeAction(mEntryId, removeActionIdentifier());
    }
}