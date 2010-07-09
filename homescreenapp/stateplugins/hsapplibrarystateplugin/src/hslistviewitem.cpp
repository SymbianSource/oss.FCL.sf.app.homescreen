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
 * Description: List View Item.
 *
 */

#include <hbnamespace.h>
#include <HbProgressBar>

#include <HbStyleLoader>
#include <caitemmodel.h>
#include "hsmenuitemmodel.h"

#include "hslistviewitem.h"

HsListViewItem::HsListViewItem(QGraphicsItem* parent) : 
    HbListViewItem(parent), progress(0), isProgress(false)
{   
    setGraphicsSize(LargeIcon);
    HbStyleLoader::registerFilePath(":/layout/hslistviewitem.css");
    HbStyleLoader::registerFilePath(":/layout/hslistviewitem.widgetml");
}

HsListViewItem::~HsListViewItem()
{
    if (this == prototype()) {
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.css");
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.widgetml");
    }
}

void HsListViewItem::updateChildItems()
{
    HbListViewItem::updateChildItems();

    // DisplayRoles
    QVariant displayRole = modelIndex().data(Qt::DisplayRole);
    QStringList stringList;
    if (displayRole.isValid()) {
        if (displayRole.canConvert<QString>()) {
            stringList.append(displayRole.toString());
        } else if (displayRole.canConvert<QStringList>()) {
            stringList = displayRole.toStringList();
        }
    }
    EntryFlags flags = modelIndex().data(
        CaItemModel::FlagsRole).value<EntryFlags> ();
    isProgress = false;
    if (flags & UninstallEntryFlag) {
        isProgress = true;
        if (!progress) {
            progress = new HbProgressBar(this);
            progress->setRange(0, 100);
            HbEffect::disable(progress);
            repolish();
        }
        int progresVal = modelIndex().data(
                CaItemModel::UninstalRole).toInt();
        progress->setProgressValue(progresVal);
        HbStyle::setItemName(progress, "progress");         
        if (!progress->isVisible()) {
            progress->setVisible(!progress->isVisible());
            repolish();
        }    
    } else if (progress) {       
        HbStyle::setItemName(progress, "");
        if (progress->isVisible()) {
            progress->setVisible(!progress->isVisible());
            repolish();
        } 
    }
    // hide text-2 if we have to 
    foreach (QGraphicsItem * item, this->childItems()) {
        if (HbStyle::itemName(item) == "text-2") {
            item->setVisible(!isProgress);            
            break;
        } 
    }
}

HbAbstractViewItem*  HsListViewItem::createItem()
{
    return new HsListViewItem(*this);
}


void HsListViewItem::polish(HbStyleParameters& params)
{       
    HbListViewItem::setProperty("progress", isProgress);
    HbListViewItem::polish(params);   
}
