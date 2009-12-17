/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Declares panic codes for widgetmanager app.
*
*/

#ifndef WIDGETMANAGER_PAN_H
#define WIDGETMANAGER_PAN_H

/** WidgetManager application panic codes */
enum TWidgetManagerPanics
	{
	EWidgetManagerUi = 1
	// add further panics here
	};

inline void Panic(TWidgetManagerPanics aReason)
	{
	_LIT(applicationName,"WidgetManager");
	User::Panic(applicationName, aReason);
	}

#endif // WIDGETMANAGER_PAN_H
