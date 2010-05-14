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

#ifndef HSPAGE_H
#define HSPAGE_H

#include <HbWidget>
#include <HbIconItem>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

class HsWidgetHost;
class HsPageData;

HOMESCREEN_TEST_CLASS(TestRuntimeServices)

class HSDOMAINMODEL_EXPORT HsPage : public HbWidget
{
	Q_OBJECT

public:
    HsPage(QGraphicsItem *parent = 0);
	~HsPage();

    int databaseId() const;
    void setDatabaseId(int id);
    
    bool load();

    bool addExistingWidget(HsWidgetHost *widgetHost);
    bool removeWidget(HsWidgetHost *widgeHost);

    QList<HsWidgetHost *> newWidgets();
    bool addNewWidget(HsWidgetHost *widgetHost);
    void layoutNewWidgets();
    bool deleteFromDatabase();

    QList<HsWidgetHost *> widgets() const;
        
    bool isRemovable() const;
    void setRemovable(bool removable);
    
    static HsPage *createInstance(const HsPageData &pageData);

public slots:
    void showWidgets();
    void hideWidgets();
    void setOnline(bool online = true);

    void updateZValues();

private:
    void connectWidget(HsWidgetHost *widget);
    void disconnectWidget(HsWidgetHost *widget);

private slots:
    void onWidgetFinished(HsWidgetHost *widget);
    void onWidgetResized(HsWidgetHost *widget);

private:
    int mDatabaseId;    
    bool mRemovable;
    QList<HsWidgetHost*> mWidgets;
    QList<HsWidgetHost*> mNewWidgets;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestRuntimeServices)
};
 
#endif
