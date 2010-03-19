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
* Description: Screensaver custom document loader.
*
*/

#ifndef TSDOCUMENTLOADER_H
#define TSDOCUMENTLOADER_H

#include <hbdocumentloader.h>

class TsDocumentLoader : public HbDocumentLoader
{

private:
    virtual QObject *createObject(const QString &type, const QString &name);

};

#endif // TSDOCUMENTLOADER_H
