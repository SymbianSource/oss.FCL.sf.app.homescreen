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


#ifndef C_HNMDEVENT_H
#define C_HNMDEVENT_H

#include <e32base.h>

/**
 * Event.
 *
 * An object of this class represents an event (identified
 * by id). 
 * 
 * @lib hnmetadatamodel
 * @since S60 v5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdEvent) : public CBase
    {
public:
    
    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aId Identifier.
     * @return Fully constructed object.
     */
    static CHnMdEvent* NewL( const TInt aId );

    /**
     * Standard factory method.
     *
     * since S60 v5.0
     * @param aId Identifier.
     * @return Fully constructed object.
     */

    static CHnMdEvent* NewLC( const TInt aId );

    /**
     * Standard destructor.
     */
    virtual ~CHnMdEvent();
    
    /**
     * Returns Id.
     *
     * since S60 v5.0
     * @return iId Identifier of the event.
     */
    IMPORT_C TInt Id() const;

    /**
     * Sets Id.
     * 
     * @param aId Identifier.
     */
    void SetId( TInt aId );

private:

    /**
     * Standard constructor.
     */
    CHnMdEvent();
    
    /**
     * EPOC default constructor for performing 2nd stage construction.
     * 
     * @param aId Identifier.
     */
    void ConstructL( const TInt aId );
    
private: // data

    /**
     * Event's identifier.
     */
    TInt iId;

    };

#endif // C_HNMMEVENT_H
