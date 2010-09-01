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
* Description:   suite observer interface
*
*/



#ifndef HNSUITEOBSERVER_H
#define HNSUITEOBSERVER_H

#include <e32def.h>

/**
 * Definition of events that are associated
 * with the lifecycle of a suite
 */
enum THnCustomSuiteEvent
    {
        ESuitePushedToStack,
        ESuitePoppedFromStack,
        ESuiteModelInitialized,
        ESuiteModelDestroyed,
        ESuiteItemsAdded,
        ESuiteItemsRemoved,
        ESuiteItemsUpdated,
        ESuiteHighlightChanged
    };

class CHnSuiteModel;

/**
 *  Suite observer interface class.
 * 
 *  Interface, for observers of suite events. Suite 
 *  May inform its observers about different situation it encoutners.
 *  e.g. suite informs all about its initialization, its deletion, 
 *  about updates of items. 
 *  This interface is also used by the suite container stack to inform
 *  all interested classes of new suite being pushed or popped from the stack. 
 * 
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
class MHnSuiteObserver
    {
    
public:

    /**
     * Whenever an event occurs, and suite wants to notify observers,
     * it invokes this method on all of its own observers, passing the information
     * about the particular event as a parameter.
     * 
     * @param aCustomSuiteEvent Type of evetn
     * @param aModel Suite presentation model that is notifying.
     */
    virtual void HandleSuiteEventL ( THnCustomSuiteEvent aCustomSuiteEvent, 
            CHnSuiteModel *aModel ) = 0;
    
    };


#endif // HNSUITEOBSERVER_H

