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
* Description:  Observer for changes in Publish & Subscribe keys
*
*/


#ifndef C_PSOBSERVER_H
#define C_PSOBSERVER_H

#include <e32property.h>
#include <e32base.h>


namespace AiXmlUiController
{

class MPSCallback;

/**
*  @ingroup group_xmluicontroller
* 
*  Observer for changes in Publish & Subcribe keys
*
*  @lib AiXmlUiMain
*/
class CPSObserver : public CActive
    {
	public:  // Constructors and destructor
	    
	    static CPSObserver* NewL( MPSCallback& aCallback, TUid aCategory, TInt aKey );
	    
	    virtual ~CPSObserver();

	    /**
	     * Returns the current value of the monitored
	     * P&S key. 
	     * 
	     * @param aValue Value is placed here
	     * 
	     * @return KErrNone if succesfull
	     */
	    TInt GetValue( TInt &aValue ) const;
	    
	private:

		CPSObserver( MPSCallback& aCallback, TUid aCategory, TInt aKey );
		void ConstructL();
		

	private:	// from CActive

		void RunL();
		void DoCancel();
		TInt RunError();	
		

	private:    // Data

	    RProperty iProperty;
	    MPSCallback& iCallback;
	    TUid iCategory;
	    TInt iKey;
	    
   };

} // namespace AiXmlUiController

#endif      // C_PSOBSERVER_H 
           
//  End of File
