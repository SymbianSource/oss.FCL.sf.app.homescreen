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
* Description:  Container for oled digital clock.
*
*/

#include "snsroleddigitalclockcontainer.h"

#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <hblabel.h>
#include <hbextendedlocale.h>

/*!
    \class SnsrOledDigitalClockContainer
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Container used for preparing layout for oled digital clock.
 */

const char *gOledDigitalLayoutDocml = ":/xml/snsrbigclockscreensaveroleddigital.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;

extern const char *gTimeLabelName;
extern const char *gAmPmLabelName;
extern const char *gDateLabelName;

extern const char *gDateFormatVerticalStr;
extern const char *gDateFormatHorizontalStr;

extern const char *gTimeFormatStr;
extern const char *gAmPmFormatStr;

/*!
    Constructs a new SnsrOledDigitalClockContainer.
 */
SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer() :
    SnsrBigClockContainer(),
    mTimeLabel(0),
    mAmPmLabel(0),
    mDateLabel(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")

    bool ok(true);
    qDebug() << gOledDigitalLayoutDocml;

    // load digital clock
    mDocumentObjects = mDocumentLoader.load(gOledDigitalLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid DocML file.");

    if (ok) {
        mMainContainer = mDocumentLoader.findWidget(gMainViewName);
        mDateLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gDateLabelName));
        mTimeLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gTimeLabelName));
        mAmPmLabel = qobject_cast<HbLabel *>(
                mDocumentLoader.findWidget(gAmPmLabelName));
        Q_ASSERT_X(
                mMainContainer && mDateLabel && mTimeLabel && mAmPmLabel,
                gOledDigitalLayoutDocml, "Objects not found in DocML file."
                );

        mBackgroundContainerLayout->addItem(mMainContainer);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::SnsrOledDigitalClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledDigitalClockContainer::~SnsrOledDigitalClockContainer()
{
    //mTimeLabel, mAmPmLabel, mDateLabel - deleted by the parent
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
            HbExtendedLocale().format(QDate::currentDate(), gDateFormatVerticalStr)
            );
    } else {
        mDateLabel->setPlainText(
            HbExtendedLocale().format(QDate::currentDate(), gDateFormatHorizontalStr)
            );
    }

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
        bool ok;
        // hide controls to avoid screen flickering
        mMainContainer->hide();

        QString sectionToLoad("");
        if (mCurrentOrientation == Qt::Vertical) {
            sectionToLoad = gPortraitSectionName;
        } else {
            sectionToLoad = gLandscapeSectionName;
        }
        qDebug() << "loading: " << gOledDigitalLayoutDocml << ", section: " << sectionToLoad;
        mDocumentLoader.load(gOledDigitalLayoutDocml, sectionToLoad, &ok);
        // view is rebuilt and ready to show
        mMainContainer->show();
        Q_ASSERT_X(ok, gOledDigitalLayoutDocml, "Invalid section in DocML file.");
    }
    // update anyway - this is needed in situations when screensaver goes to
    // foreground but layout change did not occur
    update();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledDigitalClockContainer::changeLayout")
}
