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
* Description: Screensaver application.
*
*/

#include <hbapplication.h>

#include "snsrscreensaver.h"
#include "snsrtest_global.h"

int main(int argc, char *argv[])
{
    SCREENSAVER_TEST_FUNC_ENTRY("main")
    
    HbApplication scsrApp(argc, argv);
    scsrApp.setApplicationName("Screensaver");
    SnsrScreensaver scsr;
    QObject::connect(&scsrApp, SIGNAL(aboutToQuit()), &scsr, SLOT(stop()));
    QObject::connect(&scsr, SIGNAL(exit()), &scsrApp, SLOT(quit()), Qt::QueuedConnection);
    scsr.start();
    int ret = scsrApp.exec();
    
    SCREENSAVER_TEST_FUNC_EXIT("main")
    return ret;
}
