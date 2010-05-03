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
* Description:  Clock widget
*
*/

#ifndef HSANALOGCLOCKWIDGET_H
#define HSANALOGCLOCKWIDGET_H

#include <QGraphicsSvgItem>
#include <hbwidget.h>
#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HbIconItem;

class HsAnalogClockWidget : public HbWidget
{
    Q_OBJECT

public:

    explicit HsAnalogClockWidget(QGraphicsItem *parent = 0);
    virtual ~HsAnalogClockWidget();

public slots:

    void tick();

protected:

    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void polish(HbStyleParameters& params);

private:

    void updatePrimitives();

private:

    HbIconItem *mClockBackground;
    HbIconItem *mClockHourHand;
    HbIconItem *mClockMinuteHand;
    
    Q_DISABLE_COPY(HsAnalogClockWidget)

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
};




#endif
