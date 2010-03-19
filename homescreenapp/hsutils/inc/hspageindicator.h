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
* Description:  Page indicator
*
*/

#ifndef HSPAGEINDICATOR_H
#define HSPAGEINDICATOR_H

#include <hbwidget.h>

#include "hsutils_global.h"
#include "hstest_global.h"

class QGraphicsLinearLayout;
class QParallelAnimationGroup;
class HbIconItem;

HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsPageIndicator: public HbWidget
{
    Q_OBJECT

public:
    HsPageIndicator(QGraphicsItem* parent = 0);
    ~HsPageIndicator();
       
signals:
    void currentItemChangeAnimationFinished();

public slots:
    void addItem(bool setAsCurrent=false,bool animated=true);
    void removeItem(bool animated=true);
    void setCurrentIndex(int currentIndex,bool animated=true);

public:
    void setItemCount(int itemCount);    
    int itemCount();
    int currentIndex();
    bool isAnimatingCurrentItemChange();
    
private slots:
    void resetEffect();
    
private: 
    void addItemInternal(bool setAsCurrent,int itemIndex,bool animated=true);

    void startItemAnimation();

private:
    int mItemCount;
    int mCurrentIndex;
    HbIcon *mNonActiveIcon;
    HbIcon *mActiveIcon;
    QSizeF mItemSize;
    QGraphicsLinearLayout *mPageIndicatorLayout;
    QParallelAnimationGroup *mIconAnimationGroup;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif
