/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef HNEVENTGENERATOR_H_
#define HNEVENTGENERATOR_H_

#include <e32def.h>

class CLiwGenericParamList;
/**
 * @ingroup group_hnengine
 * Hierarchy navigator event generator.
 * 
 * This class is an interface for event triggering in hierarchy navigator.
 * Events being triggered may be e.g. SuiteLoaded, ItemFocus, etc.
 * It also mediates in events being passed from the UI. 
 *
 * @lib hnengine
 * @since S60 5.0 
 */
class MHnEventGenerator
    {
    public:
  
    /**
    * Method is invoked whenever any component wants to trigger an event 
    * inside hierarchy navigator.
    * 
    * @since S60 5.0
    * @param aHnEventId Event type id to trigger.
    * @param aRecipientId The id of the recipient item which the event concerns.
    * @param aEventParameters Event specific parameters used to perform action.
    * @return Status code.
    */
    virtual TInt TriggerHnEventL( const TInt aHnEventId, const TInt aRecipientId,
            CLiwGenericParamList* aEventParameters = NULL ) = 0;
    };


#endif // HNEVENTGENERATOR_H_
