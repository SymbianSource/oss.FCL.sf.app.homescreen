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

#ifndef HSWIDGETPOSITIONINGONWIDGETADD_H
#define HSWIDGETPOSITIONINGONWIDGETADD_H

#include <QList>
#include <QRectF>
#include "hsutils_global.h"

class HSUTILS_EXPORT HsWidgetPositioningOnWidgetAdd
{
public:
    virtual ~HsWidgetPositioningOnWidgetAdd() {}
    
    virtual QList<QRectF> convert(const QRectF &contentArea,
                                  const QList<QRectF> &widgets) = 0;

    static void setInstance(HsWidgetPositioningOnWidgetAdd *instance);
    static HsWidgetPositioningOnWidgetAdd *instance();

private:
    static HsWidgetPositioningOnWidgetAdd *mInstance;
};

class HSUTILS_EXPORT HsAnchorPointInBottomRight : public HsWidgetPositioningOnWidgetAdd
{
public:
    QList<QRectF> convert(const QRectF &contentArea,
                          const QList<QRectF> &widgets);
};

class HSUTILS_EXPORT HsAnchorPointInCenter : public HsWidgetPositioningOnWidgetAdd
{
public:
    QList<QRectF> convert(const QRectF &contentArea,
                          const QList<QRectF> &widgets);
};


#endif // HSWIDGETPOSITIONINGONWIDGETADD_H
