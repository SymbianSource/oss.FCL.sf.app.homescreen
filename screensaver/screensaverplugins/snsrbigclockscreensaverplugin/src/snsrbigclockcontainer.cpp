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
* Description:  Base class container.
*
*/

#include "snsrbigclockcontainer.h"

#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QGraphicsLinearLayout>

#include <hblabel.h>
#include <hbevent.h>
#include <hbcolorscheme.h>

#include <qsysteminfo.h>

#include "snsrbigclockcontainer.h"
#include "snsrindicatorwidget.h"
#include "snsrindicatormodel.h"

/*!
    \class SnsrBigClockContainer
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Base class. Container used for drawing background and for preparing layout.
 */

const QString snsrBackgroundColorRole("snsrbackground");
const int gStep(5);

QTM_USE_NAMESPACE

/*!
    Constructs a new SnsrBigClockContainer.
 */
SnsrBigClockContainer::SnsrBigClockContainer() :
    mBackgroundContainerLayout(0),
    mMainView(0),
    mMainContainer(0),
    mIndicatorModel(0),
    mIndicatorWidget(0),
    mCurrentOrientation(-1)
{
    setBackgroundColor();
    mBackgroundContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    setLayout(mBackgroundContainerLayout);
    qsrand(QTime::currentTime().msec());
}

/*!
    Destructs the class.
 */
SnsrBigClockContainer::~SnsrBigClockContainer()
{
    resetIndicatorConnections();
    
    // e.g. mIndicatorWidget gets deleted during these calls
    mDocumentLoader.reset();
    qDeleteAll(mDocumentObjects);
    mDocumentObjects.clear();

    //mMainContainer,mBackgroundContainerLayout - deleted by the parent
    mIndicatorModel=0; // not owned
}

/*!
    \fn virtual void update() = 0;

    Slot for members update in container e.g. when time or date changed.
 */

/*!
    \fn virtual void changeLayout(Qt::Orientation orientation) = 0;

    Slot for members update in container when orientation changed.
 */

/*!
    \fn virtual int updateIntervalInMilliseconds() = 0;

    Concrete inherited container classes must implement this to return
    the desired update interval for that clock mode.
 */

/*!
    Set used indicator model and do necessary initializations to show currently
    active indicators.
 */
void SnsrBigClockContainer::initIndicators(SnsrIndicatorModel &model)
{
    mIndicatorModel = &model;
    if (mIndicatorWidget) {
        connect(mIndicatorModel, SIGNAL(indicatorsUpdated(QList<SnsrIndicatorInfo>)),
                mIndicatorWidget, SLOT(showIndicators(QList<SnsrIndicatorInfo>)));
                
        connect(mIndicatorModel, SIGNAL(allIndicatorsDeactivated()),
                mIndicatorWidget, SLOT(removeAllIndicators()));
        
        mIndicatorModel->initializeIndicatorWidget();
    }
}

/*!
    \reimp
 */
void SnsrBigClockContainer::changeEvent(QEvent *event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        setBackgroundColor();
    }
    return QGraphicsWidget::changeEvent(event);
}

/*!
    Returns random point for given range.
    \param rect Area within which the generated point will be.
 */
QPointF SnsrBigClockContainer::randomPosition(const QRectF &rect)
{
    int width( rect.width() );
    int height( rect.height() );
    if ( width > 0 && height > 0 ) {
        return rect.topLeft() + QPointF( qrand()%width, qrand()%height );
    }
    else {
        return QPointF();
    }

}

/*!
    Returns new position between curRect position and destPos position.
    \param curPos Current position.
    \param destPos Destination position. When current position is near this
                   position or outside of the container, a new value is generated.
    \param containerRect The container within which the destination position will always
                         be after a call.
 */
QPointF SnsrBigClockContainer::nextRandomPosition(const QPointF &curPos, QPointF &destPos, const QRectF &containerRect)
{
    const int delta(gStep+2);
    const int minCntDimension(3*delta);

    // The random movement logic can work only if the container has enough space to move the
    // clock around. If the container is too small, just return the middle point of the container.
    if ( containerRect.width() < minCntDimension && containerRect.height() < minCntDimension ) {
        return containerRect.center();
    }
    
    int xDistance = abs( destPos.x() - curPos.x() );
    int yDistance = abs( destPos.y() - curPos.y() );
    
    // Generate new destination position when current widget position is close to 
    // destination random position or when current destination position is out of bounds.
    // It is possible that the new random position is very close to the current position,
    // in which case the random position is generated again.
    // It is paramount that container is large enough when next loop is entered
    // to prevent infinite looping.
    while ( (xDistance < delta && yDistance < delta)
            || !containerRect.contains(destPos) ) {
        destPos = randomPosition( containerRect );
        xDistance = abs( destPos.x() - curPos.x() );
        yDistance = abs( destPos.y() - curPos.y() );
    }

    // If, for some reason, the current position is out-of-bounds, then there's no
    // point to slowly move towards the destination. In that case, move immediately
    // to destination point.
    if ( !containerRect.contains(curPos) ) {
        return destPos;
    }
    // Otherwise, this is normal case and we will calculate a point which is just
    // a bit closer to the destination.
    
    // Version 1:
    //  o-----o---------------------------o
    //  p1    p2                          p3
    //
    // Version 2:
    //  o---------------------------o-----o
    //  p3                          p2    p1
    //
    // p1 - current widget position
    // p2 - next position to compute
    // p3 - destination random position
    QPointF p1(curPos);
    QPointF p2(0,0);
    QPointF p3(destPos);

    // Computes point p2 - new position between p1 and p3

    // Move the coordinate which is further away from the destination
    // and calculate the other coordinate from that so that the
    // result point p2 lies on the straigth line between p1 and p3.
    if ( yDistance > xDistance ) {
        if (p3.y() > p1.y()) {
            p2.setY(p1.y()+gStep);
        }
        else {
            p2.setY(p1.y()-gStep);
        }
        p2.setX((((p2.y()-p1.y())*(p3.x()-p1.x())) / (p3.y()-p1.y())) + p1.x()); // x2 = (((y2-y1)*(x3-x1)) / (y3-y1)) + x1
    }
    else {
        if (p3.x() > p1.x()) {
            p2.setX(p1.x()+gStep);
        }
        else {
            p2.setX(p1.x()-gStep);
        }
        p2.setY((((p3.y()-p1.y())*(p2.x()-p1.x())) / (p3.x()-p1.x())) + p1.y()); // y2 = (((y3-y1)*(x2-x1)) / (x3-x1)) + y1
    }

    // Return new position between points p1 and p3.
    return p2;
}

/*!
    Disconnect connections between indicator model and widget.
 */
void SnsrBigClockContainer::resetIndicatorConnections()
{
    disconnect(mIndicatorModel, SIGNAL(indicatorsUpdated(QList<SnsrIndicatorInfo>)),
               mIndicatorWidget, SLOT(showIndicators(QList<SnsrIndicatorInfo>)));
    
    disconnect(mIndicatorModel, SIGNAL(allIndicatorsDeactivated()),
               mIndicatorWidget, SLOT(removeAllIndicators()));
}

/*!
    Set background color.
 */
void SnsrBigClockContainer::setBackgroundColor()
{
    QColor backgroundColor = HbColorScheme::color(snsrBackgroundColorRole);
    if (backgroundColor.isValid()) {
        mBackgroundColor = backgroundColor;
    }
    else {
        mBackgroundColor = Qt::black;
    }
}

/*!
    Paints the contents of an item in local coordinates.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void SnsrBigClockContainer::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
        )
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->fillRect(rect(), mBackgroundColor);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

