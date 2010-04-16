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
* Description: 
*
*/

#include "tsdeactivation.h"

#include <QCoreApplication>
#include <QEvent>

/*!
    \class TsDeactivation
    \ingroup group_tsserviceplugin
    \brief Deactivation service.
	
	Service responsible for observation of all events that might hide TS. When any of 
	them occurs it is emiting signal.
*/
TsDeactivation::TsDeactivation(QObject *parent) : TsDeactivationInterface(parent)
{
    qApp->installEventFilter(this);
}

bool TsDeactivation::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationDeactivate) {
        emit deactivated();
    } 
    return TsDeactivationInterface::eventFilter(obj, event);  
}
