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
#ifndef TSPRESENTATION_H
#define TSPRESENTATION_H

#include "tspresentationinterface.h"
#include "tsdocumentloader.h"

class HbDialog;
class TsTasksGrid;

class TsPresentation : public TsPresentationInterface
{
    Q_OBJECT

public:
    TsPresentation(QObject *parent = 0);

public:
    virtual void setModel(QAbstractItemModel *model);

public slots:
    virtual void hide();
    virtual void show();

private slots:
    void loadOrientationSpecificLayoutData(Qt::Orientation orientation);

private:
    TsDocumentLoader mLoader;
    HbDialog *mDialog;
    TsTasksGrid *mGrid;

};

#endif // TSPRESENTATION_H
