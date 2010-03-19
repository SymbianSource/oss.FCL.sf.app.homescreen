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
* Description: Default Screensaver provider.
*
*/

#include "snsrdefaultstateprovider.h"

#include <QState>

#include "snsrrootstate.h"

/*!
    \class SnsrDefaultStateProvider
    \ingroup group_snsrdefaultstateprovider
    \brief Provides a default implementation for each screensaver state.

    States are described in the snsrdefaultstateprovider.manifest file.
*/

/*!
    Constructs a new SnsrDefaultStateProvider.
*/
SnsrDefaultStateProvider::SnsrDefaultStateProvider()
{
    mSnsrRootStateToken.mLibrary = "snsrdefaultstateprovider.dll";
    mSnsrRootStateToken.mUri = "screensaver.nokia.com/state/root";
}

/*!
    Destructs the class.
*/
SnsrDefaultStateProvider::~SnsrDefaultStateProvider()
{
}

/*!
    Returns contained states as a list of tokens.
*/
QList<HsStateToken> SnsrDefaultStateProvider::states()
{
    return QList<HsStateToken>() << mSnsrRootStateToken;
}

/*!
    Creates a state based on the given token.
    \param token - Identifies the state to be created.
*/
QState *SnsrDefaultStateProvider::createState(const HsStateToken& token)
{
    if ((token.mLibrary == mSnsrRootStateToken.mLibrary) &&
        (token.mUri == mSnsrRootStateToken.mUri)) {
            return new SnsrRootState();
    }
    return 0;
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(snsrdefaultstateprovider, SnsrDefaultStateProvider)
#endif // COVERAGE_MEASUREMENT
