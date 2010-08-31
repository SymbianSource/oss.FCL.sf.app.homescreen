/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "snsrlabel.h"

#include <QGraphicsSvgItem>
#include <QTime>
#include <QDebug>
#include <QLocale>

#include <HbStyleLoader>
#include <HbIconItem>
#include <HbTextItem>


const char *gOledClockCssFilePath = ":/style/snsroledclockwidget.css";
const char *gOledClockWidgetMLFilePath = ":/style/snsroledclockwidget.widgetml";
const char *gDateFormatStr = "ddd d"; // this is Qt format string, not Hb format like the others

/*!
    \class SnsrOledClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver oled clock widget.
 */

/*!
    Constructs a new SnsrOledClockWidget.
    \param parent Parent object.
 */
SnsrOledClockWidget::SnsrOledClockWidget(QGraphicsItem* parent):
    HbWidget(parent),
    mClockBackground(0), mClockHourHand(0), mClockMinuteHand(0)
    //,mClockDateFrame(0), mClockDateLabel(0)
{
    HbStyleLoader::registerFilePath(gOledClockCssFilePath);
    HbStyleLoader::registerFilePath(gOledClockWidgetMLFilePath);

    createPrimitives();
    updatePrimitives();
}

void SnsrOledClockWidget::polish( HbStyleParameters& params )
{
    // TODO: updatePrimitives might not be needed to be called from here anymore (hb changes)?
    HbWidget::polish(params);
    updatePrimitives();
}

/*!
    Destructs the class.
 */
SnsrOledClockWidget::~SnsrOledClockWidget()
{
    // TODO: deleted by parent?
  /*  delete mClockBackground;
    delete mClockHourHand;
    delete mClockMinuteHand;
    delete mClockDateFrame;
    delete mClockDateLabel;*/

    HbStyleLoader::unregisterFilePath( gOledClockCssFilePath );
    HbStyleLoader::unregisterFilePath( gOledClockWidgetMLFilePath );

}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void SnsrOledClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();
}

/*!
    Creates all widget primitives.
 */
void SnsrOledClockWidget::createPrimitives()
{
    // TODO: final graphics will be named qtg_graf_screensaver_clock_oled_xxx.
    // not available in platform yet
    // Also am/pm label should be added into the clock face and date label
    // be removed (=replace date with am/pm) ?

    if (!mClockBackground) {
        mClockBackground = new HbIconItem(QLatin1String("qtg_graf_clock_night_bg"), this);
        HbStyle::setItemName(mClockBackground, QLatin1String("oled_clock_background"));
    }
    if (!mClockHourHand) {
        mClockHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_night_hour"), this);
        HbStyle::setItemName(mClockHourHand, QLatin1String("oled_clock_hour_hand"));
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_night_min"), this);
        HbStyle::setItemName(mClockMinuteHand, QLatin1String("oled_clock_minute_hand"));
    }
  /*
    if (!mClockDateFrame) {
        mClockDateFrame = new HbIconItem(QLatin1String("oled-clock-date-frame"), this);
        HbStyle::setItemName(mClockDateFrame, QLatin1String("oled_clock_date_frame"));
    }
    if (!mClockDateLabel) {
        mClockDateLabel = new SnsrLabel(this);
        HbStyle::setItemName(mClockDateLabel, QLatin1String("oled_clock_date_label"));
    }*/
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void SnsrOledClockWidget::updatePrimitives()
{
    if (!mClockHourHand || !mClockMinuteHand /*|| !mClockDateLabel*/) {
        createPrimitives();
    }
    Q_ASSERT( mClockHourHand && mClockMinuteHand /*&& mClockDateLabel */);
    
    // Calculate angles for clock hands.
    // Use granularity of one minute so that minute hand is always exactly
    // on some minute and not between minutes. OLED clock is not updated more
    // frequently than once per minute and using finer granularity would cause
    // the minute hand to be always between even minutes.
    QTime time = QTime::currentTime();
    qreal m = 6 * time.minute();
    qreal h = 30 * ((time.hour() % 12) + m/360);
    
    int x = mClockHourHand->preferredSize().width()/2;
    int y = mClockHourHand->preferredSize().height()/2;
    mClockHourHand->setTransform(QTransform().translate(x, y).rotate(h).translate(-x, -y));

    x = mClockMinuteHand->preferredSize().width()/2;
    y = mClockMinuteHand->preferredSize().height()/2;
    mClockMinuteHand->setTransform(QTransform().translate(x, y).rotate(m).translate(-x, -y));

   // QString dateString = QLocale().toString(QDate::currentDate(), gDateFormatStr);
   // mClockDateLabel->setPlainText(dateString);
}

/*!
    Updates primitives when time change.
 */
void SnsrOledClockWidget::tick()
{
    updatePrimitives();
}
