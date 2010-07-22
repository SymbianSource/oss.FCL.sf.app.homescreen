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
 * Description: Menu view.
 *
 */

#ifndef HS_MAIN_WINDOW_H
#define HS_MAIN_WINDOW_H

#include <QObject>
#include <QPixmap>

class HsMenuView;
class HsMainWindow: public QObject
{
    Q_OBJECT
    
public:
    HsMainWindow();
    virtual ~HsMainWindow();

    virtual void setCurrentView(const HsMenuView &menuView);
    virtual QPixmap grabScreenshot();
       
    public slots:
    virtual void saveActivity();
    
    signals:
    void viewIsReady();
    
    private:
    /*
     * not owned
     * 
     */
    QObject* mActivityClient;
};

#endif // HS_MAIN_WINDOW_H
