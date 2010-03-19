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
* Description:  Contains widget metadata.
*
*/

#ifndef HSWIDGETDATA_H
#define HSWIDGETDATA_H

#include <QList>
#include <QString>
#include "hsdomainmodel_global.h"
#include "hswidgetpresentationdata.h"


class HSDOMAINMODEL_EXPORT HsWidgetData
{

public:

	HsWidgetData();
	~HsWidgetData();

	void setId(int id);
 	int id() const;

    void setUri(const QString &uri);
	QString uri() const;
	
    QList<HsWidgetPresentationData> presentations() const;
    QList<HsWidgetPresentationData> &presentations();

	void setPageId(int pageId);
	int pageId() const;

private:

    int mId;
	QString mUri;
    QList<HsWidgetPresentationData> mPresentations;
    int mPageId;

};




#endif
