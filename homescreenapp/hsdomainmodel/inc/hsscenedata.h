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
* Description:  Contains scene metadata.
*
*/

#ifndef HSSCENEDATA_H
#define HSSCENEDATA_H

#include <QString>
#include "hsdomainmodel_global.h"
#include "hspagedata.h"

class HSDOMAINMODEL_EXPORT HsSceneData
{

public:
	HsSceneData();
	~HsSceneData();
	
    void setId(int id);
	int id() const;

    void setPortraitWallpaper(const QString &path);
	QString portraitWallpaper() const;

    void setLandscapeWallpaper(const QString &path);
	QString landscapeWallpaper() const;

    void setDefaultPage(const HsPageData &defaultPage);
	HsPageData defaultPage() const;
    
private:
    int mId;
	QString mPortraitWallpaper;
    QString mLandscapeWallpaper;
    HsPageData mDefaultPage;
	
};




#endif
