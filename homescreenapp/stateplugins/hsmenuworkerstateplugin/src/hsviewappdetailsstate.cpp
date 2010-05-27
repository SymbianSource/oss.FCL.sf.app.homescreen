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

#include <QDebug>
#include <HbDialog>
#include <HbDocumentLoader>
#include <HbLabel>
#include <HbAction>

#include "caservice.h"
#include "casoftwareregistry.h"

#include "canotifier.h"
#include "canotifierfilter.h"

#include "hsviewappdetailsstate.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"

HsViewAppDetailsState::HsViewAppDetailsState(QState *parent) :
    QState(parent), mDialog(0), mNotifier(0)
{
    construct();
}

HsViewAppDetailsState::~HsViewAppDetailsState()
{
    if (mDialog) {
        delete mDialog;
    }
}

void HsViewAppDetailsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::construct");
    setObjectName(this->parent()->objectName()
                  + "/ViewAppDetailsState");
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::construct");
}

void HsViewAppDetailsState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    
    const int entryId = data.value(itemIdKey()).toInt();
    QSharedPointer<const CaEntry> entry = CaService::instance()->getEntry(entryId);
    const int componentId = entry->attribute(componentIdAttributeName()).toInt();
    
    QSharedPointer<CaSoftwareRegistry> scr = CaSoftwareRegistry::create();
    CaSoftwareRegistry::DetailMap detailMap = scr->entryDetails(componentId);
    
    HbDocumentLoader loader;
    bool loadStatusOk = false;
    mObjectList = 
        loader.load(HS_DETAILS_DIALOG_LAYOUT, &loadStatusOk);
    Q_ASSERT_X(loadStatusOk,
        HS_DETAILS_DIALOG_LAYOUT,
           "Error while loading docml file.");

    mDialog = qobject_cast<HbDialog*>(loader.findWidget(HS_DETAILS_DIALOG_NAME));
    mDialog->actions()[0]->setParent(mDialog);
    
    subscribeForMemoryCardRemove(entryId);
    
    HbLabel* nameLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentNameKey()));
    HbLabel* versionLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentVersionKey()));
    HbLabel* supplierLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentVendorKey()));
    HbLabel* memoryLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentDriveInfoKey()));
    HbLabel* sizeLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentSizeKey()));
    HbLabel* typeLabel = qobject_cast<HbLabel*>(loader.findWidget(CaSoftwareRegistry::componentTypeKey()));    
    
    nameLabel->setPlainText(detailMap[CaSoftwareRegistry::componentNameKey()]);
    versionLabel->setPlainText(detailMap[CaSoftwareRegistry::componentVersionKey()]);
    supplierLabel->setPlainText(detailMap[CaSoftwareRegistry::componentVendorKey()]);
    if(detailMap[CaSoftwareRegistry::componentDriveInfoKey()] == "C:")
        memoryLabel->setPlainText(QString("%1 txt_applib_dialog_1_device_memory").arg(detailMap[CaSoftwareRegistry::componentDriveInfoKey()]));
    else if (detailMap[CaSoftwareRegistry::componentDriveInfoKey()] == "E:")
        memoryLabel->setPlainText(QString("%1 txt_applib_dialog_1_mass_storage").arg(detailMap[CaSoftwareRegistry::componentDriveInfoKey()]));
    else if (detailMap[CaSoftwareRegistry::componentDriveInfoKey()] == "F:")
        memoryLabel->setPlainText(QString("%1 txt_applib_dialog_1_memory_card").arg(detailMap[CaSoftwareRegistry::componentDriveInfoKey()]));
    else
        memoryLabel->setPlainText(detailMap[CaSoftwareRegistry::componentDriveInfoKey()]);
    sizeLabel->setPlainText(detailMap[CaSoftwareRegistry::componentSizeKey()]);
    typeLabel->setPlainText(detailMap[CaSoftwareRegistry::componentTypeKey()]);
    
    if (mDialog != NULL) {
        mDialog->setTimeout(HbPopup::NoTimeout);
        mDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        mDialog->open(this, SLOT(stateExited())); 
    }
    
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::onEntry");    
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Slot invoked when a state is exited.
 */
void HsViewAppDetailsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::stateExited");
    mDialog = NULL;
    emit exit();
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::stateExited");
    qDebug("HsViewAppDetailsState::stateExited()");
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsViewAppDetailsState::onExit(QEvent *event)
{
    QState::onExit(event);
    // Close popups if App key was pressed
    if (mDialog) {
        mDialog->close();
    }
    mDialog = NULL;
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Subscribe for memory card remove.
 \param entryId: entry id.
 \retval void
 */
void HsViewAppDetailsState::subscribeForMemoryCardRemove(int entryId)
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

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Memory card with instaled widget was removed.
 \retval void
 */
void HsViewAppDetailsState::memoryCardRemoved()
{
    if (mDialog) {
    mDialog->close();
    }
    // exit not needed, it is called after dialog closed

}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

