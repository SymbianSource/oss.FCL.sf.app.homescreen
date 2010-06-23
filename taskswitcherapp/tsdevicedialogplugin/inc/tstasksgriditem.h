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
#ifndef TSTASKSGRIDITEM_H
#define TSTASKSGRIDITEM_H

#include <hbabstractviewitem.h>

class HbTextItem;
class HbIconItem;
class HbFrameItem;
class HbPushButton;

class TsTasksGridItem : public HbAbstractViewItem
{
    Q_OBJECT

public:
    TsTasksGridItem();
    TsTasksGridItem(const TsTasksGridItem &item);

    HbAbstractViewItem *createItem();
    void updateChildItems();

private slots:
    void handleDeleteClicked();

signals:
    void deleteClicked(const QModelIndex &index);

private:
    HbIconItem *mScreenshotLabel;
    HbTextItem *mApplicationNameLabel;
    HbPushButton *mDeleteButton;

    HbFrameItem *mActiveLabelFrame;
    HbTextItem *mActiveLabel;
};

#endif //TSTASKSGRIDITEM_H
