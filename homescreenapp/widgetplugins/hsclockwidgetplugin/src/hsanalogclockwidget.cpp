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
* Description:  Analog clock widget
*
*/

#include <QGraphicsSceneMouseEvent>
#include <QTime>

#include <HbStyleLoader>
#include <HbIconItem>
#include <HbTouchArea>
#include <HbInstantFeedback>

#include "hsconfiguration.h"
#include "hsanalogclockwidget.h"

/*!
    \class HsAnalogClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Homescreen themable analog clock widget.
*/

/*!
    Constructs widget.
*/
HsAnalogClockWidget::HsAnalogClockWidget(QGraphicsItem *parent)
    : HbWidget(parent),
      mBackground(0), mHourHand(0), mMinuteHand(0), 
      mSecondHand(0), mTouchArea(0)
{
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.css");

    createPrimitives();
}

/*!
    Destructor.
*/
HsAnalogClockWidget::~HsAnalogClockWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsanalogclockwidget.css");
    mTouchArea->removeEventFilter(this);
}

/*!
    Filters touch area events.
*/
bool HsAnalogClockWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:
            return true;
        case QEvent::GraphicsSceneMouseRelease:
            handleMouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            return true;
        default:
            break;
    }

    return false;
}

/*!
    Updates clock visualization according to current time
 */
void HsAnalogClockWidget::tick()
{
    updatePrimitives();
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void HsAnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();   
}

/*!
    Creates all widget primitives.
 */
void HsAnalogClockWidget::createPrimitives()
{
    mBackground = new HbIconItem(QLatin1String("qtg_graf_clock_day_bg"), this);
    HbStyle::setItemName(mBackground, QLatin1String("background"));

    mHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_hour"), this);
    HbStyle::setItemName(mHourHand, QLatin1String("hour_hand"));

    mMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_min"), this);
    HbStyle::setItemName(mMinuteHand, QLatin1String("minute_hand"));

    mSecondHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_sec"), this);
    HbStyle::setItemName(mSecondHand, QLatin1String("second_hand"));

    mTouchArea = new HbTouchArea(this);
    mTouchArea->installEventFilter(this);    
    HbStyle::setItemName(mTouchArea, QLatin1String("toucharea"));
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void HsAnalogClockWidget::updatePrimitives()
{
    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

    int x = mBackground->iconItemSize().width()/2;
    int y = mBackground->iconItemSize().height()/2;
    QPointF originPoint = QPointF(x, y);

    mHourHand->setTransformOriginPoint(originPoint);
    mHourHand->setRotation(h);

    mMinuteHand->setTransformOriginPoint(originPoint);
    mMinuteHand->setRotation(m);

    mSecondHand->setTransformOriginPoint(originPoint);
    mSecondHand->setRotation(s);
}

/*!
    \internal
*/
void HsAnalogClockWidget::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!contains(event->pos())) {
        return;
    }

    HbInstantFeedback::play(HsConfiguration::clockWidgetTapFeedbackType());

    emit clockTapped();
}
