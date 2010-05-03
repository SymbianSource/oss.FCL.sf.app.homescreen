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
 * Description: hsovistorehandler.cpp
 *
 */

#include "hsovistorehandler.h"
#ifndef NO_QT_EXTENSIONS
#include "hsovistorehandler_p.h"
#else
#include <hbicon.h>
#include <hbnotificationdialog.h>
const QString DefaultOviStoreIcon("qtg_mono_ovistore");
const QString DefaultOviStoreLink("https://store.ovi.com/applications");
#endif

/*!
    \class HsOviStoreHandler
    \ingroup group_hsmenustateplugin

    \brief 

    \sa 
*/

/*!
 Constructor
 \param parent parent object
 */
HsOviStoreHandler::HsOviStoreHandler(QObject *parent):
    QObject(parent),
#ifndef NO_QT_EXTENSIONS
    m_q(new HsOviStoreHandlerPrivate(this))
#else
    m_q(0)
#endif
{
}

/*!
 Destructor
 */
HsOviStoreHandler::~HsOviStoreHandler()
{
}

/*!
 Returns Ovi Store icon
 \retval icon
 */
HbIcon HsOviStoreHandler::icon()
{
#ifndef NO_QT_EXTENSIONS
    return m_q->icon();
#else
    return HbIcon(DefaultOviStoreIcon);
#endif
}

/*!
 Open Ovi Store application or related url
 */
void HsOviStoreHandler::action()
{
#ifndef NO_QT_EXTENSIONS
    m_q->action();
#else
    HbNotificationDialog *notificationDialog = new HbNotificationDialog();
    notificationDialog->setParent(this);
    notificationDialog->setTitle(DefaultOviStoreLink);
    notificationDialog->show();
#endif
}


