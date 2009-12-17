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
 

#ifndef C_HNDATACHANGEOBSERVER_H
#define C_HNDATACHANGEOBSERVER_H

class CHnItemId;

/**
 * @ingroup group_hnengine
 * Data change observer interface. It is used to notify the implementing class that something in the database has been changed.
 *
 * @lib hnengine
 * @since S60 v5.0 
 */
NONSHARABLE_CLASS(CHnDataChangeObserver) : public CBase
    {    
    public:

    /**
     * DataChangeL is called when sth in database was changed.
     * @since S60 v5.0
     * @param aItemIds Array with ItemIds of changed entries.
     */
    virtual void DataChangedL( RPointerArray< CHnItemId >& aItemIds ) = 0;
    };


#endif // C_HNDATACHANGEOBSERVER_H
