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
* Description:  Screensaver label.
*
*/

#include "snsrlabel.h"

#include <hbevent.h>
#include <hbcolorscheme.h>

const QString snsrForegroundColorRole("snsrforeground");

/*!
    \class SnsrLabel
    \ingroup group_snsrutils
    \brief Screensaver label.
 */

/*!
    Constructs a new SnsrLabel.
    \param parent Graphics parent item.
 */
SnsrLabel::SnsrLabel(QGraphicsItem *parent)
    : HbLabel(parent)
{
    setThemedTextColor();
}

/*!
    Constructs a new SnsrLabel.
    \param displayText Text the label should display.
    \param parent Graphics parent item.
 */
SnsrLabel::SnsrLabel(const QString &displayText, QGraphicsItem *parent)
    : HbLabel(displayText, parent)
{
    setThemedTextColor();
}

/*!
    Destructs the class.
 */
SnsrLabel::~SnsrLabel()
{
}

/*!
    \reimp
 */
void SnsrLabel::changeEvent(QEvent * event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        setThemedTextColor();
    }
    return HbLabel::changeEvent(event);
}

/*!
    Sets the label's color to follow the theme.
 */
void SnsrLabel::setThemedTextColor()
{
    QColor textColor(HbColorScheme::color(snsrForegroundColorRole));
    if (textColor.isValid()) {
        setTextColor(textColor);
    } else {
        // fallback mechanism when color definition is missing in default theme
        setTextColor(Qt::white);
    }
}
