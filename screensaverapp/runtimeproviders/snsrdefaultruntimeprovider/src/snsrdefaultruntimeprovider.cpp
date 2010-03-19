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
* Description: Default runtime provider.
*
*/

#include "snsrdefaultruntimeprovider.h"

#include <QList>

#include "snsrdefaultruntime.h"

/*!
    \class SnsrDefaultRuntimeProvider
    \ingroup group_snsrdefaultruntimeprovider
    \brief Provides a default implementation of the screensaver runtime.

    This provider includes a default implementation of the screensaver runtime.
    The runtime is described in the snsrdefaultruntimeprovider.manifest file.
*/

// Constants
const char gFileName [] = "snsrdefaultruntimeprovider.dll";
const char gFileUri [] = "screensaver.nokia.com/runtime/defaultruntime";

/*!
    Constructs a new SnsrDefaultRuntimeProvider with parent.
*/
SnsrDefaultRuntimeProvider::SnsrDefaultRuntimeProvider(QObject *parent) :
    QObject(parent)
{
}

/*!
    Destructs the class.
*/
SnsrDefaultRuntimeProvider::~SnsrDefaultRuntimeProvider()
{
}

/*!
    Returns contained runtimes as a list of tokens.
*/
QList<HsRuntimeToken> SnsrDefaultRuntimeProvider::runtimes()
{
    HsRuntimeToken token = {gFileName, gFileUri};
    return QList<HsRuntimeToken>() << token;
}

/*!
    Creates a runtime based on the given token.
*/
HsRuntime *SnsrDefaultRuntimeProvider::createRuntime(const HsRuntimeToken& token)
{
    if ((token.mLibrary == gFileName) && (token.mUri == gFileUri)) {
        return new SnsrDefaultRuntime();
    }
    else {
        return 0;
    }
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(snsrdefaultruntimeprovider, SnsrDefaultRuntimeProvider)
#endif // COVERAGE_MEASUREMENT
