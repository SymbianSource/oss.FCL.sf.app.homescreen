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
* Description:  Home screen widget presentation data.
*
*/


#include "hswidgetpresentationdata.h"

/*!
    \class HsWidgetPresentationData
    \ingroup group_hsdatamodel
    \brief Contains widget presentation metadata.

    Used by content store and content service for
    holding widget presentation metadata.
 
*/




/*!
    Initialize id and widget id to -1 and z value to 0.0 
*/
HsWidgetPresentationData::HsWidgetPresentationData()
    : mId(-1),
      mWidgetId(-1),
      mZValue(0.0)
{

}

/*!
    Destructor
*/
HsWidgetPresentationData::~HsWidgetPresentationData()
{

}

/*!
    Set widget \a id 
*/
void HsWidgetPresentationData::setId(int id)
{
    mId = id;
}	

/*!
    Return widget id 
*/ 
int HsWidgetPresentationData::id() const
{
    return mId;
}
  
/*!
    Set \a key to distinguish presentations
*/
void HsWidgetPresentationData::setKey(const QString &key)
{
    mKey = key;
}
	    
/*!
    Return presentation key 
*/ 
QString HsWidgetPresentationData::key() const
{
    return mKey;
}

/*!
    Set \a position 
*/ 
void HsWidgetPresentationData::setPosition(const QPointF &position)
{
    mPosition = position;
}
   
/*!
    Return position 
*/ 
QPointF HsWidgetPresentationData::position() const
{
    return mPosition;
}

/*!
    Set \a zValue 
*/  
void HsWidgetPresentationData::setZValue(qreal zValue)
{
    mZValue = zValue;
}

/*!
    Return z value 
*/
qreal HsWidgetPresentationData::zValue() const
{
    return mZValue;
}

/*!
    Set \a size 
*/ 
void HsWidgetPresentationData::setSize(const QSizeF &size)
{
    mSize = size;
}

/*!
    Return size 
*/
QSizeF HsWidgetPresentationData::size() const
{
    return mSize;
}

/*!
    Set \a widgetId 
*/
void HsWidgetPresentationData::setWidgetId(int widgetId)
{
    mWidgetId = widgetId;
}	

/*!
    Return widget id
*/
int HsWidgetPresentationData::widgetId() const
{
    return mWidgetId;
}


