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
* Description:  Container for oled analog clock.
*
*/

#include "snsroledanalogclockcontainer.h"

#include <QDebug>
#include <QGraphicsLinearLayout>

#include <hblabel.h>

#include "snsroledclockwidget.h"

/*!
    \class SnsrOledAnalogClockContainer
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Container used for preparing layout for oled analog clock.
 */

const char *gOledAnalogLayoutDocml = ":/xml/snsrbigclockscreensaveroledanalog.docml";
extern const char *gPortraitSectionName;
extern const char *gLandscapeSectionName;

extern const char *gMainViewName;

const char *gOledAnalogClockWidgetName = "oledAnalogClockWidget";

/*!
    Constructs a new SnsrOledAnalogClockContainer.
 */
SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer() :
    SnsrBigClockContainer(),
    mOledClockWidget(0)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")

    bool ok(true);

    // load oled analog clock
    qDebug() << gOledAnalogLayoutDocml;
    mDocumentObjects = mDocumentLoader.load(gOledAnalogLayoutDocml, &ok);
    Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid DocML file.");
    if (ok) {
        mMainContainer = mDocumentLoader.findWidget(gMainViewName);
        mOledClockWidget = qobject_cast<SnsrOledClockWidget *>(
                mDocumentLoader.findWidget(gOledAnalogClockWidgetName));
        Q_ASSERT_X(
                mMainContainer && mOledClockWidget, gOledAnalogLayoutDocml,
                "Objects not found in DocML file."
                );

        mBackgroundContainerLayout->addItem(mMainContainer);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::SnsrOledAnalogClockContainer")
}

/*!
    Destructs the class.
 */
SnsrOledAnalogClockContainer::~SnsrOledAnalogClockContainer()
{
    //mOledClockWidget - deleted by the parent
}

/*!
    Updates displayed time.
 */
void SnsrOledAnalogClockContainer::update()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::update")

    // time
    mOledClockWidget->tick();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::update")
}

/*!
    Changes screensaver layout basing on orientation changes.
    \param orientation Current orientation.
 */
void SnsrOledAnalogClockContainer::changeLayout(Qt::Orientation orientation)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrOledAnalogClockContainer::changeLayout")

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
        qDebug() << "loading: " << gOledAnalogLayoutDocml << ", section: " << sectionToLoad;
        mDocumentLoader.load(gOledAnalogLayoutDocml, sectionToLoad, &ok);
        // view is rebuilt and ready to show
        mMainContainer->show();
        Q_ASSERT_X(ok, gOledAnalogLayoutDocml, "Invalid section in DocML file.");
    }
    // update anyway - this is needed in situations when screensaver goes to
    // foreground but layout change did not occur
    update();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrOledAnalogClockContainer::changeLayout")
}
