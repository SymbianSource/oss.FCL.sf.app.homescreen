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

    return HsWidgetHost::createInstance(
        widgetData, params.value("preferences").toHash());
}

/*!

*/
bool HsContentService::addWidget(const QString &uri, const QVariantHash &preferences)
{
    HsWidgetData data;
    data.uri = uri;

    HsWidgetHost *widget = HsWidgetHost::createInstance(data, preferences);    
    if (!widget) {
        return false;
    }

    HsPage *page = HsScene::instance()->activePage();
    if (!page->addNewWidget(widget)) {
        widget->remove();
        return false;
    }

    widget->startWidget();
    page->layoutNewWidgets();
    return true;
}

/*!

*/
HsContentService *HsContentService::instance()
{
    if (!mInstance) {
        mInstance = new HsContentService();
    }
    return mInstance;
}

/*!
    Points to the content service instance.
*/
HsContentService *HsContentService::mInstance(0);
