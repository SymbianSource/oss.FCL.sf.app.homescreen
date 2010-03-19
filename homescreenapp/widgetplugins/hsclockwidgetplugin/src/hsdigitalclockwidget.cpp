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
* Description:  Homescreen themable clock label.
*
*/

#include "hsdigitalclockwidget.h"

#include <hbevent.h>
#include <hbcolorscheme.h>

/*!
    \class HsDigitalClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver label.
 */

/*!
    Constructs a new HsDigitalClockWidget.
 */
HsDigitalClockWidget::HsDigitalClockWidget(QGraphicsItem *parent)
    : HbLabel(parent) 
{
}

/*!
    Destructs the class.
 */
HsDigitalClockWidget::~HsDigitalClockWidget()
{
}

/*!
    \reimp
 */
void HsDigitalClockWidget::changeEvent(QEvent * event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        const QString textColorAttribute("text");
        QColor textColor(HbColorScheme::color(textColorAttribute));
        if (textColor.isValid()) {
            setTextColor(textColor);
        }
    }
    return HbLabel::changeEvent(event);
}
