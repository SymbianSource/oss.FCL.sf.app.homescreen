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
* Description:   base class for all image providers
*
*/



#ifndef C_HNATTRIMGPROVIDER_H
#define C_HNATTRIMGPROVIDER_H

#include <e32base.h>
#include <gulicon.h>
#include <AknUtils.h>
#include "SVGRequestObserver.h"
#include "hniconholder.h"


/**
 * Allows for removing an item from a cleanup stack without using
 * CleanupStack::Pop().
 * This is useful when an item must be removed from the cleanup stack
 * but it is not possible to use Pop() because some other items have
 * been pushed onto cleanup stack after that item. 
 */
NONSHARABLE_CLASS( TDismissableCleaner )
    {
public:
    /**
     * Constructor.
     * 
     * @param aToBeDeletedOnLeave pointer to a memory to be freed on leave.
     */
    TDismissableCleaner( TAny* aToBeDeletedOnLeave );
    
    /**
     * The Close method - to be called during the cleanup process.
     * It simply deletes the object passed as parameter to constructor
     * unless Dismiss() has been called.
     */
    void Close();
    
    /**
     * Dismiss the deletion.
     * Call this method to prevent the memory freeing on leave.
     * This is an equivalent of removing item from cleanup stack with Pop().
     */
    void Dismiss();
private:
    /**
     * Pointer to memory to be deleted on leave.
     * Not own.
     */
    TAny* mToBeDeletedOnLeave;
    };

/**
 * Image provider class interface.
 * Interface for attribute image classes. Provides methods
 * for obtainnig values and icons. 
 *  
 * @lib hnpresentationmodel
 * @since S60 5.0
 * @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttrImgProvider ) : public CBase
    {
    
public:

    /**
     * Standard destructor.
     * 
     * @since S60 5.0
     */
    ~CHnAttrImgProvider();

    /**
     * Gets icon.
     * 
     * @since S60 5.0
     * @param aDesiredSize Desired size of the icon.
     * @return Icon as a CGulIcon pointer.
     */
    virtual CGulIcon* GetIcon( TSize* aDesiredIconSize );
    
    /**
     * Gets icon holder which contains the icon that GetIcon returns.
     * 
     * @param aDesiredSize Desired size of the icon.
     * @return icon hoder that contains the icon (may return NULL).
     */
    virtual CHnIconHolder* GetIconHolder( TSize* aDesiredIconSize );
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    virtual void RemoveLiwObjects();
        
protected:
    
    /**
     * Standard constructor.
     * 
     * @since S60 5.0
     */
    CHnAttrImgProvider();
    
    /**
     * Creates icon.
     * 
     * @since S60 5.0
     * @param aBitmap bitmap
     * @param aMask mask
     */
    void CreateIconL( CFbsBitmap* aBitmap, CFbsBitmap* aMask );

       
protected:
    
    /**
     * Icon holder - the object that owns the CGulIcon that this
     * provider creates. 
     * Co-own(CHnIconHolder is ref-counted).
     */
    CHnIconHolder* iIconHolder;
    
    };


#endif // C_HNATTRIMGPROVIDER_H

