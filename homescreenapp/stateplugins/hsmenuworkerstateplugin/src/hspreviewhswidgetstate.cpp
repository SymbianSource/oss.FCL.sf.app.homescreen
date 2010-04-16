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
    mNotifier(0)
{
    requestServices(QList<QVariant> () << CONTENT_SERVICE_KEY);
    connect(this, SIGNAL(exited()),SLOT(stateExited()));
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

    const int entryId = data.value(itemIdKey()).toInt();

    QVariantHash widgetData;
    widgetData.insert(LIBRARY, data.value(widgetLibraryAttributeName()).toString());
    widgetData.insert(URI, data.value(widgetUriAttributeName()).toString());
    QScopedPointer<HsWidgetHost> widget(
        contentService()->createWidgetForPreview(widgetData));

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
        
        HbScrollArea *const scrollArea( 
            qobject_cast<HbScrollArea*>(
                loader.findWidget(HS_WIDGET_PREVIEW_SCROLL_AREA_NAME)));
        
        if (mPopupDialog != NULL && scrollArea != NULL) {
            mPopupDialog->setContentWidget(scrollArea); //ownership transferred
            mPopupDialog->setTimeout(HbPopup::NoTimeout);
            scrollArea->verticalScrollBar()->setInteractive(true);
            scrollArea->horizontalScrollBar()->setInteractive(true);
            scrollArea->setContentWidget(widget.data());
            scrollArea->takeContentWidget();
            
            subscribeForMemoryCardRemove(entryId);
            widget->initializeWidget();
            widget->showWidget();
            // Launch popup syncronously
            const HbAction *const action(mPopupDialog->exec());
    
            disconnect(mNotifier,
                       SIGNAL(entryChanged(CaEntry,ChangeType)),
                       this, SLOT(memoryCardRemoved()));
    
            if (action == mPopupDialog->primaryAction()) {
                widget->hideWidget();
                HsScene::instance()->activePage()->addNewWidget(
                    widget.take()); // ownership transferred
            } else {
                widget->uninitializeWidget();
                widget->deleteFromDatabase();
            }
        }

    } else {
        showMessageWidgetCorrupted(entryId);
    }

    HSMENUTEST_FUNC_EXIT("HsPreviewHSWidgetState::onEntry");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 State exited.
 \retval void
 */
void HsPreviewHSWidgetState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsPreviewHSWidgetState::stateExited");


    delete mNotifier;
    mNotifier = NULL;

    qDeleteAll(mObjectList);
    mObjectList.clear();
    
    mPopupDialog = NULL;

    HSMENUTEST_FUNC_EXIT("HsPreviewHSWidgetState::stateExited");
    qDebug("HsPreviewHSWidgetState::stateExited()");
}

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
 Subscribe for memory card remove.
 \param entryId: HSWidget id.
 \retval void
 */
void HsPreviewHSWidgetState::subscribeForMemoryCardRemove(int entryId)
{
    CaNotifierFilter filter;
    QList<int> entryIds;
    entryIds.append(entryId);
    filter.setIds(entryIds);
    mNotifier = CaService::instance()->createNotifier(filter);
    mNotifier->setParent(this);
    connect(mNotifier,
            SIGNAL(entryChanged(CaEntry,ChangeType)),
            SLOT(memoryCardRemoved()));
}


/*!
 Shows message about corrupted widget library. Deletes widget eventually
 \param itemId entryId of widget (needed to delete it)
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsPreviewHSWidgetState::showMessageWidgetCorrupted(int itemId)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::showMessageWidgetCorrupted");
    QString message(hbTrId("txt_applib_dialog_file_corrupted_unable_to_use_wi"));
    if (HbMessageBox::question(message,hbTrId(
                                   "txt_common_button_ok"), hbTrId("txt_common_button_cancel"))) {
        HsMenuService::executeAction(itemId, removeActionIdentifier());
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionState::showMessageWidgetCorrupted");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

