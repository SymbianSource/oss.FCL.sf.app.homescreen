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
* Description:
*
*/
#include <QTranslator>
#include <QLocale>

#include <hbapplication.h>
#include <hbmainwindow.h>

#include "tstaskswitcher.h"

int main(int argc, char *argv[])
{
    HbApplication tsApp(argc, argv);
    tsApp.setApplicationName("Task Switcher");

    HbMainWindow mainWindow(NULL, Hb::WindowFlagTransparent);
    mainWindow.hideItems(Hb::AllItems);    
    
    QString locale = QLocale::system().name();
    QTranslator translator;

    QString path = "resource/qt/translations/";
    QString tsTrFile = QString("taskswitcher_") + locale;
#ifdef Q_OS_SYMBIAN
    // TRAP is must here, otherwise it crashes
    TRAP_IGNORE(
        bool tsLoaded(false);
        tsLoaded = translator.load(tsTrFile, QString("z:/") + path);
        if (!tsLoaded)
        translator.load(tsTrFile, QString("c:/") + path);
    );
#else
    translator.load(tsTrFile, QString(path));
#endif //Q_OS_SYMBIAN  
    tsApp.installTranslator(&translator);

    TsTaskSwitcher ts;
    QObject::connect(&tsApp, SIGNAL(aboutToQuit()), &ts, SLOT(stop()));
    QObject::connect(&ts, SIGNAL(exit()), &tsApp, SLOT(quit()), Qt::QueuedConnection);
    ts.start();

    return tsApp.exec();
}
