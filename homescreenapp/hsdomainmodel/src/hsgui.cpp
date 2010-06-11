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
* Description:
*
*/

#include <HbInstance>
#include <HbView>

#include "hsgui.h"

/*!
    \class HsGui
    \ingroup group_hsutils
    \brief Represents a view in the framework.
    HsGui includes common UI components for Home screen.
*/

/*!
    Returns the idle view. 
*/
HbView *HsGui::idleView()
{
    return mIdleView;    
}

/*!
    Returns the current idle view instance. Callers of this 
    function take ownership of the instance. The current 
    view instance will be reset to null.    
*/
HbView *HsGui::takeIdleView()
{	
    HbView *idleView = mIdleView;
    mIdleView = 0;
    return idleView;
}

/*!
    Sets the idle view instance. The existing instance
    will be deleted.
*/
void HsGui::setIdleView(HbView *idleView)
{
    if (mIdleView != idleView) {
        delete mIdleView;
        mIdleView = idleView;
    }
}

/*!
    Returns the main window.
*/
HbMainWindow *HsGui::mainWindow()
{
    return hbInstance->allMainWindows().first();
}

/*!
    Points to the idle view instance.
*/
QPointer<HbView> HsGui::mIdleView(0);
