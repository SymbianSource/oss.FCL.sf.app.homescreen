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
* Description:  Home screen widget meta data.
*
*/


#include "hswidgetdata.h"


/*!
    \class HsWidgetData 
    \ingroup group_hsdatamodel
    \brief Contains widget metadata.

    Used by content store and content service for
    holding widget metadata.
*/



/*!
    \fn HsWidgetData::HsWidgetData()

    Initialize widget and page id to -1
*/
HsWidgetData::HsWidgetData()
    : mId(-1),
      mPageId(-1)
{

}

/*!
    \fn HsWidgetData::~HsWidgetData()

    Destructor
*/
HsWidgetData::~HsWidgetData()
{

}

/*!
    Set widget \a id
*/
void HsWidgetData::setId(int id)
{
	mId = id;
}

/*!
    Return widget id
*/
int HsWidgetData::id() const
{
    return mId;
}

/*!
    Set widget \a uri
*/
void HsWidgetData::setUri(const QString &uri)
{
	mUri = uri;
}

/*!
     Return widget uri.
*/
QString HsWidgetData::uri() const
{
	return mUri;
}


/*!
    Return widget presentations
*/
QList<HsWidgetPresentationData> HsWidgetData::presentations() const
{
    return mPresentations;
}
	
/*!
    Return modifiable widget presentations
*/
QList<HsWidgetPresentationData> &HsWidgetData::presentations()
{
	return mPresentations;
}

/*!
    Set \a pageId where widget is
*/
void HsWidgetData::setPageId(int pageId)
{
	mPageId = pageId;
}

/*!
    Return widget page id
*/
int HsWidgetData::pageId() const
{
    return mPageId;
}


