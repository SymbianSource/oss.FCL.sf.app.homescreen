/*
* ===========================================================================
*  Name        : HgPropListener.h
*  Part of     : Hg
*  Description : Class to get notifications about changes to a P&S property
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ===========================================================================
*/

#ifndef __PROPLISTENER_H__
#define __PROPLISTENER_H__

#include <e32base.h>
#include <e32property.h>

/**
 * Observer interface for getting notifications about a P&S property change.
 */
class MHgPropertyChangeObserver
	{
public:
	virtual void PropertyChanged( TUid aCategory, TUint aKey ) = 0;
	};

/**
 * Class to get notifications about changes to a P&S property.
 */
NONSHARABLE_CLASS( CHgPropertyListener ) : public CActive
	{
public:
    /**
     * Constructor.
     * Starts listening for the property with the given category/key.
     */
	IMPORT_C CHgPropertyListener( TUid aCategory,
	    TUint aKey, MHgPropertyChangeObserver& aObserver );
	    
	/**
	 * Destructor.
	 */
	IMPORT_C ~CHgPropertyListener();
    
private:
	void DoCancel();
	void RunL(); 
	TInt RunError( TInt aError );

	MHgPropertyChangeObserver& iObserver;
	RProperty iProperty;
	TUid iCategory;
	TUint iKey;
	};
    
#endif

// End of file
