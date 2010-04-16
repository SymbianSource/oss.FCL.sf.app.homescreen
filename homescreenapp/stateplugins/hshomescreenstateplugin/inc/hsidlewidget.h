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


#ifndef HSIDLEWIDGET_H
#define HSIDLEWIDGET_H

#include <HbWidget>

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HsPage;
class HsTrashBinWidget;
class HsPageIndicator;

class HsIdleWidget : public HbWidget
{
    Q_OBJECT

public:
    HsIdleWidget(QGraphicsItem *parent = 0);
	~HsIdleWidget();

    void setGeometry(const QRectF &rect);

    void captureDelayedPress(QGraphicsSceneMouseEvent *event);
    void sendDelayedPress();
    void clearDelayedPress();

    void setActivePage(int index);
    void insertPage(int index, HsPage *page);
    void removePage(int index);

    HbWidget *controlLayer() const { return mControlLayer; }
    HbWidget *pageLayer() const { return mPageLayer; }
    HbWidget *sceneLayer() const { return mSceneLayer; }

    HsTrashBinWidget *trashBin() const { return mTrashBin; }
    HsPageIndicator *pageIndicator() const { return mPageIndicator; }

signals:    
    void mousePressed(QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void mouseMoved(QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);
    void mouseReleased(QGraphicsItem *watched, QGraphicsSceneMouseEvent *event, bool &filtered);

public slots:
    void showTrashBin();
    void showPageIndicator();

protected:
    bool eventFilter(QObject *object, QEvent *event);
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
    void polishEvent();

private:
    Q_DISABLE_COPY(HsIdleWidget)
    void loadControlLayer();
    void setItemsUnfocusable(QGraphicsSceneMouseEvent *event);
    void setItemsFocusable();

private:
    HbWidget *mControlLayer;
    HbWidget *mPageLayer;
    HbWidget *mSceneLayer;

    QMouseEvent *mDelayedPressEvent;

    HsTrashBinWidget *mTrashBin;
    HsPageIndicator *mPageIndicator;

    QList<QGraphicsItem *> mFocusableItems;

    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSIDLEWIDGET_H
