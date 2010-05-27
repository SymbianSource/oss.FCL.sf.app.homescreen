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

#ifndef HSVIEW_H
#define HSVIEW_H

#include <QPointer>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HbView;
class HbMainWindow;

class HSDOMAINMODEL_EXPORT HsGui
{
public:
	HsGui();
	~HsGui();
	static HbView *idleView();	
	static HbView *takeIdleView();
	static void setIdleView(HbView *idleView);	
	static HbMainWindow *mainWindow();

private:
	static QPointer<HbView> mIdleView;
	HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

Q_DECLARE_METATYPE(HsGui*)
#endif //HSVIEW_H
