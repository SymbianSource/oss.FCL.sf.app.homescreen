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
#ifndef TSPRESENTATIONINTERFACE_H
#define TSPRESENTATIONINTERFACE_H

#include <QObject>

class QAbstractItemModel;
class QModelIndex;

class TsPresentationInterface : public QObject
{
    Q_OBJECT

public:
    TsPresentationInterface(QObject *parent = 0) : QObject(parent) {}
    virtual ~TsPresentationInterface() {};

public:
    Q_INVOKABLE virtual void setModel(QAbstractItemModel *model) = 0;

public slots:
    virtual void hide() = 0;
    virtual void show() = 0;

signals:
    void openApplication(const QModelIndex &index);
    void closeApplication(const QModelIndex &index);
    void closeAllApplications();
    void hideTaskSwitcher();

};

#endif // TSPRESENTATIONINTERFACE_H
