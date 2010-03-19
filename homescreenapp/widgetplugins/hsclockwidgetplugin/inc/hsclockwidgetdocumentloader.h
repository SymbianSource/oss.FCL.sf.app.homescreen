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
* Description: Screensaver custom document loader.
*
*/

#ifndef HSCLOCKWIDGETDOCUMENTLOADER_H
#define HSCLOCKWIDGETDOCUMENTLOADER_H

#include <hbdocumentloader.h>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HsClockWidgetDocumentLoader : public HbDocumentLoader
{

private:

    virtual QObject *createObject(const QString &type, const QString &name);

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
    
};

#endif // HSCLOCKWIDGETDOCUMENTLOADER_H
