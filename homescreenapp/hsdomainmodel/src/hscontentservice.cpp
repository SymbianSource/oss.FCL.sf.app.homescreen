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


/*!
    \class HsContentService
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsContentService::HsContentService(QObject *parent)
    : QObject(parent),mWidgetStartFaulted(false)
{
   
}

/*!

*/
HsContentService::~HsContentService()
{
}

/*!

*/
bool HsContentService::createWidget(const QVariantHash &params)
{
    return addWidget(params.value("uri").toString(),params.value("preferences").toHash());
}

// This method will be removed.
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
HsWidgetHost *HsContentService::createWidgetForPreview(const QVariantHash &params)
{
    HsWidgetData widgetData;
    widgetData.uri = params.value("uri").toString();

    return HsWidgetHost::createInstance(
        widgetData, params.value("preferences").toHash());
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

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
    connect(widget,SIGNAL(event_faulted()),SLOT(widgetStartFaulted()));
    mWidgetStartFaulted = false; 
    widget->startWidget(); // synchronous operation
    if (mWidgetStartFaulted) {
        // page will destroy widget instance
        return false;
    }
    widget->disconnect(this);
  
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

*/
void HsContentService::widgetStartFaulted()
{
    mWidgetStartFaulted = true;
}

/*!
    Points to the content service instance.
*/
HsContentService *HsContentService::mInstance(0);
