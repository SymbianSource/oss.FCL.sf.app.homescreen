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
* Description:  Example of home screen content publishing client
*
*/

#include <HbApplication>
#include <HbMainWindow>
#include <HbView>
#include <HbPushButton>

#include <QDir>

#include "contentpublishclient.h"

int main(int argc, char *argv[])
{
	// Initialization
    HbApplication app(argc, argv);

    QString path = QDir::toNativeSeparators(QDir("z:\\hsresources\\plugins\\homescreenclientplugin\\").absolutePath());
    QString fullFileName = path + "\\hshomescreenclientplugin.xml";
    
    QServiceManager manager;
    
    if(QFile::exists(fullFileName)) {
        QCoreApplication::addLibraryPath(path);
        manager.addService(fullFileName);
    }
    
    // Create main window.
    HbMainWindow* mainWindow = new HbMainWindow();

    // Create content publisher client
	ContentPublishClient contentPublishClient(manager);
	
    QString buttonString = "Create widget";
    if(!contentPublishClient.load()) {
        buttonString = "Open failed";
    }

    HbPushButton* button = new HbPushButton(buttonString);

	// Add view
    mainWindow->addView(button);
    
    contentPublishClient.connect(button, SIGNAL(pressed()), SLOT(addWidget()));
    
    // Show main window
    mainWindow->show();

    return app.exec();
}
