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

#include <HbPushButton>
#include <HbAbstractItemView>
#include <HbFrameItem>
#include <HbTextItem>
#include <HbIconItem>
#include <HbStyleLoader>

#include "tsdataroles.h"

/*!
    \class TsTasksGridItem
    \ingroup group_tsdevicedialogplugin
    \brief Item that should be presented in grid.
*/


TsTasksGridItem::TsTasksGridItem() : HbAbstractViewItem(), mScreenshotLabel(0), mApplicationNameLabel(0), mDeleteButton(0), mActiveLabelFrame(0), mActiveLabel(0)
{
    // Register the custom docml and css to provide our own style to the list items
    bool widgetmlLoaded = HbStyleLoader::registerFilePath(":/resource/tstasksgriditem.widgetml");
    Q_ASSERT(widgetmlLoaded);
    bool cssLoaded = HbStyleLoader::registerFilePath(":/resource/tstasksgriditem.css");
    Q_ASSERT(cssLoaded);
}

TsTasksGridItem::TsTasksGridItem(const TsTasksGridItem &item) : HbAbstractViewItem(item)
{
    mScreenshotLabel = new HbIconItem(this);
    mApplicationNameLabel = new HbTextItem(this);
    mDeleteButton = new HbPushButton(this);
    HbFrameItem *screenshotFrame = new HbFrameItem(this);    
    screenshotFrame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    screenshotFrame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");

    HbFrameItem *applicationLabelFrame = new HbFrameItem(this);
    applicationLabelFrame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    applicationLabelFrame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");

    mActiveLabel = new HbTextItem(hbTrId("txt_tsw_label_active"), this);
    mActiveLabelFrame = new HbFrameItem(this);
    mActiveLabelFrame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    mActiveLabelFrame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");

    mDeleteButton->setIcon(HbIcon("qtg_mono_exit"));

    HbStyle::setItemName(mScreenshotLabel, "screenshot");
    HbStyle::setItemName(mApplicationNameLabel, "appname");
    HbStyle::setItemName(mDeleteButton, "closebadge");
    mDeleteButton->setObjectName("closebadge");
    HbStyle::setItemName(screenshotFrame, "screenshotFrame");
    HbStyle::setItemName(applicationLabelFrame, "applicationLabelFrame");
    HbStyle::setItemName(mActiveLabel, "activeLabel");
    HbStyle::setItemName(mActiveLabelFrame, "activeLabelFrame");
    connect(mDeleteButton, SIGNAL(clicked()), this, SLOT(handleDeleteClicked()));
}

HbAbstractViewItem *TsTasksGridItem::createItem()
{
    TsTasksGridItem *newItem = new TsTasksGridItem(*this);
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
    mApplicationNameLabel->setText(modelIndex().data(Qt::DisplayRole).toString());
    QVariant status(modelIndex().data(TsDataRoles::Closable));
    const bool isRunning(status.isValid() && status.toBool());
    if (isRunning) {
        mDeleteButton->show();
        mActiveLabel->show();
        mActiveLabelFrame->show();
    } else {
        mDeleteButton->hide();
        mActiveLabel->hide();
        mActiveLabelFrame->hide();
    }
}
