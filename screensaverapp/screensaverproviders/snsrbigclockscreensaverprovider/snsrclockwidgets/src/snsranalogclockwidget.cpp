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
* Description:  Analog Clock Widget.
*
*/

#include "snsranalogclockwidget.h"

#include <QGraphicsSvgItem>
#include <QTime>

#include "snsrstyleoptionanalogclock.h"

#ifdef Q_OS_SYMBIAN
const char *gAnalogClockPluginPath("../../snsrresources/plugins/snsrstyleplugins/snsranalogclockstyleplugin.dll");
#else
const char *gAnalogClockPluginPath("snsrresources/plugins/snsrstyleplugins/snsranalogclockstyleplugin.dll");
#endif

/*!
    \class SnsrAnalogClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver analog clock widget.
 */

/*!
    Constructs a new SnsrAnalogClockWidget.
    \param stylePluginPath Style plugin path.
    \param parent Parent object.
 */
SnsrAnalogClockWidget::SnsrAnalogClockWidget(QGraphicsItem* parent):
    HbWidget(parent), mStylePluginName(gAnalogClockPluginPath),
    mClockBackground(0), mClockHourHand(0), mClockMinuteHand(0)
{
    setPluginBaseId(style()->registerPlugin(mStylePluginName));
    
    createPrimitives();
    updatePrimitives();
}

/*!
    Destructs the class.
 */
SnsrAnalogClockWidget::~SnsrAnalogClockWidget()
{
    delete mClockBackground;
    delete mClockHourHand;
    delete mClockMinuteHand;

    style()->unregisterPlugin(mStylePluginName);
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void SnsrAnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    updatePrimitives();    
}

/*!
    Creates all widget primitives.
 */
void SnsrAnalogClockWidget::createPrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }
    if (!mClockBackground) {
        mClockBackground = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()), this);
    }
    if (!mClockHourHand) {
        mClockHourHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionAnalogClock::HourHandItemIndex), this);
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionAnalogClock::MinuteHandItemIndex), this);
    }
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void SnsrAnalogClockWidget::updatePrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }

    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

    SnsrStyleOptionAnalogClock option;
    initStyleOption(&option);
    option.mM = m;
    option.mH = h;

    if (mClockBackground) {
        style()->updatePrimitive(mClockBackground, (HbStyle::Primitive)(pluginBaseId()), &option);
    }
    if (mClockHourHand) {
        style()->updatePrimitive(mClockHourHand, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionAnalogClock::HourHandItemIndex), &option);
    }
    if (mClockMinuteHand) {
        style()->updatePrimitive(mClockMinuteHand, (HbStyle::Primitive)(pluginBaseId()+SnsrStyleOptionAnalogClock::MinuteHandItemIndex), &option);
    }
}

/*!
    Updates primitives when time change.
 */
void SnsrAnalogClockWidget::tick()
{
    updatePrimitives();
}
