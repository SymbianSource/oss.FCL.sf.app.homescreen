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

namespace
{
    const char PLUGIN_PATH[] = "/hsresources/plugins/styleplugins/hsanalogclockstyleplugin.dll";
}

/*!
    \class HsAnalogClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Homescreen themable analog clock widget.

*/

/*!
    \fn HsAnalogClockWidget::HsAnalogClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)

    Constructs widget.
*/
HsAnalogClockWidget::HsAnalogClockWidget(const QString &stylePluginName, QGraphicsItem *parent)
    : HbWidget(parent),
      mClockBackground(0), 
      mClockHourHand(0), 
      mClockMinuteHand(0)
{
    if (stylePluginName.isEmpty()) {
        mStylePluginName = QDir::currentPath() + PLUGIN_PATH;
    } else {
        mStylePluginName = stylePluginName;
    }
    setPluginBaseId(style()->registerPlugin(mStylePluginName));

    createPrimitives();
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

    style()->unregisterPlugin(mStylePluginName);    
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
    Creates all widget primitives.
 */
void HsAnalogClockWidget::createPrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }
    if (!mClockBackground) {
        mClockBackground = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()), this);
    }
    if (!mClockHourHand) {
        mClockHourHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+hourHandItemIndex), this);
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = style()->createPrimitive((HbStyle::Primitive)(pluginBaseId()+minuteHandItemIndex), this);
    }
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void HsAnalogClockWidget::updatePrimitives()
{
    if (pluginBaseId()==-1) {
        return;
    }

    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360)-90;
    HsAnalogClockStyleOption option;
    initStyleOption(&option);
    option.mM = m;
    option.mH = h;

    if (mClockBackground) {
        style()->updatePrimitive(mClockBackground, (HbStyle::Primitive)(pluginBaseId()), &option);
    }
    if (mClockHourHand) {
        style()->updatePrimitive(mClockHourHand, (HbStyle::Primitive)(pluginBaseId()+hourHandItemIndex), &option);
    }
    if (mClockMinuteHand) {
        style()->updatePrimitive(mClockMinuteHand, (HbStyle::Primitive)(pluginBaseId()+minuteHandItemIndex), &option);
    }
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
