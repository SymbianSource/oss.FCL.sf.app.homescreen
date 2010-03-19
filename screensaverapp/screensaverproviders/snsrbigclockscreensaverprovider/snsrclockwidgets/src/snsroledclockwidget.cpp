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
* Description: Oled Clock Widget.
*
*/

#include "snsroledclockwidget.h"

#include <QGraphicsSvgItem>
#include <QTime>

#include "snsrstyleoptionoledanalogclock.h"

#ifdef Q_OS_SYMBIAN
const char *gOledClockPluginPath("../../snsrresources/plugins/snsrstyleplugins/snsroledclockstyleplugin.dll");
#else
const char *gOledClockPluginPath("snsrresources/plugins/snsrstyleplugins/snsroledclockstyleplugin.dll");
#endif

/*!
    \class SnsrOledClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver oled clock widget.
 */

/*!
    Constructs a new SnsrOledClockWidget.
    \param stylePluginPath Style plugin path.
    \param parent Parent object.
 */
SnsrOledClockWidget::SnsrOledClockWidget(QGraphicsItem* parent):
    HbWidget(parent), mStylePluginName(gOledClockPluginPath),
    mClockBackground(0), mClockHourHand(0), mClockMinuteHand(0),
    mClockDateFrame(0), mClockDateLabel(0)
{
    setPluginBaseId(style()->registerPlugin(mStylePluginName));

    createPrimitives();
    updatePrimitives();
}

/*!
    Destructs the class.
 */
SnsrOledClockWidget::~SnsrOledClockWidget()
{
    delete mClockBackground;
    delete mClockHourHand;
    delete mClockMinuteHand;
    delete mClockDateFrame;
    delete mClockDateLabel;

    style()->unregisterPlugin(mStylePluginName);
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void SnsrOledClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    updatePrimitives();
}

/*!
    Creates all widget primitives.
 */
void SnsrOledClockWidget::createPrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }
    if (!mClockBackground) {
        mClockBackground = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()), this);
    }
    if (!mClockHourHand) {
        mClockHourHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::HourHandItemIndex), this);
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::MinuteHandItemIndex), this);
    }
    if (!mClockDateFrame) {
        mClockDateFrame = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::DateFrameItemIndex), this);
    }
    if (!mClockDateLabel) {
        mClockDateLabel = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::DateLabelItemIndex), this);
    }
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void SnsrOledClockWidget::updatePrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }

    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

    SnsrStyleOptionOledAnalogClock option;
    initStyleOption(&option);
    option.mM = m;
    option.mH = h;

    QDate currentdate = QDate::currentDate();
    option.mShortDate = QDate::shortDayName(currentdate.dayOfWeek())+" "+QString::number(currentdate.day());

    if (mClockBackground) {
        style()->updatePrimitive(mClockBackground, (HbStyle::Primitive)(pluginBaseId()), &option);
    }
    if (mClockHourHand) {
        style()->updatePrimitive(mClockHourHand, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::HourHandItemIndex), &option);
    }
    if (mClockMinuteHand) {
        style()->updatePrimitive(mClockMinuteHand, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::MinuteHandItemIndex), &option);
    }
    if (mClockDateFrame) {
        style()->updatePrimitive(mClockDateFrame, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::DateFrameItemIndex), &option);
    }
    if (mClockDateLabel) {
        style()->updatePrimitive(mClockDateLabel, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionOledAnalogClock::DateLabelItemIndex), &option);
    }
}

/*!
    Updates primitives when time change.
 */
void SnsrOledClockWidget::tick()
{
    updatePrimitives();
}
