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
 * Description: hsmenuitemmodel.h
 *
 */

#ifndef HSMENUITEMMODEL_H
#define HSMENUITEMMODEL_H

// System includes
#include <caquery.h>
#include <caitemmodel.h>
#include <HbIcon>

// User includes
#include "hsmenuservice_global.h"
#include "hsmenuservice.h"

// Class declaration
class MENUSERVICE_EXPORT HsMenuItemModel: public CaItemModel
{

public:

    // Data types
    enum IconSize {
        SmallIconSize,
        BigIconSize
    };

    // Function declarations
    explicit HsMenuItemModel(const CaQuery &query,
                             QObject *parent = 0);
    ~HsMenuItemModel();
    void setSort(HsSortAttribute sortAttribute);
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;
    bool newIconNeeded(const QModelIndex &index) const;
};

#endif // HSMENUITEMMODEL_H 
