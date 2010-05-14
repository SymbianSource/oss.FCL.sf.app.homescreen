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
 * Description: hsoperatorhandler.cpp
 *
 */

#include "hsoperatorhandler.h"
#ifndef NO_QT_EXTENSIONS
#include "hsoperatorhandler_p.h"
#else
#include <HbIcon>
#endif

/*!
    \class HsOperatorHandler
    \ingroup group_hsmenustateplugin

    \brief

    \sa
*/

/*!
 Constructor
 \param parent parent object
 */
HsOperatorHandler::HsOperatorHandler(QObject *parent):
    QObject(parent),
#ifndef NO_QT_EXTENSIONS
    m_q(new HsOperatorHandlerPrivate(this))
#else
    m_q(0)
#endif
{
}

/*!
 Destructor
 */
HsOperatorHandler::~HsOperatorHandler()
{
}

/*!
 Returns Operators icon
 \retval icon
 */
HbIcon HsOperatorHandler::icon()
{
#ifndef NO_QT_EXTENSIONS
    return m_q->icon();
#else
    return HbIcon();
#endif
}

/*!
 Returns Operators text
 \retval icon
 */
QString HsOperatorHandler::text()
{
#ifndef NO_QT_EXTENSIONS
    return m_q->text();
#else
    return QString();
#endif
}


/*!
 Open Operator application or related url
 */
void HsOperatorHandler::action()
{
#ifndef NO_QT_EXTENSIONS
    m_q->action();
#endif
}


