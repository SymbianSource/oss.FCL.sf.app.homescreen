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
* Description:  Scene meta data.
*
*/


#include "hsscenedata.h"


/*!
    \class HsSceneData
    \ingroup group_hsdatamodel
    \brief Contains scene metadata.

    Used by content store and content service for
    holding scene metadata.
*/


/*!
    \fn HsSceneData::HsSceneData()

    Initialize scene id to -1
*/
HsSceneData::HsSceneData() 
    : mId(-1)
{

}

/*!
    \fn HsSceneData::~HsSceneData()

    Destructor
*/
HsSceneData::~HsSceneData()
{

}

/*!
    \fn void HsSceneData::setId(int id)

    Set scene \a id
*/
void HsSceneData::setId(int id)
{
	mId = id;
}

/*!
    \fn int HsSceneData::id() const

    Return scene id
*/
int HsSceneData::id() const
{
    return mId;
}

/*!
    Sets portrait wallpaper \a path.
*/
void HsSceneData::setPortraitWallpaper(const QString &path)
{
	mPortraitWallpaper = path;
}

/*!
    Returns portrait wallpaper.
*/
QString HsSceneData::portraitWallpaper() const
{
	return mPortraitWallpaper;
}

/*!
    Sets landscape wallpaper \a path.
*/
void HsSceneData::setLandscapeWallpaper(const QString &path)
{
	mLandscapeWallpaper = path;
}

/*!
    Returns landscape wallpaper.
*/
QString HsSceneData::landscapeWallpaper() const
{
	return mLandscapeWallpaper;
}

/*!
    \fn void HsSceneData::setDefaultPage(const HsPageData &defaultPage)

    Set scene \a defaultPage 
*/
void HsSceneData::setDefaultPage(const HsPageData &defaultPage)
{	
    mDefaultPage = defaultPage;
}

/*!
    \fn HsPageData HsSceneData::defaultPage() const

    Return scene default page
*/
HsPageData HsSceneData::defaultPage() const
{
    return mDefaultPage;
}


