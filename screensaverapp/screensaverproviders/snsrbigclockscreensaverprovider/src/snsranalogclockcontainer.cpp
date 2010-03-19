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
* Description:  Container for analog clock.
*
*/

#include "snsranalogclockcontainer.h"

#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <hblabel.h>
#include <hbextendedlocale.h>

#include "snsranalogclockwidget.h"

/*!
    \class SnsrAnalogClockContainer
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Container used for preparing layout for analog clock.
 */

const char *gAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveranalog.docml";

const char *gPortraitSectionName = "portrait";
const char *gLandscapeSectionName = "landscape";

const char *gMainViewName = "view";

const char *gDateLabelName = "dateLabel";
const char *gAnalogClockWidgetName = "analogClockWidget";

const char *gDateFormatVerticalStr = "%E%,% %*D%*N%/0%4%/1%5";
const char *gDateFormatHorizontalStr = "%E%,% %*D%N%/0%4%/1%5";

/*!
    Constructs a new SnsrAnalogClockContainer.
 */
SnsrAnalogClockContainer::SnsrAnalogClockContainer() :
    SnsrBigClockContainer(),
    mDateLabel(0),
    mAnalogClockWidget(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrAnalogClockContainer::SnsrAnalogClockContainer")

    bool ok(true);

    // load analog clock
    qDebug() << gAnalogLayoutDocml;
    mDocumentObjects = mDocumentLoader.load(gAnalogLayoutDocml, &ok);
    Q_ASSERT_X(ok, gAnalogLayoutDocml, "Invalid DocML file.");
    if (ok) {
        mMainContainer = mDocumentLoader.findWidget(gMainViewName);
        mDateLabel = qobject_cast<HbLabel *>(
            mDocumentLoader.findWidget(gDateLabelName));
        mAnalogClockWidget = qobject_cast<SnsrAnalogClockWidget *>(
            mDocumentLoader.findWidget(gAnalogClockWidgetName));
        Q_ASSERT_X(
                mMainContainer && mDateLabel && mAnalogClockWidget,
                gAnalogLayoutDocml, "Objects not found in DocML file."
                );

        mBackgroundContainerLayout->addItem(mMainContainer);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrAnalogClockContainer::SnsrAnalogClockContainer")
}

/*!
    Destructs the class.
 */
SnsrAnalogClockContainer::~SnsrAnalogClockContainer()
{
    //mDateLabel, mAnalogClockWidget - deleted by the parent
}

/*!
    Updates displayed time and date.
 */
void SnsrAnalogClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrAnalogClockContainer::update")

    // time
    mAnalogClockWidget->tick();

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

    SCREENSAVER_TEST_FUNC_EXIT("SnsrAnalogClockContainer::update")
}

/*!
    Changes screensaver layout basing on orientation changes.
    \param orientation Current orientation.
 */
void SnsrAnalogClockContainer::changeLayout(Qt::Orientation orientation)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrAnalogClockContainer::changeLayout")

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
        qDebug() << "loading: " << gAnalogLayoutDocml << ", section: " << sectionToLoad;
        mDocumentLoader.load(gAnalogLayoutDocml, sectionToLoad, &ok);
        // view is rebuilt and ready to show
        mMainContainer->show();
        Q_ASSERT_X(ok, gAnalogLayoutDocml, "Invalid section in DocML file.");
    }
    // update anyway - this is needed in situations when screensaver goes to
    // foreground but layout change did not occur
    update();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrAnalogClockContainer::changeLayout")
}
