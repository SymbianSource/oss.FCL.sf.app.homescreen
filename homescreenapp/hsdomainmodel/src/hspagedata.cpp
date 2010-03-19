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
* Description:  HsPage meta data.
*
*/

#include "hspagedata.h"

/*!
    \class HsPageData
    \ingroup group_hsdatamodel
    \brief Contains page metadata.

    Used by content store and content service for
    holding page metadata.
*/


/*!
    \fn HsPageData::HsPageData()

    Constructor
*/
HsPageData::HsPageData()
    : mId(-1),
      mIndex(-1)
{

}

/*!
    \fn HsPageData::~HsPageData()

    Destructor
*/
HsPageData::~HsPageData()
{

}

/*!
    \fn void HsPageData::setId(int aId)

    Set page \a pageId
*/
void HsPageData::setId(int pageId)
{
	mId = pageId;
}

/*!
    \fn int HsPageData::id() const

    Return page id
*/
int HsPageData::id() const
{
    return mId;
}

/*!
    \fn void HsPageData::setIndex(int index)

    Set page \a index i.e it's appearance order
*/
void HsPageData::setIndex(int index)
{
	mIndex = index;
}

/*!
    \fn int HsPageData::index() const

    Return page index
*/
int HsPageData::index() const
{
	return mIndex;
}

/*!
    \fn QList<HsWidgetData> HsPageData::widgets() const

    Get widgets belonging to this page
*/
QList<HsWidgetData> HsPageData::widgets() const
{
	return mWidgets; 
}

/*!
    \fn QList<HsWidgetData> &HsPageData::widgets()

    Get modifiable list of widgets belonging to this page
*/
QList<HsWidgetData> &HsPageData::widgets()
{
    return mWidgets; 
}
