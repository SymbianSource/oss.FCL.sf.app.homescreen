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

#ifndef HSDIGITALCLOCKWIDGET_H
#define HSDIGITALCLOCKWIDGET_H

#include <hblabel.h>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HsDigitalClockWidget : public HbLabel
{
    Q_OBJECT
    
public:
    
    HsDigitalClockWidget(QGraphicsItem *parent = 0);
    ~HsDigitalClockWidget();
    
protected:

    virtual void changeEvent(QEvent *event);

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)

};

#endif // HSDIGITALCLOCKWIDGET_H
