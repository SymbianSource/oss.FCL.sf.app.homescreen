/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Callback for Publish & Subscribe observation
*
*/


#ifndef M_PSCALLBACK_H
#define M_PSCALLBACK_H

#include <e32base.h>

namespace AiXmlUiController
{
	
class MPSCallback
	{
	public:
	
		/**
		 * Callback method for P&S observer.
		 * Called when appropriate value has been updated.
		 * @param aKey Key that has been observed.
		 * @param aValue New Value
		 */
		virtual void ValueUpdatedL( TInt aKey, TInt aValue ) = 0;	
   
    protected:

        /**
         * Protected destructor prevents deletion through this interface.
         */
        MPSCallback() {};	
	};
	
} // namespace AiXmlUiController

#endif // M_PSCALLBACK_H
