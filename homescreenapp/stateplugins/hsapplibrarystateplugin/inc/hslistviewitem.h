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
 * Description: List View Item.
 *
 */

#ifndef CUSTOMLISTVIEWITEM_H
#define CUSTOMLISTVIEWITEM_H

#include <hblistviewitem.h>

#include "hsmenustates_global.h"

class HbProgressBar;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsListViewItem : public HbListViewItem
{
    Q_OBJECT
    public:
                
        explicit HsListViewItem(QGraphicsItem* parent=0);
        virtual ~HsListViewItem();
        
        virtual HbAbstractViewItem* createItem();
        virtual void updateChildItems();
    
	    virtual void polish(HbStyleParameters& params);

    private:

        HbProgressBar *progress;
        bool isProgress;
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};

#endif // CUSTOMLISTVIEWITEM_H
