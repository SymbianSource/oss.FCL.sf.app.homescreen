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
void HsMainWindow::setCurrentView(const HsMenuView &menuView)
{
    HbMainWindow *const hbW(
        HbInstance::instance()->allMainWindows().value(0));

    HbView *const view = menuView.view();
    
    if (!hbW->views().contains(view)) {
        hbW->addView(view);
    }
    if (hbW->currentView() && !hbW->currentView()->isContentFullScreen()) {
        // quick trick to know if we are not comming from IDLE
        hbW->setCurrentView(view, true, Hb::ViewSwitchUseAltEvent);
    } else {
        hbW->setCurrentView(view);
    }    
}

