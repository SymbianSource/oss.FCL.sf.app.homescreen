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
* Description:  Menu Event.
*
*/

#include <QGraphicsLinearLayout>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPointer>

#include <HbIconItem>

#include "hspageindicator.h"

const char *KPageIndicatorActiveItemImageName = "qtg_graf_hspage_highlight";
const char *KPageIndicatorNonActiveItemImageName = "qtg_graf_hspage_normal";

/*! 
    \class HsPageIndicator
    \ingroup group_hsutils
    \brief Page indicator widget.
*/

/*!
    Constructor.

    \a parent Owner. 
*/
HsPageIndicator::HsPageIndicator(QGraphicsItem* parent):
    HbWidget(parent),
    mItemCount(0),
    mCurrentIndex(0),
    mNonActiveIcon(0),
    mActiveIcon(0),
    mIconAnimationGroup(0)
{
    mItemSize = QSizeF(30, 15);

    // perf improvement to load icons only once
    mNonActiveIcon = new HbIcon(KPageIndicatorNonActiveItemImageName);
    mActiveIcon = new HbIcon(KPageIndicatorActiveItemImageName);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->addStretch();

    mPageIndicatorLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->addItem(mPageIndicatorLayout);

    layout->addStretch();
    
    setLayout(layout);
}

/*!
    Destructor.
*/
HsPageIndicator::~HsPageIndicator()
{
}

/*! 
    \fn HsPageIndicator::currentItemChangeAnimationFinished()
    Signaled when current item is changed and animation is finished.
*/

/*!
   Add an item and animates if \a animated is true and sets the
   new page as current if \a setAsCurrent is true
*/
void HsPageIndicator::addItem(bool setAsCurrent,bool animated)
{
    mItemCount++;
    addItemInternal(setAsCurrent,mItemCount - 1,animated);
}

/*!
   Removes on item and animates if \a animated is true.
*/
void HsPageIndicator::removeItem(bool animated)
{
    if (mItemCount < 1) {
        return;
    }

    mItemCount--;

    if(mCurrentIndex == mItemCount) {
        setCurrentIndex(mItemCount-1,animated);
    }
    
    QGraphicsLayoutItem *item = mPageIndicatorLayout->itemAt(mPageIndicatorLayout->count() - 1);
    mPageIndicatorLayout->removeAt(mPageIndicatorLayout->count() - 1);

    if (mItemCount < 2) {
        hide();
    }
    
    delete item;
    layout()->invalidate();
}

/*!
    Set \a currentIndex as current item and animates the change if \a animated
    is true.
*/
void HsPageIndicator::setCurrentIndex(int currentIndex,bool animated)
{
    if( currentIndex < mPageIndicatorLayout->count() && currentIndex>=0) {
        if ( mIconAnimationGroup ) {
            mIconAnimationGroup->disconnect(this);
            resetEffect();        
        }
        
        QGraphicsLayoutItem *previousItem = mPageIndicatorLayout->itemAt(mCurrentIndex);
        HbIconItem *previousIconItem = static_cast<HbIconItem*>(previousItem);
        previousIconItem->setIcon(*mNonActiveIcon);
        mCurrentIndex = currentIndex;

        if (animated) {
            startItemAnimation();
        }
    }
}

/*!
    Sets the item count to \a itemCount. Ie. removes or adds items if 
    necessary.
*/
void HsPageIndicator::setItemCount(int itemCount)
{
    if (mItemCount < itemCount) {
        int count = itemCount - mItemCount;
        for (int i = 0; i < count; ++i) {
            addItem(false,false);
        }
    } else if (mItemCount > itemCount) {
        int count = mItemCount - itemCount;
        for (int i = 0; i < count; ++i) {
            removeItem(false);
        }
    }
}

/*!
    Returns the item count
*/
int HsPageIndicator::itemCount()
{
    return mItemCount;
}


/*!
    Returns current index.
*/
int HsPageIndicator::currentIndex()
{
    return mCurrentIndex;
}

/*!
    Returns true if current item animation is ongoing.
*/
bool HsPageIndicator::isAnimatingCurrentItemChange()
{
    return mIconAnimationGroup;
}

/*!
    \internal
*/
void HsPageIndicator::resetEffect()
{
    QGraphicsLayoutItem *item = mPageIndicatorLayout->itemAt(mCurrentIndex);
    HbIconItem *iconItem = static_cast<HbIconItem*>(item);
    if ( iconItem ) {
        QPointer<QGraphicsEffect> iconEffect = iconItem->graphicsEffect();
        iconItem->setGraphicsEffect(0);
        if (iconEffect) {
            delete iconEffect;
        }
    }
    mIconAnimationGroup = 0;
    emit currentItemChangeAnimationFinished();
}

/*!
    \internal    
*/
void HsPageIndicator::addItemInternal(bool setAsCurrent,int itemIndex,bool animated)
{    
    HbIconItem *iconItem = new HbIconItem();
    iconItem->setIcon(*mNonActiveIcon);
    iconItem->setPreferredSize(mItemSize);
    iconItem->setMinimumSize(mItemSize);
    iconItem->setMaximumSize(mItemSize);
    
    mPageIndicatorLayout->addItem(iconItem);
    if (setAsCurrent) {
        setCurrentIndex(itemIndex,animated);
    }
    layout()->invalidate();

    if (mItemCount < 2) {
        hide();
    } else {
        show();
    }    
}

/*!
    \internal    
*/
void HsPageIndicator::startItemAnimation()
{
    QGraphicsLayoutItem *item = mPageIndicatorLayout->itemAt(mCurrentIndex);
    HbIconItem *iconItem = static_cast<HbIconItem*>(item);
    iconItem->setIcon(*mActiveIcon);
    
    QGraphicsBlurEffect* iconBlurEffect = new QGraphicsBlurEffect();
    iconBlurEffect->setBlurRadius(9);
    iconItem->setGraphicsEffect(iconBlurEffect);
    iconItem->setTransformOriginPoint(iconItem->rect().center());
    
    mIconAnimationGroup = new QParallelAnimationGroup();

    QPropertyAnimation *animation = new QPropertyAnimation(iconItem, "scale");
    animation->setDuration(1000);
    animation->setKeyValueAt(0.5, 2);
    animation->setEndValue(1.0);
    mIconAnimationGroup->addAnimation(animation);

    connect(mIconAnimationGroup, SIGNAL(finished()), SLOT(resetEffect()));

    mIconAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);

}
