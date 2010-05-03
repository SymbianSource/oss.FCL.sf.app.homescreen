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
* Description:  Homescreen runtime content service.
*
*/

#include "hscontentservice.h"
#include "hsdatabase.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"


HsContentService::HsContentService(QObject *parent)
    : QObject(parent)
{
   
}

HsContentService::~HsContentService()
{
}


bool HsContentService::createWidget(const QVariantHash &params)
{
    HsWidgetHost *widget = createWidgetForPreview(params);
    if (!widget) {
        return false;
    }
     
    return HsScene::instance()->activePage()->addNewWidget(widget);
}

HsWidgetHost *HsContentService::createWidgetForPreview(const QVariantHash &params)
{
    HsWidgetData widgetData;
    widgetData.uri = params.value("uri").toString();


    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData,
                                                        params.value("preferences").toHash());

    if (!widget) {
        return NULL;
    }
    if (!widget->load()) {
        widget->deleteFromDatabase();
        delete widget;
        return NULL;
    }

    return widget;
}

/*!

*/
bool HsContentService::addWidget(const QString &uri, const QVariantHash &preferences)
{
    HsWidgetData widgetData;
    widgetData.uri = uri;

    QScopedPointer<HsWidgetHost> widget(HsWidgetHost::createInstance(widgetData, preferences));
    if (!widget.data()) {
        return false;
    }

    HsPage *activePage = HsScene::instance()->activePage();
    if (!widget->load() || !activePage->addNewWidget(widget.data())) {
        widget->deleteFromDatabase();
        return false;
    }

    HsWidgetHost *taken = widget.take();
    taken->initializeWidget();
    taken->showWidget();
    activePage->layoutNewWidgets();
    return true;
}

/*!

*/
HsContentService *HsContentService::instance()
{
    if (!mInstance) {
        mInstance.reset(new HsContentService);
    }
    return mInstance.data();
}

/*!
    Points to the content service instance.
*/
QScopedPointer<HsContentService> HsContentService::mInstance(0);
