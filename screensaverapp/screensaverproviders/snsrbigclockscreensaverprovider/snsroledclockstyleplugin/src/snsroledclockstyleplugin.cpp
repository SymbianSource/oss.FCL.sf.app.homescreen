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

#include "snsroledclockstyleplugin.h"

#include <QGraphicsDropShadowEffect>

#include <hbiconitem.h>

#include "snsrstyleoptionoledanalogclock.h"
#include "snsrlabel.h"

/*!
    \class SnsrOledClockStylePlugin
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver oled clock style plugin.
 */

/*!
    @copydoc HbStyleInterface::primitiveCount()
 */
int SnsrOledClockStylePlugin::primitiveCount() const
{
    return SnsrStyleOptionOledAnalogClock::LastItemIndex; // first index = 0, index of LastItemIndex = number of primitives;
}

/*!
    @copydoc HbStyleInterface::createPrimitive()
 */
HbWidgetBase* SnsrOledClockStylePlugin::createPrimitive(HbStyle::Primitive primitive,
                                                        QGraphicsItem *parent) const
{
    if (primitive == (int)SnsrStyleOptionOledAnalogClock::BackgroundItemIndex) {
        HbIconItem *item = new HbIconItem("oled-clock-background", parent);
        HbStyle::setItemName(item, "oled_clock_background");
        item->setZValue(1);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::DateFrameItemIndex) {
        HbIconItem *item = new HbIconItem("oled-clock-date-frame", parent);
        HbStyle::setItemName(item, "oled_clock_date_frame");
        item->setZValue(2);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::DateLabelItemIndex) {
        SnsrLabel *item = new SnsrLabel(parent);
        HbStyle::setItemName(item, "oled_clock_date_label");
        item->setZValue(3);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::HourHandItemIndex) {
        HbIconItem *item = new HbIconItem("oled-clock-hour-hand", parent);
        HbStyle::setItemName(item, "oled_clock_hour_hand");
        item->setZValue(4);
        return item;
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::MinuteHandItemIndex) {
        HbIconItem *item = new HbIconItem("oled-clock-minute-hand", parent);
        HbStyle::setItemName(item, "oled_clock_minute_hand");
        item->setZValue(5);
        return item;
    }

    return 0;
}

/*!
    @copydoc HbStyleInterface::updatePrimitive()
 */
void SnsrOledClockStylePlugin::updatePrimitive(QGraphicsItem *item,
                                               HbStyle::Primitive primitive,
                                               const QStyleOption *option ) const
{
    const SnsrStyleOptionOledAnalogClock *opt =
            qstyleoption_cast<const SnsrStyleOptionOledAnalogClock *>(option);
    if (!opt || !item) {
        return;
    }

    if (primitive == (int)SnsrStyleOptionOledAnalogClock::HourHandItemIndex) {
        HbIconItem *iconItem = static_cast<HbIconItem *>(item);
        int x = iconItem->iconItemSize().width()/2;
        int y = iconItem->iconItemSize().height()/2;

        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mH).translate(-x, -y));
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::MinuteHandItemIndex) {
        HbIconItem *iconItem = static_cast<HbIconItem *>(item);
        int x = iconItem->iconItemSize().width()/2;
        int y = iconItem->iconItemSize().height()/2;

        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mM).translate(-x, -y));
    }
    else if (primitive == (int)SnsrStyleOptionOledAnalogClock::DateLabelItemIndex) {
        HbLabel *dateLabel = static_cast<HbLabel *>(item);
        dateLabel->setPlainText(opt->mShortDate);
    }
}

/*!
    @copydoc HbStyleInterface::layoutPath()
 */
QString SnsrOledClockStylePlugin::layoutPath() const
{
    return QString(":/");
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(snsroledclockstyleplugin, SnsrOledClockStylePlugin)
#endif //COVERAGE_MEASUREMENT
