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
* Description:  Analog clock style plugin.
*
*/

#include "snsranalogclockstyleplugin.h"

#include <QGraphicsDropShadowEffect>
#include <hbiconitem.h>

#include "snsrstyleoptionanalogclock.h"

const int gDropShadowOffset(2);

/*!
    \class SnsrAnalogClockStylePlugin
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver analog clock style plugin.
 */

/*!
    @copydoc HbStyleInterface::primitiveCount()
 */
int SnsrAnalogClockStylePlugin::primitiveCount() const
{
    return SnsrStyleOptionAnalogClock::LastItemIndex; // first index = 0, index of LastItemIndex = number of primitives;
}

/*!
    @copydoc HbStyleInterface::createPrimitive()
 */
HbWidgetBase* SnsrAnalogClockStylePlugin::createPrimitive(HbStyle::Primitive primitive,
                                                          QGraphicsItem *parent) const
{
    if (primitive == (int)SnsrStyleOptionAnalogClock::BackgroundItemIndex) {
        HbIconItem *item = new HbIconItem("analog-clock-background", parent);
        HbStyle::setItemName(item, "analog_clock_background");
        item->setZValue(1);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionAnalogClock::HourHandItemIndex) {
        HbIconItem *item = new HbIconItem("analog-clock-hour-hand", parent);
        HbStyle::setItemName(item, "analog_clock_hour_hand");
        item->setZValue(2);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setOffset(gDropShadowOffset);
        item->setGraphicsEffect(effect);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionAnalogClock::MinuteHandItemIndex) {
        HbIconItem *item = new HbIconItem("analog-clock-minute-hand", parent);
        HbStyle::setItemName(item, "analog_clock_minute_hand");
        item->setZValue(3);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setOffset(gDropShadowOffset);
        item->setGraphicsEffect(effect);
        return item;
    }
    return 0;
}

/*!
    @copydoc HbStyleInterface::updatePrimitive()
 */
void SnsrAnalogClockStylePlugin::updatePrimitive(QGraphicsItem *item,
                                                 HbStyle::Primitive primitive,
                                                 const QStyleOption *option ) const
{
    const SnsrStyleOptionAnalogClock *opt =
            qstyleoption_cast<const SnsrStyleOptionAnalogClock *>(option);
    if (!opt || !item) {
        return;
    }

    HbIconItem *iconItem = static_cast<HbIconItem *>(item);

    int x = iconItem->iconItemSize().width()/2;
    int y = iconItem->iconItemSize().height()/2;

    if (primitive == (int)SnsrStyleOptionAnalogClock::HourHandItemIndex) {
        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mH).translate(-x, -y));
    }
    else if (primitive == (int)SnsrStyleOptionAnalogClock::MinuteHandItemIndex) {
        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mM).translate(-x, -y));
    }
}

/*!
    @copydoc HbStyleInterface::layoutPath()
 */
QString SnsrAnalogClockStylePlugin::layoutPath() const
{
    return QString(":/");
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(snsranalogclockstyleplugin, SnsrAnalogClockStylePlugin)
#endif //COVERAGE_MEASUREMENT
