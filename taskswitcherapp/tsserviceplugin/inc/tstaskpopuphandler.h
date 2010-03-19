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
#ifndef TSTASKPOPUPHANDLER_H
#define TSTASKPOPUPHANDLER_H

#include <QObject>
#include <QPointF>
#include <QAbstractItemModel>

class HbAbstractViewItem;
class T_TsTaskPopupHandler;

class TsTaskPopupHandler : public QObject
{
    Q_OBJECT

public:
    explicit TsTaskPopupHandler(QObject *parent = 0);

signals:
    void closeTask(const QModelIndex &index);
    void closeAllTasks();

public slots:
    void showTaskPopup(HbAbstractViewItem *item, const QPointF &coords);

private:
    bool isClosable(const QModelIndex &index) const;
    bool containsClosableItems(const QAbstractItemModel &model) const;

    friend class T_TsTaskPopupHandler;

};

#endif // TSTASKPOPUPHANDLER_H
