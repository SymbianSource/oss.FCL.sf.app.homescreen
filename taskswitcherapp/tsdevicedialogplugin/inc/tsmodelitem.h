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
* Description: tsmodelitem.h
*
*/

#ifndef TSMODELITEM_H
#define TSMODELITEM_H
#include <QVariant>
#include <QModelIndex>
/*!
    Abstract class which define model item for taskswitcher
*/
class TsModelItem: public QObject
{
    Q_OBJECT

public:
    virtual QVariant data(int role) const =0;
    virtual void close() =0;
    virtual void open() =0;
};

#endif // TSMODELITEM_H
