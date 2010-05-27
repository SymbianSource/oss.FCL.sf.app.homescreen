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
    Constructor
*/
HsGui::HsGui()	
{    
}

/*!
    Destructor.
*/
HsGui::~HsGui()
{
}

/*!
    Returns idle view. 
*/
HbView *HsGui::idleView()
{
	return mIdleView.data();    
}

/*!
    Returns the current idle view instance. Callers of this 
    function take ownership of the instance. The current 
    view instance will be reset to null.    
*/
HbView *HsGui::takeIdleView()
{	
    HbView *view = mIdleView;
    mIdleView = 0;
    return view;
}

/*!
    Sets the idle view instance. The existing instance
    will be deleted.
*/
void HsGui::setIdleView(HbView *idleView)
{
    if (mIdleView != idleView) {
        HbView *oldView = mIdleView;
        mIdleView = idleView;
        delete oldView;
    }
}

/*!
    Return main window.
*/
HbMainWindow *HsGui::mainWindow()
{
    return hbInstance->allMainWindows().first();
}

/*!
    Points to the view instance.
*/
QPointer<HbView> HsGui::mIdleView(0);
