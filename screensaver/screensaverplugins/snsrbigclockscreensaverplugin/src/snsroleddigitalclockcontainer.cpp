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
* Description:  Container for oled digital clock.
*
*/

#include "snsroleddigitalclockcontainer.h"

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QTime>
#include <QTimer>

#include <hblabel.h>
#include <hbextendedlocale.h>

#include "snsrindicatorwidget.h"

/*!
    \class SnsrOledDigitalClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for oled digital clock.
 */

const char *gOledDigitalLayoutDocml = ":/xml/snsrbigclockscreensaveroleddigital.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;
extern const char *gMainContainerName;
extern const char *gClockContainerName;

extern const char *gTimeLabelName;
extern const char *gAmPmLabelName;
extern const char *gDateLabelName;

extern const char *gDateFormatVerticalStr;
extern const char *gDateFormatHorizontalStr;

extern const char *gTimeFormatStr;
extern const char *gAmPmFormatStr;

extern const char *gIndicatorWidgetName;


/*!
    Constructs a new SnsrOledDigitalClockContainer.
 */
SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer() :
    SnsrBigClockContainer(), mClockContainer(0), mTimeLabel(0),
    mDateLabel(0), mAmPmLabel(0), mDestPosition(QPointF()), mInitialize(false)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")

    bool ok(false);
    qDebug() << gOledDigitalLayoutDocml;

    // load digital clock
    mDocumentObjects = mDocumentLoader.load(gOledDigitalLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid DocML file.");

    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mMainContainer = mDocumentLoader.findWidget(gMainContainerName);
        mClockContainer = mDocumentLoader.findWidget(gClockContainerName);
        mDateLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gDateLabelName));
        mTimeLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gTimeLabelName));
        mAmPmLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gAmPmLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
                mDocumentLoader.findWidget(gIndicatorWidgetName));
        Q_ASSERT_X(
                mMainView && mMainContainer && mClockContainer 
                && mDateLabel && mTimeLabel && mAmPmLabel
                && mIndicatorWidget,
                gOledDigitalLayoutDocml, "Objects not found in DocML file."
                );

        mIndicatorWidget->setLayoutType(SnsrIndicatorWidget::IndicatorsCentered);
        
        mBackgroundContainerLayout->addItem(mMainView);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledDigitalClockContainer::~SnsrOledDigitalClockContainer()
{
    //mTimeLabel, mAmPmLabel, mDateLabel, mMoveTimer - deleted by the parent
}

/*!
    Updates OLED digital clock widget position.
 */
void SnsrOledDigitalClockContainer::updatePosition()
{
    QSizeF containerSize = mMainContainer->size();
    
    // Container must have a valid size to enable calculating the 
    // destination position for the clock.
    if ( containerSize.width() > 0 && containerSize.height() > 0 ) {
        containerSize -= mClockContainer->boundingRect().size();
        QRectF containerRect( mMainContainer->pos(), containerSize );
        if ( mInitialize ) {
            QPointF clockPos = nextRandomPosition( mClockContainer->pos(), mDestPosition, containerRect );
            mClockContainer->setPos( clockPos );
        }
        else {
            mDestPosition = randomPosition( containerRect );
            mInitialize = true;
        }
    }
}

/*!
    Updates displayed time and date.
 */
void SnsrOledDigitalClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::update")

    // time
    mTimeLabel->setPlainText(
        HbExtendedLocale().format(QTime::currentTime(), gTimeFormatStr)
        );
    mAmPmLabel->setPlainText(
        HbExtendedLocale().format(QTime::currentTime(), gAmPmFormatStr)
        );

    // date
    if (mCurrentOrientation == Qt::Vertical) {
        mDateLabel->setPlainText(
            HbExtendedLocale().format(
                QDate::currentDate(), gDateFormatVerticalStr)
            );
    } else {
        mDateLabel->setPlainText(
            HbExtendedLocale().format(
                QDate::currentDate(), gDateFormatHorizontalStr)
            );
    }

    updatePosition();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::update")
}

/*!
    Changes screensaver layout basing on orientation changes.
    \param orientation Current orientation.
 */
void SnsrOledDigitalClockContainer::changeLayout(Qt::Orientation orientation)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::changeLayout")

    if (mCurrentOrientation != orientation) {
        mCurrentOrientation = orientation;

        // hide controls to avoid screen flickering
        mMainView->hide();

        QString sectionToLoad("");
        if (mCurrentOrientation == Qt::Horizontal) {
            sectionToLoad = gLandscapeSectionName;
        }
        qDebug() << "loading: " << gOledDigitalLayoutDocml << ", section: "
            << sectionToLoad;
        bool ok(false);
        mDocumentLoader.load(gOledDigitalLayoutDocml, sectionToLoad, &ok);
        Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid section in DocML file.");

        // disconnect container from parent layout,
        // connected container resets its position to the one defined in docml
        // after label text updates
        mClockContainer->setParentLayoutItem(0);

        // update labels before showing the view
        update();

        // view is rebuilt and ready to show
        mMainView->show();
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::changeLayout")
}

int SnsrOledDigitalClockContainer::updateIntervalInMilliseconds()
{
    return 60*1000;
}

// end of file
