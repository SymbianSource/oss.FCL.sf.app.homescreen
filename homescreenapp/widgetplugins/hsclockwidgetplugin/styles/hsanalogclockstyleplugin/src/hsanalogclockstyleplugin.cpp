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

#include "hsanalogclockstyleplugin.h"

#include <hbstyle.h>
#include <hbiconitem.h>

#include "hsanalogclockstyleoption.h"

/*!
    \class HsAnalogClockStylePlugin
    \brief Screensaver analog clock style plugin.
 */

/*!
    @copydoc HbStyleInterface::primitiveCount()
 */
int HsAnalogClockStylePlugin::primitiveCount() const
{
    return 3;
}

/*!
    @copydoc HbStyleInterface::createPrimitive()
 */
HbWidgetBase* HsAnalogClockStylePlugin::createPrimitive(HbStyle::Primitive primitive, QGraphicsItem *parent) const
{
    if (primitive == backgroundItemIndex) {
        HbIconItem *item = new HbIconItem("qtg_graf_clock_day_bg", parent);
        HbStyle::setItemName(item, "clock_background");
        item->setZValue(1);
        return item;
    }
    else if (primitive == hourHandItemIndex) {
        HbIconItem *item = new HbIconItem("qtg_graf_clock_day_hour", parent);
        HbStyle::setItemName(item, "clock_hour_hand");
        item->setZValue(3); // hour hand is on top of minute hand
        return item;
    }
    else if (primitive == minuteHandItemIndex) {
        HbIconItem *item = new HbIconItem("qtg_graf_clock_day_min", parent);
        HbStyle::setItemName(item, "clock_minute_hand");
        item->setZValue(2); // minute hand is below hour hand
        return item;
    }
    return 0;
}

/*!
    @copydoc HbStyleInterface::updatePrimitive()
 */
void HsAnalogClockStylePlugin::updatePrimitive(QGraphicsItem *item, HbStyle::Primitive primitive, const QStyleOption *option ) const
{
    const HsAnalogClockStyleOption *opt = qstyleoption_cast<const HsAnalogClockStyleOption *>(option);
    if (!opt || !item) {
        return;
    }

    HbIconItem *iconItem = static_cast<HbIconItem *>(item);
    
    // these should work but don't
    //int x = iconItem->iconItemSize().width()/2;
    //int y = iconItem->iconItemSize().height()/2;

    // workaround
    int x = iconItem->preferredSize().width()/2;
    int y = iconItem->preferredSize().height()/2;

    if (primitive == hourHandItemIndex) {
        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mH).translate(-x, -y));
    }
    else if (primitive == minuteHandItemIndex) {
        iconItem->setTransform(QTransform().translate(x, y).rotate(opt->mM).translate(-x, -y));
    }
}

/*!
    @copydoc HbStyleInterface::layoutPath()
 */
QString HsAnalogClockStylePlugin::layoutPath() const
{
    return QString(":/");
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(hsanalogclockstyleplugin, HsAnalogClockStylePlugin)
#endif //COVERAGE_MEASUREMENT
