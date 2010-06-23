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
 * Description: Menu delete collection item state
 *
 */

#include <hbmessagebox.h>
#include <hbaction.h>
#include <hsmenuservice.h>
#include <hsshortcutservice.h>
#include <HbDocumentLoader>
#include <HbLabel>
#include <HbListView>
#include <HbParameterLengthLimiter>


#include <QStandardItemModel>
#include <QStandardItem>
#include <casoftwareregistry.h>
#include <caquery.h>
#include <canotifier.h>

#include "hsuninstallitemstate.h"
#include "hsmenuevent.h"

/*!
 \class HsUninstallItemState
 \ingroup group_hsworkerstateplugin
 \brief Uninstall Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Parent state.
 */
HsUninstallItemState::HsUninstallItemState(QState *parent) :
    QState(parent),
    mItemId(0),
    mNotifier(NULL),
    mUninstallMessage(NULL),
    mUninstallJavaMessage(NULL),
    mConfirmAction(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsUninstallItemState::~HsUninstallItemState()
{
    cleanUp(); // in case of throw
}

/*!
 Constructs contained objects.
 */
void HsUninstallItemState::construct()
{
    setObjectName("/UninstallItemState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsUninstallItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsUninstallItemState::onEntry");
    QState::onEntry(event);
    qDebug("HsUninstallItemState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mItemId = data.value(itemIdKey()).toInt();
    QSharedPointer<CaEntry> entry = CaService::instance()->getEntry(mItemId);
    QString appType = entry->attribute(swTypeKey());
    if (!appType.compare(javaSwType())) {
        // java
        createUninstallJavaMessage();
    } else {
        // other
        createUninstallMessage();
    }
    
    subscribeForMemoryCardRemove();

    HSMENUTEST_FUNC_EXIT("HsUninstallItemState::onEntry");
}

/*!
 Action after closed confirmation dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsUninstallItemState::uninstallMessageFinished(HbAction* finishedAction)
{
    if (finishedAction == mConfirmAction) {
        HsMenuService::executeAction(mItemId, removeActionIdentifier());
    }
    emit exit();
}

/*!
 Return information about a component: component name, a list of names
 of applications in this component and a delete message.
 \param[out] componentName component name.
 \param[out] applicationsNames a list of names of applications.
 \param[out] confirmationMessage delete message.
 \retval true if there is no error.
 */
bool HsUninstallItemState::getApplicationsNames(QString &componentName,
    QStringList &applicationsNames,
    QString &confirmationMessage)
{
    componentName.clear();
    applicationsNames.clear();
    confirmationMessage.clear();
    
    QSharedPointer<CaService> service = CaService::instance();
    QSharedPointer<CaEntry> entry = service->getEntry(mItemId);
    const int componentId =
        entry->attribute(componentIdAttributeName()).toInt();
    
    QSharedPointer<CaSoftwareRegistry> softwareRegistry =
        CaSoftwareRegistry::create();
    QStringList appUids;
    bool retval = softwareRegistry->getUninstallDetails(componentId,
        componentName,
        appUids,
        confirmationMessage);
    if (retval) {
        CaQuery query;
        foreach (QString uid, appUids) {
            query.setAttribute(applicationUidEntryKey(), uid);
            QList< QSharedPointer<CaEntry> > entries =
                service->getEntries(query);
            applicationsNames << entries[0]->text();
        }
        if (applicationsNames.length()==1
            && applicationsNames[0]==componentName) {
            applicationsNames.clear();
        }
    }
    return retval;
}

/*!
 Subscribe for memory card remove.
 */
void HsUninstallItemState::subscribeForMemoryCardRemove()
{
    CaNotifierFilter filter;
    filter.setIds(QList<int>() << mItemId);
    mNotifier = CaService::instance()->createNotifier(filter);
    mNotifier->setParent(this);
    connect(mNotifier,
        SIGNAL(entryChanged(CaEntry,ChangeType)),
        SIGNAL(exit()));
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsUninstallItemState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mUninstallMessage) {
        disconnect(mUninstallMessage, SIGNAL(finished(HbAction*)),
            this, SLOT(uninstallMessageFinished(HbAction*)));
        mUninstallMessage->close();
        mUninstallMessage = NULL;
    }
    
    if (mUninstallJavaMessage) {
        disconnect(mUninstallJavaMessage, SIGNAL(finished(HbAction*)), this, SLOT(uninstallMessageFinished(HbAction*)));
        mUninstallJavaMessage->close();
        mUninstallJavaMessage = NULL;
    }

    delete mNotifier;
    mNotifier = NULL;
    mConfirmAction = NULL;
    mItemId = 0;
}

/*!
 Method create uninstall confirmation message.
 \retval void
 */
void HsUninstallItemState::createUninstallMessage()
{
    QString message;
    if (HsShortcutService::instance()->isItemShortcutWidget(mItemId)) {
        message.append(
            HbParameterLengthLimiter("txt_applib_dialog_uninstalls_1_and_deletes_all_sh").arg(
                HsMenuService::getName(mItemId)));
    } else {
        message.append(
            HbParameterLengthLimiter("txt_applib_dialog_1_will_be_removed_from_phone_c").arg(
                HsMenuService::getName(mItemId)));
    }

    // create and show message box
    mUninstallMessage = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    mUninstallMessage->setAttribute(Qt::WA_DeleteOnClose);

    mUninstallMessage->setText(message);

    mUninstallMessage->clearActions();
    mConfirmAction = new HbAction(hbTrId("txt_common_button_ok"),
            mUninstallMessage);
    mUninstallMessage->addAction(mConfirmAction);

    HbAction *secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"),
            mUninstallMessage);
    mUninstallMessage->addAction(secondaryAction);

    mUninstallMessage->open(this, SLOT(uninstallMessageFinished(HbAction*)));
}

