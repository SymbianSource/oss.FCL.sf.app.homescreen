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
* Description:  Main class for HsHomeScreenClientServiceProvider library.
*
*/

#include "hshomescreenclientserviceprovider.h"

#include "hsscene.h"
#include "hspage.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"

const char INTERFACE_NAME[] = "com.nokia.services.hsapplication.IHomeScreenClient";

HsHomeScreenClientServiceProvider::HsHomeScreenClientServiceProvider(QObject* parent)
: XQServiceProvider(INTERFACE_NAME,parent)
{
    publishAll();
}

HsHomeScreenClientServiceProvider::~HsHomeScreenClientServiceProvider()
{
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

// this should be moved to HsContentService class 

bool HsHomeScreenClientServiceProvider::addWidget(const QString &uri,const QVariantHash &preferences)
{

    HsWidgetData widgetData;
    widgetData.uri = uri;

    QScopedPointer<HsWidgetHost> widget(HsWidgetHost::createInstance(widgetData,preferences));

    if (!widget.data()) {
        return false;
    }
    HsPage* activePage = HsScene::instance()->activePage();
    if (!widget->load() || !activePage->addNewWidget(widget.data())) {
        widget->deleteFromDatabase();
        return false;
    }
    HsWidgetHost* taken = widget.take();
    taken->initializeWidget();
    taken->showWidget();
    activePage->layoutNewWidgets();

    return true;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

