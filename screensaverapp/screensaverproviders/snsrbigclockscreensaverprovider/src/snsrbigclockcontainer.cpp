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
* Description:  Base class container.
*
*/

#include "snsrbigclockcontainer.h"

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif // Q_OS_SYMBIAN

#include <QPainter>
#include <QDebug>
#include <QGraphicsLinearLayout>

#include <hbevent.h>
#include <hbcolorscheme.h>

#include "snsrbigclockcontainer.h"

/*!
    \class SnsrBigClockContainer
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Base class. Container used for drawing background and for preparing layout.
 */

const QString popupBackgroundColorRole("popupbackground");
const QString snsrBackgroundColorRole("snsrbackground");

/*!
    Constructs a new SnsrBigClockContainer.
 */
SnsrBigClockContainer::SnsrBigClockContainer() :
    mMainContainer(0),
    mBackgroundContainerLayout(0),
    mCurrentOrientation(-1)
{
    setBackgroundColor();
    mBackgroundContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    setLayout(mBackgroundContainerLayout);
}

/*!
    Destructs the class.
 */
SnsrBigClockContainer::~SnsrBigClockContainer()
{
    mDocumentLoader.reset();
    qDeleteAll(mDocumentObjects);
    mDocumentObjects.clear();

    //mMainContainer, mBackgroundContainerLayout - deleted by the parent
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
    Set background color.
 */
void SnsrBigClockContainer::setBackgroundColor()
{
    QColor backgroundColor = HbColorScheme::color(snsrBackgroundColorRole);
    if (backgroundColor.isValid()) {
        mBackgroundColor = backgroundColor;
    }
    else {
        backgroundColor = HbColorScheme::color(popupBackgroundColorRole);
        if (backgroundColor.isValid()) {
            mBackgroundColor = backgroundColor;
        }
        else {
            mBackgroundColor = Qt::black;
        }
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

