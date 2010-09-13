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

#include <HbMessageBox>
#include "hsuinstallfailedstate.h"
#include "hsmenuevent.h"
#include "hsapp_defs.h"

const int installerBusyError = 4;

/*!
 Constructor.
 /param parent Parent state.
 */
HsUninstallFailedState::HsUninstallFailedState(QState *parent) :
    QState(parent), mBox(0)
{
    construct();
}

/*!
 Destructor
 */
HsUninstallFailedState::~HsUninstallFailedState()
{
    cleanUp(); // in case of throw
}

/*!
 Construction
 */
void HsUninstallFailedState::construct()
{
    setObjectName("/HsUninstallFailedState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}


/*!
 Sets entry event.
 \param event entry event.
 */
void HsUninstallFailedState::onEntry(QEvent *event)
{

    
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    int error = data.value(Hs::uninstallError).toInt();
    if (error == installerBusyError) {
        // Installer is in use
        mBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
        mBox->setText(hbTrId("txt_applib_info_installer_is_currently_busy"));
    } else {
        // other errors
        mBox = new HbMessageBox(HbMessageBox::MessageTypeWarning);
        mBox->setText(hbTrId("txt_applib_info_uninstallation_failed"));
    }

    mBox->setAttribute(Qt::WA_DeleteOnClose);
    mBox->setStandardButtons(HbMessageBox::Close);
    mBox->open(this, SLOT(stateExited()));
}

/*!
 Invoked on exiting state
 */
void HsUninstallFailedState::onExit(QEvent *event)
{
    QState::onExit(event);
}

/*!
 State exited.
 */
void HsUninstallFailedState::stateExited()
{
    mBox = NULL;
    emit exit();
}

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsUninstallFailedState::cleanUp()
{
    // Close popups if App key was pressed
    if (mBox) {
        mBox->close();
        mBox = NULL;
    }
}
