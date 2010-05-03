/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hsanalogclockwidget.h"
#include <QGraphicsGridLayout>
#include <QTime>
#include <QDir>
#include <HbIconItem>
#include <HbStyleLoader>

/*!
    \class HsAnalogClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Homescreen themable analog clock widget.

*/

/*!
    \fn HsAnalogClockWidget::HsAnalogClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)

    Constructs widget.
*/
HsAnalogClockWidget::HsAnalogClockWidget(QGraphicsItem *parent)
    : HbWidget(parent),
      mClockBackground(0), 
      mClockHourHand(0), 
      mClockMinuteHand(0)
{
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.css");

    updatePrimitives();
}

/*!
    \fn HsAnalogClockWidget::~HsAnalogClockWidget()

    Destructor.
*/
HsAnalogClockWidget::~HsAnalogClockWidget()
{
    delete mClockBackground;
    delete mClockHourHand;
    delete mClockMinuteHand;

    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.css");
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void HsAnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    updatePrimitives();   
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void HsAnalogClockWidget::updatePrimitives()
{
    if (!mClockBackground) {
        mClockBackground = new HbIconItem(QLatin1String("qtg_graf_clock_day_bg"), this);
        HbStyle::setItemName(mClockBackground, QLatin1String("clock_background"));
    }

    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

	if (!mClockHourHand) {
        mClockHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_hour"), this);
        HbStyle::setItemName(mClockHourHand, QLatin1String("clock_hour_hand"));
    }

    // these should work but don't
    //int x = mClockHourHand->iconItemSize().width()/2;
    //int y = mClockHourHand->iconItemSize().height()/2;
    // workaround
    int x = mClockHourHand->preferredSize().width()/2;
    int y = mClockHourHand->preferredSize().height()/2;
    mClockHourHand->setTransform(QTransform().translate(x, y).rotate(h).translate(-x, -y));

	if (!mClockMinuteHand) {
        mClockMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_min"), this);
        HbStyle::setItemName(mClockMinuteHand, QLatin1String("clock_minute_hand"));
    }

    // these should work but don't
    //int x = mClockMinuteHand->iconItemSize().width()/2;
    //int y = mClockMinuteHand->iconItemSize().height()/2;
    // workaround
    x = mClockMinuteHand->preferredSize().width()/2;
    y = mClockMinuteHand->preferredSize().height()/2;
    mClockMinuteHand->setTransform(QTransform().translate(x, y).rotate(m).translate(-x, -y));

}

/*!
    Updates clock visualization according to current time
 */
void HsAnalogClockWidget::tick()
{
    updatePrimitives();
    update();
}

/*!
    Update primitives also here because a problem in Hb - to be removed later on
 */
void HsAnalogClockWidget::polish( HbStyleParameters& params ) 
{  
    HbWidget::polish(params); 
    updatePrimitives();
} 
