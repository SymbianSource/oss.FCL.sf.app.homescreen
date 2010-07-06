/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Swipe Widget.
*
*/

#include <QDebug>
#include <HbStyleLoader>
#include <hbeffect.h>
#include <hblabel.h>

#include "snsrbigclockcontainer.h"
#include "snsranalogclockcontainer.h"
#include "snsrswipewidget.h"

const char *gSwipeCssFilePath =":/style/snsrswipewidget.css";
const char *gSwipeWidgetMLFilePath = ":/style/snsrswipewidget.widgetml";
const char *gSwipeColorCssFilePath = ":/style/snsrswipewidget_color.css";

const qreal gSwipeDownAngle = 270;
const qreal gSwipeAngleTolerance = 25;

/*!
    \class SnsrSwipeWidget
    \ingroup group_snsrbigclockscreensaverplugins
    \brief Screensaver swipe widget.
 */

/*!
    Constructs a new SnsrSwipeWidget.
    \param parent Parent object.
 */
SnsrSwipeWidget::SnsrSwipeWidget(QGraphicsItem* parent):
    HbWidget(parent),
    mSlideLabel(0), mSlideIcon(0)
{
    HbStyleLoader::registerFilePath(gSwipeCssFilePath);
    HbStyleLoader::registerFilePath(gSwipeWidgetMLFilePath);
    HbStyleLoader::registerFilePath(gSwipeColorCssFilePath);
    
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    createPrimitives();

    HbEffect::add(mSlideIcon,
        ":/xml/animate_portrait.fxml",
        "slideDownPortrait");

    reset();
    grabGesture(Qt::SwipeGesture);

}

/*!
    Destructs the class.
 */
SnsrSwipeWidget::~SnsrSwipeWidget()
{
    HbStyleLoader::unregisterFilePath( gSwipeCssFilePath );
    HbStyleLoader::unregisterFilePath( gSwipeWidgetMLFilePath );
    HbStyleLoader::unregisterFilePath( gSwipeColorCssFilePath );

    HbEffect::remove(mSlideIcon);
}

void SnsrSwipeWidget::start()
{
    // Start animation
    HbEffect::start(mSlideIcon, "slideDownPortrait", this, "animationComplete");
}

void SnsrSwipeWidget::reset()
{
    // Cancel animation and disable callback
    HbEffect::cancel(mSlideIcon, "slideDownPortrait", false, false);
}

void SnsrSwipeWidget::animationComplete(const HbEffect::EffectStatus &status)
{
    Q_UNUSED(status);
    HbEffect::start(mSlideIcon, "slideDownPortrait", this, "animationComplete");
}


/*!
    Creates all widget primitives.
 */
void SnsrSwipeWidget::createPrimitives()
{
    if (!mSlideLabel) {
        mSlideLabel = new HbLabel(this);
        mSlideLabel->setPlainText(hbTrId("txt_screensaver_swipe"));
        mSlideLabel->setAlignment(Qt::AlignCenter);
        mSlideLabel->setPreferredHeight(50.0);
        mSlideLabel->setPreferredWidth(224.0);
        HbStyle::setItemName(mSlideLabel, QLatin1String("slideLabel"));
    }
    if (!mSlideIcon) {
        mSlideIcon = new HbLabel(QLatin1String("slider"), this);
        mSlideIcon->setIcon(HbIcon(":/slider"));   
        mSlideIcon->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        mSlideIcon->setAlignment(Qt::AlignCenter);
        mSlideIcon->setPreferredHeight(50.0);
        mSlideIcon->setPreferredWidth(100.0);
        HbStyle::setItemName(mSlideIcon, QLatin1String("slideIcon"));
    }
}

/*!
    \reimp
 */

void SnsrSwipeWidget::gestureEvent(QGestureEvent *event)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrSwipeWidget::gestureEvent")

    QGesture *gesture = event->gesture(Qt::SwipeGesture);
    QSwipeGesture *swipe = static_cast<QSwipeGesture *>(gesture);

    if ( swipe && swipe->state() == Qt::GestureStarted ) {
        start();
        event->accept(Qt::SwipeGesture);
    }
    else if ( swipe && swipe->state() == Qt::GestureFinished ) {
        // TODO: Remove following, currently shows some debug data on screen
        mSlideLabel->setPlainText( QString("angle=%1, horD=%2, verD=%3 ")
                .arg(swipe->swipeAngle())
                .arg(swipe->horizontalDirection())
                .arg(swipe->verticalDirection()) );
        
        // unclock with downward swipe
        qreal swipeAngle = swipe->swipeAngle();
        if ( abs(swipeAngle - gSwipeDownAngle) < gSwipeAngleTolerance ) {
            
            emit swipeDownDetected();
            event->accept(Qt::SwipeGesture);
            }
    }
    else { event->ignore(Qt::SwipeGesture); }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrSwipeWidget::gestureEvent")
}
