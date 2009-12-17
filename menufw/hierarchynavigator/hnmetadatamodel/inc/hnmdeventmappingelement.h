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



#ifndef C_HNMDEVENTMAPPINGELEMENT_H
#define C_HNMDEVENTMAPPINGELEMENT_H

#include <e32base.h>
#include <xmlengdom.h>

#include "hnglobals.h"

class CHnMdItem; 
class CHnMdAction;
class CHnMdEvent;
struct THnMdCommonPointers;

/**
 * Event Mapping Element.
 * 
 * This class is used to bind key events (or different events)
 * with specific actions.
 
 *
 * @lib hierarchynavigatorengine
 * @since S60 v5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdEventMappingElement) : public CBase
    {
public:
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdEventMappingElement* NewL( TXmlEngElement aElement );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdEventMappingElement* NewLC( TXmlEngElement aElement );

    /**
     * Default destructor.
     */
    virtual ~CHnMdEventMappingElement();
    
    /**
     * Returns actions.
     * 
     * @return A list of actions stored in an object of this class.
     */
    RPointerArray<CHnMdAction>& Actions();
    
    /**
     * Returns events.
     * 
     * @return A list of events stored in the object of this class.
     */
    const RPointerArray<CHnMdEvent>& Events() const;


private:

    /**
     * Standard constructor.
     */
    CHnMdEventMappingElement();

    /**
     * EPOC default constructor for performing 2nd stage construction.
     * 
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );
    
    /**
     * Adds an event to the list of events.
     * 
     * @param aEvent A pointer to the event object.
     */
    void AddEventL( CHnMdEvent* aEvent );

    /**
     * Sets events on the base of the xml element.
     * 
     * @param aElement Xml element.
     */
    void SetEventsL( TXmlEngElement aElement );
    
    /**
     * Sets actions on the base of the xml element.
     * 
     * @param aElement Xml element.
     */
    void SetActionsL( TXmlEngElement aElement );

private: // data

    /**
     * Own - Events.
     */
    RPointerArray<CHnMdEvent> iEvents;
    
    /**
     * Own - Actions.
     */
    RPointerArray<CHnMdAction> iActions;
    
    };

#endif // C_HNMEVENTMAPPINGELEMENT_H
