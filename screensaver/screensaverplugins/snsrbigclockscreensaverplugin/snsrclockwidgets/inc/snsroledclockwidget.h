/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Oled Clock Widget.
*
*/

#ifndef SNSROLEDCLOCKWIDGET_H
#define SNSROLEDCLOCKWIDGET_H

#include <hbwidget.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrClockWidgets)

class HbIconItem;
class HbTextItem;
//class SnsrLabel;

class SnsrOledClockWidget: public HbWidget
{
    Q_OBJECT

public:

    explicit SnsrOledClockWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrOledClockWidget();

public slots:

    void tick();

private:

    void resizeEvent (QGraphicsSceneResizeEvent *event);
    void createPrimitives();
    void updatePrimitives();
    void polish( HbStyleParameters& params );

private:

    HbIconItem *mClockBackground;
    HbIconItem *mClockHourHand;
    HbIconItem *mClockMinuteHand;
    // TODO: not known yet what label (or any) is shown in clock face (day or am/pm or none)
    // If some label will be put onto the clock face, then it's color must come
    // from theme and is most propably different than SnsrLabel uses (=text/icon color
    // used in screensaver in general
    //HbIconItem *mClockDateFrame;
    //SnsrLabel *mClockDateLabel;
    
    Q_DISABLE_COPY(SnsrOledClockWidget)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrClockWidgets)

};

#endif // SNSROLEDCLOCKWIDGET_H