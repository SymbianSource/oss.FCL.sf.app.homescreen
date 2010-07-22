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


/*!
 Constructor.
 @param parent Parent state.
 */
HsViewAppDetailsState::HsViewAppDetailsState(QState *parent) :
    QState(parent), mDialog(0), mNotifier(0)
{
    construct();
}

/*!
 Destructor
 */
HsViewAppDetailsState::~HsViewAppDetailsState()
{
    if (mDialog) {
        delete mDialog;
    }
}

/*!
 Construction
 */
void HsViewAppDetailsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::construct");
        
    setObjectName("/ViewAppDetailsState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::construct");
}

/*!
 Invoked on entering state
 */
void HsViewAppDetailsState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    
    const int entryId = data.value(itemIdKey()).toInt();
    QSharedPointer<const CaEntry> entry 
        = CaService::instance()->getEntry(entryId);
    const int componentId = entry->attribute(
        componentIdAttributeName()).toInt();
    
    QSharedPointer<CaSoftwareRegistry> scr = CaSoftwareRegistry::create();
    CaSoftwareRegistry::DetailMap detailMap = scr->entryDetails(componentId);
    
    HbDocumentLoader loader;
    bool loadStatusOk = false;
    mObjectList = loader.load(HS_DETAILS_DIALOG_LAYOUT, &loadStatusOk);
    Q_ASSERT_X(loadStatusOk, HS_DETAILS_DIALOG_LAYOUT,
           "Error while loading docml file.");

    mDialog = qobject_cast<HbDialog*>(
        loader.findWidget(HS_DETAILS_DIALOG_NAME));
    mDialog->actions()[0]->setParent(mDialog);
    
    subscribeForMemoryCardRemove(entryId);
    
    HbLabel* nameLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentNameKey()));
    HbLabel* versionLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentVersionKey()));
    HbLabel* supplierLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentVendorKey()));
    HbLabel* memoryLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentDriveInfoKey()));
    HbLabel* sizeLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentSizeKey()));
    HbLabel* typeLabel = qobject_cast<HbLabel*>(
        loader.findWidget(CaSoftwareRegistry::componentTypeKey()));    
    
    nameLabel->setPlainText(
        detailMap[CaSoftwareRegistry::componentNameKey()]);
    versionLabel->setPlainText(
        detailMap[CaSoftwareRegistry::componentVersionKey()]);
    supplierLabel->setPlainText(
        detailMap[CaSoftwareRegistry::componentVendorKey()]);
    memoryLabel->setPlainText(
            detailMap[CaSoftwareRegistry::componentDriveInfoKey()]);
    sizeLabel->setPlainText(
        detailMap[CaSoftwareRegistry::componentSizeKey()]);

    typeLabel->setPlainText(
        detailMap[CaSoftwareRegistry::componentTypeKey()]);
    
    if (mDialog != NULL) {
        mDialog->setTimeout(HbPopup::NoTimeout);
        mDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        mDialog->open(this, SLOT(stateExited())); 
    }
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::onEntry");    
}

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

/*!
 Invoked on exiting state
 */
void HsViewAppDetailsState::onExit(QEvent *event)
{
    QState::onExit(event);
    // Close popups if App key was pressed or
    // memory card removed
    if (mDialog) {
        mDialog->close();
    }
    mDialog = NULL;
}

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
            SIGNAL(exit()));
}
