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

#ifndef HSDOMAINMODELDATASTRUCTURES_H
#define HSDOMAINMODELDATASTRUCTURES_H

#include <QRectF>
#include "hsdomainmodel_global.h"

class HSDOMAINMODEL_EXPORT HsSceneData
{
public:
    HsSceneData() 
      : id(-1), defaultPageId(-1), maximumPageCount(-1),
        maximumWidgetHeight(-1.0), maximumWidgetWidth(-1.0),
        minimumWidgetHeight(-1.0), minimumWidgetWidth(-1.0)
    {}
    
    int     id;
    QString portraitWallpaper;
    QString landscapeWallpaper;
    int     defaultPageId;
    int     maximumPageCount;
    qreal   maximumWidgetHeight;
    qreal   maximumWidgetWidth;
    qreal   minimumWidgetHeight;
    qreal   minimumWidgetWidth;
};

class HSDOMAINMODEL_EXPORT HsPageData
{
public:
    HsPageData() 
      : id(-1), indexPosition(-1)
    {}
    
    int id;    
    int indexPosition;
};

class HSDOMAINMODEL_EXPORT HsWidgetData
{
public:
    HsWidgetData()
      : id(-1), 
        pageId(-1)
    {}

    int     id;
    QString uri;
    int     pageId;
};

class HSDOMAINMODEL_EXPORT HsWidgetPresentationData
{
public:
    HsWidgetPresentationData()
        : x(0), y(0), zValue(0), 
          widgetId(-1)
    {}

    void setPos(const QPointF &pos)
    {
        x = pos.x();
        y = pos.y();
    }

    QString key;
    qreal   x;
    qreal   y;
    qreal   zValue;
    int     widgetId;
};

#endif // HSDOMAINMODELDATASTRUCTURES_H
