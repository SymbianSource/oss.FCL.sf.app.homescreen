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
* Description: tsentrymodelitem.h
*
*/

#ifndef TSENTRYMODELITEM_H
#define TSENTRYMODELITEM_H
#include "tsmodelitem.h"
#include <caservice.h>

class CaEntry;

/*!
    Class implements model item which represents Content Arsenal entry
*/
class TsEntryModelItem : public TsModelItem
{
public:
    TsEntryModelItem(CaService &service, QSharedPointer<CaEntry> entry, QSize mSize);
    ~TsEntryModelItem();
    QVariant data(int role) const;
    void close();
    void open();

private:
    bool closable() const;

private:
    CaService &mService;
    const QSharedPointer<CaEntry> mEntry;
    const QSize mSize;
};

#endif // TSMODEL_H
