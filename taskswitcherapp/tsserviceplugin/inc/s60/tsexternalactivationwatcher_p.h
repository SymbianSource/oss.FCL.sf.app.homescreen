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
#ifndef TSEXTERNALACTIVATIONWATCHERPRIVATE_H
#define TSEXTERNALACTIVATIONWATCHERPRIVATE_H

#include <e32base.h>
#include <e32property.h>

class TsExternalActivationWatcher;

class TsExternalActivationWatcherPrivate : public CActive
{
public:
    TsExternalActivationWatcherPrivate(TsExternalActivationWatcher *parent);
    virtual ~TsExternalActivationWatcherPrivate();

public: //From CActive
    /**
    * Implements cancellation of an outstanding request.
    */
    virtual void DoCancel();

    /**
    Handles an active object's request completion event.
    */
    void RunL();
    
private:
    bool CheckActivationFlag();
    void ResetActivationFlag();

private:
    TsExternalActivationWatcher *mParent;
    RProperty mProperty;

};

#endif // TSEXTERNALACTIVATIONWATCHERPRIVATE_H
