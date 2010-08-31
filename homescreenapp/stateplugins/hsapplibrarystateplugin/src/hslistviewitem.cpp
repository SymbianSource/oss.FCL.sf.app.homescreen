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
#include <HbAbstractItemView>
#include <HbStyleLoader>
#include <QPainter>
#include <caitemmodel.h>
#include "hsmenuitemmodel.h"

#include "hslistviewitem.h"


// TODO: this is only temporary class for show progress bar.
// It should be remove when fix from orbit will be in official platfrom.
// Remove it from header too.

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsProgressBar::paint(QPainter * painter, 
        const QStyleOptionGraphicsItem * option, 
        QWidget * widget)
{
    Q_UNUSED(widget)
    QStyleOptionGraphicsItem pixmapOption(*option);
    foreach (QGraphicsItem *child, childItems()) {
        painter->save();            
        painter->translate(child->pos());
        pixmapOption.exposedRect = child->boundingRect();
        child->paint(painter, &pixmapOption, 0);
        
        foreach (QGraphicsItem *child2, child->childItems()) {
            if (child2->isVisible()) {
                painter->save();            
                painter->translate(child2->pos());
                pixmapOption.exposedRect = child2->boundingRect();
                child2->paint(painter, &pixmapOption, 0);
                painter->restore(); 
            }
        }
        painter->restore();
    }
}    
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


HsListViewItem::HsListViewItem(QGraphicsItem* parent) : 
    HbListViewItem(parent), progress(0), isProgress(false)
{   
    setGraphicsSize(LargeIcon);
    if (this == prototype()) {
        HbStyleLoader::registerFilePath(":/layout/hslistviewitem.css");        
    }
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

    EntryFlags flags = modelIndex().data(
        CaItemModel::FlagsRole).value<EntryFlags> ();
    isProgress = false;
    if (flags & UninstallEntryFlag) {
        isProgress = true;
        if (!progress) {
            progress = new HsProgressBar(this);
            HbStyle::setItemName(progress, "progress"); 
            progress->setRange(0, 100);
            HbEffect::disable(progress);  
            repolish();
        }
        int progresVal = modelIndex().data(
                CaItemModel::UninstalRole).toInt();
        progress->setProgressValue(progresVal);
    } else if (progress) {       
        delete progress;
        progress = 0;
        repolish();
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
    if (isProgress) {
        HbStyleLoader::registerFilePath(":/layout/hslistviewitem.widgetml");
    }
    HbListViewItem::setProperty("progress", isProgress);
    HbListViewItem::polish(params);   
    if (isProgress) {
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.widgetml");
    }
}

