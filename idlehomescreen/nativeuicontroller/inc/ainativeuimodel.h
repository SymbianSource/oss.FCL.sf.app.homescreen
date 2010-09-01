/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common NativeUi definitions and utilities
*
*/


#ifndef AINATIVEUIMODEL_H
#define AINATIVEUIMODEL_H


#include <e32std.h>

namespace AiNativeUiController
{

/**
 *  Common NativeUi definitions and utilities
 *
 *  @since S60 3.2
 */

namespace AiNativeUiModel
{
	/// Internal identifier for navi pane
	_LIT8( KNaviPaneId, "navipane" );

    /// Constant for unassigned rendering priority value
    const TInt KNoPriority = KErrNotFound;

    struct TRenderingPriorityItem;
    
/**
 * UI Control rendering priorities object.
 */ 
class TRenderingPriorities
	{
public:

	/**
	 * Constructor for publishable items that need to be priorized.
	 * @param aRenderingPriorities Array of priorized items
	 * @aCount Number of items in the array
	 */
	TRenderingPriorities(const TRenderingPriorityItem* aRenderingPriorities, TInt aCount);

	/**
	 * Returns rendering priority for given ContentId.
	 */
	TInt RenderingPriority( TInt aContentId );	
	
	/**
	 * Gets next content id with priority < aNextPriority.
	 *
	 * @param[in,out] aContentId	Next content id with priority < aNextPriority.
	 * @param[in,out] aNextPriority	On entry the priority is used to search the 
	 *						item content with next lowest priority. On exit
	 *						the parameter's value is the rendering priority 
	 *						of aContentId.
	 * @return  true if next content item was found, false if out of
	 *			content items with lower priority than aNextPriority.
	 *			Out parameters are updated only if this function returns true.
	 */
	TBool GetNextContentId( TInt& aContentId, TInt& aNextPriority );

private:  // Data

	/**
	 * An array that holds rendering priority related items.
	 */
	const TRenderingPriorityItem* iRenderingPriorities;
	
	/**
	 * Number of items in the array.
	 */
	TInt iCount;

	};

/**
 * Returns rendering priorities for ui control.
 *
 * @param aUiElementId  UI control identifier.
 * @return UI control priorities object.
 */
TRenderingPriorities RenderingPriorities( const TDesC8& aUiElementId );
    
} // namespace AiNativeUiModel

} // namespace AiNativeUiController

#endif // AINATIVEUIMODEL_H
