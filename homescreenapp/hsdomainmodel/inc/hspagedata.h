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
* Description:  Contains page metadata.
*
*/

#ifndef HSPAGEDATA_H
#define HSPAGEDATA_H

#include <QList>
#include <QString>
#include "hsdomainmodel_global.h"
#include "hswidgetdata.h"

class HSDOMAINMODEL_EXPORT HsPageData
{
public:	
    HsPageData();
	~HsPageData();

    void setId(int pageId);
	int id() const;
	
	void setIndex(int index);
    int index() const;
	    
    QList<HsWidgetData> widgets() const;
    QList<HsWidgetData> &widgets();
        
private:
    int mId;	
    int mIndex;	    
	QList<HsWidgetData> mWidgets;    
};

#endif
