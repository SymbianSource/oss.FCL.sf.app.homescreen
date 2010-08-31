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

#include <HbDocumentLoader>
#include <HbLabel>
#include <HbMessageBox>
#include <QAction>
#include <hsmenuservice.h>
#include <casoftwareregistry.h>

#include "hsinstallationlogstate.h"

/*!
 \class HsInstallationLogState
 \ingroup group_hsworkerstateplugin
 \brief Uninstall Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Parent state.
 */
HsInstallationLogState::HsInstallationLogState(QState *parent) :
    QState(parent),
    mInstalationLogDialog(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsInstallationLogState::~HsInstallationLogState()
{
    cleanUp(); // in case of throw
}

/*!
 Constructs contained objects.
 */
void HsInstallationLogState::construct()
{
    setObjectName("/InstallationLogState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsInstallationLogState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsInstallationLogState::onEntry");
    QState::onEntry(event);
    qDebug("HsInstallationLogState::onEntry()");
    
    createInstallationLogDialog();

    HSMENUTEST_FUNC_EXIT("HsInstallationLogState::onEntry");
}

/*!
 Invoked on exiting state
 */
void HsInstallationLogState::onExit(QEvent *event)
{
    QState::onExit(event);
}

/*!
 Create installation log dialog.
 \retval void
 */
void HsInstallationLogState::createInstallationLogDialog()
{
    QList<CaSoftwareRegistry::DetailMap> detailsList =
        CaSoftwareRegistry::create()->retrieveLogEntries();
    
    HbDocumentLoader loader;
    bool loadStatusOk = false;
    loader.load(HS_INSTALLATION_LOG_DIALOG_LAYOUT, &loadStatusOk);
    Q_ASSERT_X(loadStatusOk, HS_INSTALLATION_LOG_DIALOG_LAYOUT,
           "Error while loading docml file.");

    mInstalationLogDialog = qobject_cast<HbDialog*>(
        loader.findWidget(HS_INSTALLATION_LOG_DIALOG_NAME));
    
    if (mInstalationLogDialog != NULL) {
        mInstalationLogDialog->actions()[0]->setParent(mInstalationLogDialog);
        mInstalationLogDialog->setTimeout(HbPopup::NoTimeout);
        mInstalationLogDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    
        HbLabel* logList;
        QString applicationsList;
        QString listElement("%1 (%2)\n%3 - ");
        QString applicationName;
        QString version;
        QString date;
        QString type;
        QString installed(hbTrId("txt_applib_dialog_installed"));
        QString removed(hbTrId("txt_applib_dialog_removed"));
        QString newLine("\n");
    
        logList = qobject_cast<HbLabel*>(
                loader.findWidget(HS_INSTALLATION_LOG_DIALOG_LOG_LABEL));

        for (int i=0; i<detailsList.count(); i++) {
            applicationName = detailsList[i].value(CaSoftwareRegistry::componentNameKey());
            version = detailsList[i].value(CaSoftwareRegistry::componentVersionKey());
            date = detailsList[i].value(CaSoftwareRegistry::componentTimeKey());
            QString localListElement(listElement);
            localListElement = localListElement.arg(applicationName).arg(version).arg(date);
            type = detailsList[i].value(CaSoftwareRegistry::componentOperationTypeKey());
            if ((type.compare(CaSoftwareRegistry::componentInstallValue()) == 0) ||
                (type.compare(CaSoftwareRegistry::componentUpgradeValue()) == 0)) {
                localListElement = localListElement + installed;
                applicationsList = applicationsList + localListElement +
                        newLine + newLine;
            } else if (type.compare(CaSoftwareRegistry::componentUninstallValue()) == 0) {
                localListElement = localListElement + removed;
                applicationsList = applicationsList + localListElement + 
                        newLine + newLine;
            }
        }

        applicationsList.chop(2 * newLine.size());
        logList->setPlainText(applicationsList);
    
        mInstalationLogDialog->open(this, SLOT(stateExited()));
    }
}

void HsInstallationLogState::stateExited()
{
    mInstalationLogDialog = NULL;
    emit exit();
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsInstallationLogState::cleanUp()
{
    // Close popups if App key was pressed
    if (mInstalationLogDialog) {
        mInstalationLogDialog->close();
        mInstalationLogDialog = NULL;
    }
}

