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
#include "tstasksgriditem.h"

#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbabstractitemview.h>
#include <hbanchorlayout.h>
#include "tsdataroles.h"

TsTasksGridItem::TsTasksGridItem() : HbAbstractViewItem()
{
    // add screenshot
    mScreenshotLabel = new HbLabel();
    mScreenshotLabel->setAlignment(Qt::AlignHCenter);

    // add application name label
    mApplicationNameLabel = new HbLabel();
    mApplicationNameLabel->setAlignment(Qt::AlignCenter);

    // add close app button
    HbIcon deleteIcon(":/resource/delete.png");
    mDeleteButton = new HbPushButton();
    mDeleteButton->setIcon(deleteIcon);
    mDeleteButton->setPreferredSize(48, 48);
    mDeleteButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mDeleteButton->setEnabled(true);
    connect(mDeleteButton, SIGNAL(clicked()), this, SLOT(handleDeleteClicked()));
    mDeleteButton->hide();

    setContentsMargins(5, 5, 5, 5);

    // add all items to layout
    HbAnchorLayout *anchorLayout = new HbAnchorLayout();
    anchorLayout->setAnchor(mApplicationNameLabel, Hb::LeftEdge, anchorLayout, Hb::LeftEdge, 0);
    anchorLayout->setAnchor(mApplicationNameLabel, Hb::TopEdge, anchorLayout, Hb::TopEdge, 0);
    anchorLayout->setAnchor(mApplicationNameLabel, Hb::RightEdge, anchorLayout, Hb::RightEdge, 0);

    anchorLayout->setAnchor(mScreenshotLabel, Hb::TopEdge, mApplicationNameLabel, Hb::BottomEdge, 0);
    anchorLayout->setAnchor(mScreenshotLabel, Hb::LeftEdge, anchorLayout, Hb::LeftEdge, 0);
    anchorLayout->setAnchor(mScreenshotLabel, Hb::RightEdge, anchorLayout, Hb::RightEdge, 0);
    anchorLayout->setAnchor(mScreenshotLabel, Hb::BottomEdge, anchorLayout, Hb::BottomEdge, 0);

    anchorLayout->setAnchor(mDeleteButton, Hb::RightEdge, mScreenshotLabel, Hb::RightEdge, 0);
    anchorLayout->setAnchor(mDeleteButton, Hb::TopEdge, mScreenshotLabel, Hb::TopEdge, 0);

    setLayout(anchorLayout);
}

TsTasksGridItem::~TsTasksGridItem()
{
}

HbAbstractViewItem *TsTasksGridItem::createItem()
{
    TsTasksGridItem *newItem = new TsTasksGridItem();
    connect(newItem, SIGNAL(deleteClicked(QModelIndex)), itemView(), SIGNAL(deleteButtonClicked(QModelIndex)));
    return newItem;
}

void TsTasksGridItem::handleDeleteClicked()
{
    emit deleteClicked(modelIndex());
}

void TsTasksGridItem::updateChildItems()
{
    mScreenshotLabel->setIcon(modelIndex().data(Qt::DecorationRole).value<HbIcon>());
    mApplicationNameLabel->setPlainText(modelIndex().data(Qt::DisplayRole).toString());
    QVariant status(modelIndex().data(TsDataRoles::Closable));
    const bool isRunning(status.isValid() && status.toBool());
    if (isRunning) {
        mDeleteButton->show();
    } else {
        mDeleteButton->hide();
    }
}
