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
* Description:  Container for digital clock.
*
*/

#include "snsrdigitalclockcontainer.h"

#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <HbExtendedLocale>
#include <HbMainWindow>

#include "snsrswipewidget.h"
#include "snsrindicatorwidget.h"
#include "snsrlabel.h"

/*!
    \class SnsrDigitalClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for digital clock.
 */

const char *gDigitalLayoutDocml = ":/xml/snsrbigclockscreensaverdigital.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;

const char *gTimeLabelName = "timeLabel";
const char *gAmPmLabelName = "amPmLabel";
extern const char *gDateLabelName;

extern const char *gDateFormatVerticalStr;
extern const char *gDateFormatHorizontalStr;

// This is same as r_qtn_time_usual_with_zero defined in
// hbi18ndef.h expect am/pm place holders are removed.
const char *gTimeFormatStr = "%:0%J%:1%T%:3";
const char *gAmPmFormatStr = "%B";

extern const char *gIndicatorWidgetName;
extern const char *gSwipeWidgetName;


/*!
    Constructs a new SnsrDigitalClockContainer.
 */
SnsrDigitalClockContainer::SnsrDigitalClockContainer() :
    SnsrBigClockContainer(),
    mTimeLabel(0),
    mAmPmLabel(0),
    mDateLabel(0)/*,
    mSwipeWidget(0)*/
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrDigitalClockContainer::SnsrDigitalClockContainer")
    SCREENSAVER_TEST_FUNC_EXIT("SnsrDigitalClockContainer::SnsrDigitalClockContainer")
}

/*!
    Destructs the class.
 */
SnsrDigitalClockContainer::~SnsrDigitalClockContainer()
{
    resetIndicatorConnections();
    //mTimeLabel, mAmPmLabel, mDateLabel - deleted by the parent
}

/*!
    Updates displayed time and date.
 */
void SnsrDigitalClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrDigitalClockContainer::update")

    // time
    // Even though the format string (hh:mm) contain the leading zero, it's
    // removed always when clock type is 12h -> h:mm. 
   
    // TODO: within this method call, you find a comment in code that
    // it's intentional to remove leading zero always with 12h type clock.
    // However it seems to be recommended to use hbExtendedLocale to get
    // proper time formatting for all locales and settings..
    mTimeLabel->setPlainText(
        HbExtendedLocale().format(QTime::currentTime(), gTimeFormatStr)
        );
    
    // if clock type is 24h, this will return an empty string.
    mAmPmLabel->setPlainText(
        HbExtendedLocale().format(QTime::currentTime(), gAmPmFormatStr)
        );

    // date
    const char *dateFormat = (mCurrentOrientation == Qt::Vertical) ?
        gDateFormatVerticalStr : gDateFormatHorizontalStr;
    QString dateText = HbExtendedLocale().format( QDate::currentDate(), dateFormat );
    mDateLabel->setPlainText( dateText );

    SCREENSAVER_TEST_FUNC_EXIT("SnsrDigitalClockContainer::update")
}

/*!
    @copydoc SnsrBigClockContainer::updateIntervalInMilliseconds()
 */
int SnsrDigitalClockContainer::updateIntervalInMilliseconds()
{
    return 1000;
}

/*!
    @copydoc SnsrBigClockContainer::loadWidgets()
 */
void SnsrDigitalClockContainer::loadWidgets()
{
    // reset widget pointers, any previous widgets are already deleted by now
    mMainView = 0;
    mDateLabel = 0;
    mTimeLabel = 0;
    mAmPmLabel = 0;
    mIndicatorWidget = 0;
    
    // load widgets from docml
    bool ok(false);
    qDebug() << gDigitalLayoutDocml;
    mDocumentObjects = mDocumentLoader.load(gDigitalLayoutDocml, &ok);
    Q_ASSERT_X(ok, gDigitalLayoutDocml, "Invalid DocML file.");

    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mDateLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gDateLabelName));
        mTimeLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gTimeLabelName));
        mAmPmLabel = qobject_cast<SnsrLabel *>(
                mDocumentLoader.findWidget(gAmPmLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
                mDocumentLoader.findWidget(gIndicatorWidgetName));
        // implement swipewidget later on
        /*mSwipeWidget = qobject_cast<SnsrSwipeWidget *>(
                mDocumentLoader.findWidget(gSwipeWidgetName));*/

        Q_ASSERT_X(
                mMainView && mDateLabel && mTimeLabel && mAmPmLabel &&
                mIndicatorWidget /*&& mSwipeWidget*/,
                gDigitalLayoutDocml, "Objects not found in DocML file."
                );

        // In case of landscape layout, read also the landscape delta section
        if ( mCurrentOrientation == Qt::Horizontal ) {
            qDebug() << "loading: " << gDigitalLayoutDocml << ", section: " << gLandscapeSectionName;
            mDocumentLoader.load(gDigitalLayoutDocml, gLandscapeSectionName, &ok);
            Q_ASSERT_X(ok, gDigitalLayoutDocml, "Invalid section in DocML file.");
        }

        mIndicatorWidget->setLayoutType(SnsrIndicatorWidget::IndicatorsCentered);
        mIndicatorWidget->setPowerSaveModeColor(false);
        initIndicatorWidget();
        
        mBackgroundContainerLayout->addItem(mMainView);

        //connect( mSwipeWidget, SIGNAL(swipeDownDetected()), SIGNAL(unlockRequested()) );
        //mSwipeWidget->start();
    }
}

