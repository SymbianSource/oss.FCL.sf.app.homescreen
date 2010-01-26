/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmListBoxItemModel
*  Version     : %version: MM_18.1.4 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMLISTBOXMODEL_H
#define C_MMLISTBOXMODEL_H

#include <e32std.h>
#include <e32base.h>
#include <gulicon.h>
#include <AknUtils.h>
#include "hnsuiteobserver.h"

class CHnSuiteModel;
class CHnAttributeBase;
class CHnAttributeImage;
class CHnIconHolder;

/**
 *  Multimedia Menu listbox model
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmListBoxModel ):
    public CAknFilteredTextListBoxModel,
    public MHnSuiteObserver
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CMmListBoxModel();

	/**
	 * Two-phased constructor.
	 */
	static CMmListBoxModel* NewL( );

	/**
	 * Two-phased constructor.
	 */
	static CMmListBoxModel* NewLC( );
	
	/**
	 * Gets the text attribute for a specific item by the name of the attribute.
	 * 
	 * @since S60 v3.0
	 * @param aIndex The index of the item for which the text is requested.
	 * @param aAttributeName The name of the text attribute to be returned.
	 * @return Size of item.
	 */
	const TDesC8& GetAttributeAsText( TInt aIndex, const TDesC8 & aAttributeName );
    
    /**
     * Gets the graphical attribute for a specific item by the name of the attribute.
     * The attribute is returned as an icon holder. Please see @c CHnIconHolder for
     * infomation on what icon holder is and what
     * it is useful for.
     * 
     * @param aIndex The index of the item for which the graphic is requested.
     * @param aAttributeName The name of the graphical attribute to be returned.
     * @param aDesiredIconSize The desired icon size. 
     * @return An icon holder object.
     */
    CHnIconHolder* GetAttributeAsRefCountedGraphics( TInt aIndex,
            const TDesC8& aAttributeName, TSize* aDesiredIconSize = NULL );
	
	/**
	 * Sets the suite model. This method is used by matrix menu application when suite is loaded.
	 * 
	 * @since S60 v3.0
	 * @param aSuiteModel The suite model to be set.
	 */ 
	void SetSuiteModelL( CHnSuiteModel * aSuiteModel );	
	
	/**
	 * Gets the suite model of the currently visible suite.
	 * 
	 * @since S60 v3.0
	 * @return The current suite model.
	 */
	CHnSuiteModel* GetSuiteModel(); 
	
	/**
	 * Reorders the model when item positions are edited in edit mode.
	 * 
	 * @since S60 v3.0
	 * @param aFromIndex The starting index of items to be shifted.
	 * @param aToIndex The end index of items to be shifted.
	 */
	void ReorderModelL(TInt aFromIndex, TInt aToIndex);
	
	/**
	 * Handle event triggered by suite.
	 * 
	 * @since S60 v3.0
	 * @param aCustomSuiteEvent Type of suite event.
	 * @param aModel The suite model that the event concerns.
	 */
    void HandleSuiteEventL( THnCustomSuiteEvent aCustomSuiteEvent, 
            CHnSuiteModel* aModel );
    
    /**
     * Determines the number of item-specific menu items for given item.
     * 
     * @since S60 v3.0
     * @param aItemIndex Item index.
     * @return Number of item-specific menu items.
     */
    TInt GetNumberOfSpecificMenuItemsL( TInt aItemIndex );
    
private:

	/**
	 * Gets the attribute by name.
	 * 
	 * @since S60 v3.0
	 * @param aIndex The index of the item for which the attribute is requested.
	 * @param aAttributeName The name of the requested attribute.
	 * @return Size of item.
	 */
    CHnAttributeBase* GetAttribute( TInt aIndex, const TDesC8 & aAttributeName );
	
	/**
	 * Updates the item text array when model is reordered or new model is loaded.
	 * It is important that the number of elements in the array is the same
	 * as number of items in the suite.
	 * 
	 * @since S60 v3.0
	 */
    void UpdateDummyArrayDataL();  

	
    /**
	 * Constructor for performing 1st stage construction
	 */
	CMmListBoxModel( );

	/**
	 * Default symbian 2nd stage constructor.
	 */
	void ConstructL();


private:
    
    /**
     * Not own. Suite model being the container for the data.
     */
    CHnSuiteModel* iSuiteModel;
    
	};

#endif // CMMLISTBOXMODEL_H
