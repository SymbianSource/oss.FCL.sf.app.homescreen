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
* Description:  Contains widget presentation metadata.
*
*/


#ifndef HSWIDGETPRESENTATIONDATA_H
#define HSWIDGETPRESENTATIONDATA_H

#include <QString>
#include <QPointF>
#include <QSizeF>
#include "hsdomainmodel_global.h"

class HSDOMAINMODEL_EXPORT HsWidgetPresentationData
{

public:
	HsWidgetPresentationData();
	~HsWidgetPresentationData();

	void setId(int id);
	int id() const;

    void setKey(const QString &key);
    QString key() const;

    void setPosition(const QPointF &position);
    QPointF position() const;

	void setZValue(qreal zValue);
   	qreal zValue() const;

    void setSize(const QSizeF &size);
    QSizeF size() const;

	void setWidgetId(int widgetId);
	int widgetId() const;
   	
private:
    int mId;
	QString mKey;
    QPointF mPosition;
    QSizeF mSize;
    int mWidgetId;
    qreal mZValue;	
};

#endif
