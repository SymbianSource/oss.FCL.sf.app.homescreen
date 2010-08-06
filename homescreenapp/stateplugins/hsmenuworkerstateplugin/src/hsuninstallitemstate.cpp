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

#include <QScopedPointer>
#include <QStandardItemModel>
#include <QStandardItem>
#include <casoftwareregistry.h>
#include <caquery.h>
#include <canotifier.h>

#include "hsuninstallitemstate.h"
#include "hsmenuevent.h"
#include "hsmenudialogfactory.h"
#include "hsmenuentryremovedhandler.h"
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
        
    HSMENUTEST_FUNC_EXIT("HsUninstallItemState::onEntry");
}

/*!
 Action after closed confirmation dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsUninstallItemState::uninstallMessageFinished(HbAction* finishedAction)
{
    if (static_cast<QAction*>(finishedAction) == mConfirmAction) {
        HsMenuService::executeAction(mItemId, removeActionIdentifier());
    }
    mConfirmAction = NULL;
    mUninstallMessage = NULL;
	mUninstallJavaMessage = NULL;
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
            if (entries.length() > 0) {
                applicationsNames << entries[0]->text();
            }
        }
        if (applicationsNames.length()==1
            && applicationsNames[0]==componentName) {
            applicationsNames.clear();
        }
    }
    return retval;
}


/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsUninstallItemState::cleanUp()
{
    // Close messagebox if App key was pressed
    if (mUninstallMessage != NULL) {
        mUninstallMessage->close();
        mUninstallMessage = NULL;
    }
    
    if (mUninstallJavaMessage != NULL) {
        mUninstallJavaMessage->close();
        mUninstallJavaMessage = NULL;
    }

    mConfirmAction = NULL;
    mItemId = 0;
}

/*!
 Method create uninstall confirmation message.
 \retval void
 */
void HsUninstallItemState::createUninstallMessage()
{
    CaQuery parentsQuery;
    parentsQuery.setChildId(mItemId);
    parentsQuery.setEntryTypeNames(QStringList(collectionTypeName()));
    QList<int> parentsIds = CaService::instance()->getEntryIds(
            parentsQuery);
    QString message;
    if (HsShortcutService::instance()->isItemShortcutWidget(mItemId) || (parentsIds.count() > 0)) {
        message.append(
            HbParameterLengthLimiter("txt_applib_dialog_uninstalls_1_and_deletes_all_sh").arg(
                HsMenuService::getName(mItemId)));
    } else {
        message.append(
            HbParameterLengthLimiter("txt_applib_dialog_1_will_be_removed_from_phone_c").arg(
                HsMenuService::getName(mItemId)));
    }

    // create and show message box
    mUninstallMessage = HsMenuDialogFactory().create(message);
    mConfirmAction = mUninstallMessage->actions().value(0);
    
    QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
        new HsMenuEntryRemovedHandler(mItemId, this, SIGNAL(exit())));
    entryObserver.take()->setParent(mUninstallMessage);
    
    mUninstallMessage->open(this, SLOT(uninstallMessageFinished(HbAction*)));
}

/*!
 Method create uninstall confirmation message.
 Used for java applications without deletion confirmation
 and with midlet name equal to app name
 \retval void
 */
void HsUninstallItemState::createSimpleUninstallJavaMessage()
{
    QString message;
    message.append(
        HbParameterLengthLimiter("txt_applib_dialog_1_will_be_removed_from_phone_c").arg(
            HsMenuService::getName(mItemId)));

    // create and show message box
    mUninstallMessage = HsMenuDialogFactory().create(message);
    
    QSharedPointer<CaEntry> entry = CaService::instance()->getEntry(mItemId);
    HbIcon icon = entry->makeIcon();
    mUninstallMessage->setIcon(icon);
    mConfirmAction = mUninstallMessage->actions().value(0);

    QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
        new HsMenuEntryRemovedHandler(mItemId, this, SIGNAL(exit())));
    entryObserver.take()->setParent(mUninstallMessage);

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
    loader.load(HS_UNINSTALL_DIALOG_LAYOUT, &loadStatusOk);
    
    Q_ASSERT_X(loadStatusOk,
            HS_UNINSTALL_DIALOG_LAYOUT,
           "Error while loading docml file.");

    QString componentName;
    QStringList applicationsNames;
    QString detailsMessage;
    getApplicationsNames(componentName, applicationsNames, detailsMessage);
    
    QString section;
    if (applicationsNames.isEmpty() && detailsMessage.isEmpty()) {        
        createSimpleUninstallJavaMessage();        
    }
    else {
        if ( (!applicationsNames.isEmpty()) && (!detailsMessage.isEmpty())) {
            mDialogType = UninstallDialogDefinition02;
            section = QString("uninstallDialogDefinition02");
            loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
        }
        else if ( (!applicationsNames.isEmpty() && detailsMessage.isEmpty())) {
            mDialogType = UninstallDialogDefinition03;
            section = QString("uninstallDialogDefinition03");
            loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
        }
        else if (applicationsNames.isEmpty() && (!detailsMessage.isEmpty())) {
            mDialogType = UninstallDialogDefinition04;
            section = QString("uninstallDialogDefinition04");
            loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
        }

        mUninstallJavaMessage = qobject_cast<HbDialog*>(
            loader.findWidget(HS_UNINSTALL_DIALOG_NAME));

        if (mUninstallJavaMessage) {
            mUninstallJavaMessage->setAttribute(Qt::WA_DeleteOnClose);
            mUninstallJavaMessage->setTimeout(HbPopup::NoTimeout);
            mUninstallJavaMessage->actions()[0]->setParent(mUninstallJavaMessage);
            mUninstallJavaMessage->actions()[1]->setParent(mUninstallJavaMessage);

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

            mConfirmAction = qobject_cast<HbAction*>(loader.findObject(
                    HS_UNINSTALL_DIALOG_CONFIRMATION_ACTION));
        
            QScopedPointer<HsMenuEntryRemovedHandler> entryObserver(
                new HsMenuEntryRemovedHandler(mItemId, this, SIGNAL(exit())));
            entryObserver.take()->setParent(mUninstallJavaMessage);
            
            mUninstallJavaMessage->open(this, SLOT(uninstallMessageFinished(HbAction*)));
        }
    }
}
