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
 * Description: Menu view.
 *
 */

#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include "hsmainwindow.h"
#include "hsmenuview.h"


/*!
 Constructor
 */



/*
 * A word about HsMainWindow class.
 * This is not an utility class. It was designed to isolate HbMainWindow connected code, so
 * it can be substitued in MT for application library. Unfortunately it seems to be the best
 * way to cope with HbMainWindow problem in MT.  
 */


HsMainWindow::HsMainWindow() 
{
}

/*!
 Destructor
 */
HsMainWindow::~HsMainWindow()
{
}

/*!
 Add the \menuView to first instance of HbMainWindow registered in HbInstance.
 \param view View to be added to HbMainWindow.
*/
void HsMainWindow::setCurrentView(HbView *view)
{
    HbMainWindow *const hbW(
        HbInstance::instance()->allMainWindows().value(0));
    
    if (!hbW->views().contains(view)) {
        hbW->addView(view);
    }
    hbW->setCurrentView(view);
}