/*!
 Method create uninstall confirmation dialog for java.
 \retval void
 */
void HsUninstallItemState::createUninstallJavaMessage()
{
    HbDocumentLoader loader;
    bool loadStatusOk = false;
    mObjectList = loader.load(HS_UNINSTALL_DIALOG_LAYOUT, &loadStatusOk);
    
    Q_ASSERT_X(loadStatusOk,
            HS_UNINSTALL_DIALOG_LAYOUT,
           "Error while loading docml file.");

    QString componentName;
    QStringList applicationsNames;
    QString detailsMessage;
    getApplicationsNames(componentName, applicationsNames, detailsMessage);
    
    QString section;
    if (applicationsNames.isEmpty() && detailsMessage.isEmpty()) {
        mDialogType = UninstallDialogDefinition01;
        section = QString("uninstallDialogDefinition01");
        mObjectList = loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
    } else if ( (!applicationsNames.isEmpty()) && (!detailsMessage.isEmpty())) {
        mDialogType = UninstallDialogDefinition02;
        section = QString("uninstallDialogDefinition02");
        mObjectList = loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
    } else if ( (!applicationsNames.isEmpty() && detailsMessage.isEmpty())) {
        mDialogType = UninstallDialogDefinition03;
        section = QString("uninstallDialogDefinition03");
        mObjectList = loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
    } else if (applicationsNames.isEmpty()  && (!detailsMessage.isEmpty())) {
        mDialogType = UninstallDialogDefinition04;
        section = QString("uninstallDialogDefinition04");
        mObjectList = loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
    }
 
    mUninstallJavaMessage = 
        qobject_cast<HbDialog*>(
            loader.findWidget(HS_UNINSTALL_DIALOG_NAME));
    
    HbLabel* iconLabel = 
        qobject_cast<HbLabel*>(
            loader.findWidget(HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_ICON));
    QSharedPointer<CaEntry> entry2 = CaService::instance()->getEntry(mItemId);
    HbIcon icon = entry2->makeIcon();
    iconLabel->setIcon(icon);
    
    HbLabel* textLabel = 
        qobject_cast<HbLabel*>(
            loader.findWidget(HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_LABEL));
    textLabel->setPlainText(textLabel->plainText().arg(componentName));
    
    HbLabel* detailsUninstalLabel;
    if ((mDialogType == UninstallDialogDefinition02) ||
        (mDialogType == UninstallDialogDefinition04)) {
        detailsUninstalLabel= qobject_cast<HbLabel*>(
                loader.findWidget(HS_UNINSTALL_DIALOG_DELETE_LABEL));
        detailsUninstalLabel->setPlainText(detailsMessage);
    }

    HbLabel* listView;
    QString applicationsList;
    QString newLine("\n");
    if ((mDialogType == UninstallDialogDefinition02) ||
        (mDialogType == UninstallDialogDefinition03)) {
        listView = qobject_cast<HbLabel*>(
                loader.findWidget(HS_UNINSTALL_DIALOG_LIST_VIEW_LABEL));

        for (int i=0; i<applicationsNames.count(); i++) {
            applicationsList = applicationsList + applicationsNames[i] + newLine;
        }
        listView->setPlainText(applicationsList);
    }

    mUninstallJavaMessage->setAttribute(Qt::WA_DeleteOnClose);
    mUninstallJavaMessage->setTimeout(HbPopup::NoTimeout);
    mConfirmAction = qobject_cast<HbAction*>(loader.findObject(
            HS_UNINSTALL_DIALOG_CONFIRMATION_ACTION));
    
    mUninstallJavaMessage->open(this, SLOT(uninstallMessageFinished(HbAction*)));
}
