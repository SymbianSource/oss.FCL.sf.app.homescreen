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
#include <hbmessagebox.h>
#include <qstatemachine.h>
#include <hbaction.h>
#include <HbDocumentLoader>
#include <hbdialog.h>
#include <HbIcon>
#include <HbLabel>
#include <hbwidget.h>
#include <hbscrollarea.h>
#include <hbscrollbar.h>
#include <QtAlgorithms>
#include <hbnotificationdialog.h>

#include "hsmenueventfactory.h"
#include "hsmenuservice.h"
#include "hswidgethost.h"
#include "hspreviewhswidgetstate.h"
#include "hsmenuevent.h"
#include "hsmenudialogfactory.h"
#include "hsapp_defs.h"
#include "hsscene.h"
#include "hspage.h"


#include "canotifier.h"
#include "canotifierfilter.h"
#include "caservice.h"
#include "hsdomainmodel_global.h"
#include <hscontentservice.h>


/*!
 \class HsPreviewHSWidgetState
 \ingroup group_hsworkerstateplugin
 \brief Application Library State.
 Parent state for Application Library functionality (browsing applications and collections)
 \see StateMachine
 \lib ?library
 */

/*!
 Constructor
 \param parent Parent state.
 \retval void
 */
HsPreviewHSWidgetState::HsPreviewHSWidgetState(QState *parent) :
    QState(parent),
    mPreviewDialog(0),
    mEntryId(0),
    mCorruptedMessage(0),
    mConfirmRemovalAction(0),
    mAddToHomescreenAction(0),
    mToken(),
    mUri()
{
    setObjectName("/HsPreviewHSWidgetState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Destructor.
 */
HsPreviewHSWidgetState::~HsPreviewHSWidgetState()
{
    cleanUp(); // in case of throw
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
void HsPreviewHSWidgetState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsPreviewHSWidgetState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
   
    QVariantMap data = menuEvent->data();
    mEntryId = data.value(itemIdKey()).toInt();
    mToken = data.value(HOMESCREENDATA);
    
        
    QSharedPointer<CaEntry> entry =
        CaService::instance()->getEntry(mEntryId);
    mUri = entry->attribute(widgetUriAttributeName());
    mPreviewDialog = buildPreviewDialog(*entry);
    mAddToHomescreenAction = mPreviewDialog->actions().value(0);
    
    if (mPreviewDialog != NULL) {
        // Launch popup asyncronously
        
        mEntryObserver.reset(
            new HsMenuEntryRemovedHandler(mEntryId, this, SIGNAL(exit())));
        
        mPreviewDialog->open(this, SLOT(previewDialogFinished(HbAction*)));
    }

    HSMENUTEST_FUNC_EXIT("HsPreviewHSWidgetState::onEntry");
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsPreviewHSWidgetState::cleanUp()
{
    if (mPreviewDialog != NULL) {
        mPreviewDialog->close();
        mPreviewDialog = NULL;
    }
    
    if (mCorruptedMessage != NULL) {
        mCorruptedMessage->close();
        mCorruptedMessage = NULL;
    }
    mEntryObserver.reset(0);
    mToken = NULL;
}

/*!
 Slot launched on dismissing the preview dialog
 \retval void
 */
void HsPreviewHSWidgetState::previewDialogFinished(HbAction* finishedAction)
{
    mPreviewDialog = NULL;

    if (finishedAction == mAddToHomescreenAction) {

        QVariantHash widgetData;
        widgetData[URI] = mUri;
        widgetData[HOMESCREENDATA] = mToken;
        
        bool success = HsContentService::instance()->createWidget(widgetData);
        
        if (success) {
            HbNotificationDialog *notificationDialog = new HbNotificationDialog();
            notificationDialog->setAttribute(Qt::WA_DeleteOnClose);
            notificationDialog->setTitle(hbTrId("txt_applib_dpophead_added_to_homescreen"));
            notificationDialog->show();
            emit exit();
        }
        else {
            showMessageWidgetCorrupted();            
        }
    } else {
        emit exit();
    }
    mAddToHomescreenAction = 0;
}

/*!
 Shows message about corrupted widget library. Deletes widget eventually
 \retval void
 */
void HsPreviewHSWidgetState::showMessageWidgetCorrupted()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::showMessageWidgetCorrupted");

    mCorruptedMessage = HsMenuDialogFactory().create(
            hbTrId("txt_applib_dialog_file_corrupted_unable_to_use_wi"));

    mConfirmRemovalAction = mCorruptedMessage->actions().value(0);
    mCorruptedMessage
        ->open(this, SLOT(messageWidgetCorruptedFinished(HbAction*)));

    HSMENUTEST_FUNC_EXIT("HsCollectionState::showMessageWidgetCorrupted");
}

/*!
 Slot launched on dismissing the corrupted widget error note
 \retval void
 */
void HsPreviewHSWidgetState::messageWidgetCorruptedFinished(HbAction* finishedAction)
{
    mCorruptedMessage = NULL;
	
    if (static_cast<QAction*>(finishedAction) == mConfirmRemovalAction) {
        HsMenuService::executeAction(mEntryId, removeActionIdentifier());
    }
    emit exit();
    
    mConfirmRemovalAction = NULL;
}

/*!
 \param entry The entry which is to be presented by the preview dialog.
 \return Preview popup for a given entry on success, NULL otherwise. 
  Ownership is passed to the caller.
 */
HbDialog* HsPreviewHSWidgetState::buildPreviewDialog(const CaEntry& entry) const
{
    HbDocumentLoader loader;

    bool loadStatusOk = false;
    loader.load(HS_WIDGET_PREVIEW_DIALOG_LAYOUT, &loadStatusOk);
    
    HbDialog *const previewDialog = 
        qobject_cast<HbDialog*>(
            loader.findWidget(HS_WIDGET_PREVIEW_DIALOG_NAME));
    HbLabel *const headingLabel =
        qobject_cast<HbLabel*>(
            loader.findWidget(HS_WIDGET_PREVIEW_LABEL_NAME));
    HbLabel *const iconBox =
        qobject_cast<HbLabel*>(
            loader.findWidget(HS_WIDGET_PREVIEW_ICON_BOX_NAME));

    loadStatusOk = loadStatusOk && (previewDialog != NULL)
        && (headingLabel != NULL) && (iconBox != NULL);
    
    Q_ASSERT_X(loadStatusOk,
        HS_WIDGET_PREVIEW_DIALOG_LAYOUT,
           "Cannot initialize widgets based on docml file.");
    
    if (loadStatusOk) {
        previewDialog->actions().value(0)->setParent(previewDialog);
        previewDialog->actions().value(1)->setParent(previewDialog);
        
        previewDialog->setTimeout(HbPopup::NoTimeout);
        previewDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        
        headingLabel->setPlainText(entry.text());
        
        const QString previewImageName(entry.attribute(
            QLatin1String(HS_PREVIEW_ATTRIBUTE_NAME))); 
        
        if (!previewImageName.isEmpty()) {
            const HbIcon previewImage(previewImageName);
            if (previewImage.size().isValid()) {
                iconBox->setMinimumHeight(previewImage.height());
                iconBox->setMaximumHeight(previewImage.height());
                iconBox->setIcon(previewImage);
            }
        }
    }
    
    return previewDialog;
}

