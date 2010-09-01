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
* Description:   toolbar presentation model
*
*/


#ifndef HNTOOLBARMODEL_H_
#define HNTOOLBARMODEL_H_

#include <e32base.h>
#include <e32cmn.h>

class CHnButtonModel;

/**
 *  Toolbar model class.
 *  
 *  Toolbar represents toolbar class from meta data model. Contains buttons.
 *  UI layer can fetch the data from toolbar model to display it on screen. 
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnToolbarModel ) : public CBase
    {
public:
        
    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C static CHnToolbarModel* NewLC();
        
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C ~CHnToolbarModel();
    
    /**
     * Adds toolbar button.
     *
     * @since S60 5.0 
     * @param aButton Toolbar button to add
     */
    IMPORT_C void AddButtonL( CHnButtonModel* aButton );
    
    /**
     * Gets toolbar button model.
     * 
     * @since S60 5.0
     * @param aIndex Index of toolbar button
     * @return Toolbar button model for item, NULL if not exist
     */
    IMPORT_C const CHnButtonModel* GetButton( TInt aIndex ) const;
    
    /**
     * Informs if toolbar has any buttons.
     * 
     * @since S60 5.0
     * @return true if has, false if doesn't
     */    
    IMPORT_C TBool HasButtons() const;
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    void RemoveLiwObjects();
    
private:
    
    /**
     * Default constructor.
     * 
     * @param aMaxCount Maximal number of buttons in the toolbar.
     * @since S60 5.0
     */
    CHnToolbarModel( TInt aMaxCount = 3);
    
    /**
     * Second phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();
    
private: // data

    /**
     * Buttons for toolbar. Max 3 buttons.
     */
    RPointerArray< CHnButtonModel > iButtons;
    
private: // constants
    
    /**
     * Max button count.
     */
    const TInt KMaxButtonCount;
    };


#endif // HNTOOLBARMODEL_H_
