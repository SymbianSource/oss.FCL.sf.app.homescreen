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
* Description:  Container for oled analog clock.
*
*/

#include "snsroledanalogclockcontainer.h"

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QTimer>
#include <QTime>
#include <hblabel.h>
#include <hbextendedlocale.h>

#include "snsroledclockwidget.h"
#include "snsrindicatorwidget.h"

/*!
    \class SnsrOledAnalogClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Container used for preparing layout for oled analog clock.
 */

const char *gOledAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveroledanalog.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;
extern const char *gMainContainerName;
extern const char *gClockContainerName;
extern const char *gIndicatorWidgetName;

extern const char *gDateLabelName;
extern const char *gDateFormatVerticalStr;
extern const char *gDateFormatHorizontalStr;

const char *gOledAnalogClockWidgetName = "oledAnalogClockWidget";


/*!
    Constructs a new SnsrOledAnalogClockContainer.
 */
SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer() :
    SnsrBigClockContainer(), mClockContainer(0), mOledClockWidget(0),
    mDateLabel(0), mDestPosition(QPointF()), mInitialize(false)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")

    bool ok(false);
    qDebug() << gOledAnalogLayoutDocml;

    // load oled analog clock
    mDocumentObjects = mDocumentLoader.load(gOledAnalogLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid DocML file.");
    if (ok) {
        mMainView = mDocumentLoader.findWidget(gMainViewName);
        mMainContainer = mDocumentLoader.findWidget(gMainContainerName);
        mClockContainer = mDocumentLoader.findWidget(gClockContainerName);
        mOledClockWidget = qobject_cast<SnsrOledClockWidget *>(
              mDocumentLoader.findWidget(gOledAnalogClockWidgetName));
        mDateLabel = qobject_cast<HbLabel *>(
              mDocumentLoader.findWidget(gDateLabelName));
        mIndicatorWidget = qobject_cast<SnsrIndicatorWidget *>(
              mDocumentLoader.findWidget(gIndicatorWidgetName));
        Q_ASSERT_X(
                mMainView && mMainContainer && mClockContainer
                && mOledClockWidget && mDateLabel && mIndicatorWidget,
                gOledAnalogLayoutDocml, "Objects not found in DocML file."
                );
        
        mIndicatorWidget->setLayoutType(SnsrIndicatorWidget::IndicatorsCentered);
        
        mBackgroundContainerLayout->addItem(mMainView);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledAnalogClockContainer::~SnsrOledAnalogClockContainer()
{
    //mOledClockWidget etc - deleted by the parent
}

/*!
    Updates OLED analog clock widget position.
 */
void SnsrOledAnalogClockContainer::updatePosition()
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
    Updates displayed time.
 */
void SnsrOledAnalogClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::update")

    // time
    mOledClockWidget->tick();
    
    // date
    if (mCurrentOrientation == Qt::Vertical) {
        mDateLabel->setPlainText(
            HbExtendedLocale().format(QDate::currentDate(), gDateFormatVerticalStr)
        );
    } else {
        mDateLabel->setPlainText(
            HbExtendedLocale().format(QDate::currentDate(), gDateFormatHorizontalStr)
        );
    }
    
    // position
    updatePosition();
    
    // TODO: Currently, both time and position are updated 10 times per second.
    // These should happen only once per minute in the power save mode.

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::update")
}

/*!
    Changes screensaver layout basing on orientation changes.
    \param orientation Current orientation.
 */
void SnsrOledAnalogClockContainer::changeLayout(Qt::Orientation orientation)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::changeLayout")

    bool ok(false);
    if (mCurrentOrientation != orientation) {
        mCurrentOrientation = orientation;
        // hide controls to avoid screen flickering
        mMainView->hide();

        QString sectionToLoad("");
        if (mCurrentOrientation == Qt::Horizontal) {
            sectionToLoad = gLandscapeSectionName;
        }
        qDebug() << "loading: " << gOledAnalogLayoutDocml << ", section: " << sectionToLoad;
        mDocumentLoader.load(gOledAnalogLayoutDocml, sectionToLoad, &ok);
        Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid section in DocML file.");

        // disconnect container from parent layout,
        // connected container resets its position to the one defined in docml
        // after label text updates
        mClockContainer->setParentLayoutItem(0);

        update();

        // view is rebuilt and ready to show
        mMainView->show();
    }

    // update anyway - this is needed in situations when screensaver goes to
    // foreground but layout change did not occur
    if (!ok) {
        update();
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::changeLayout")
}

int SnsrOledAnalogClockContainer::updateIntervalInMilliseconds()
{
    return 60*1000;
}

// end of file
