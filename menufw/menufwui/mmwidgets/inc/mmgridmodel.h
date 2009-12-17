/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmGridModel declaration
*  Version     : %version: MM_14 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMGRIDMODEL_H
#define C_MMGRIDMODEL_H

#include <e32std.h>
#include <e32base.h>
#include <AknGridM.h> 

class CMmListBoxModel;
class CHnSuiteModel;

/**
 *  Multimedia Menu grid model.
 *
 *  @code
 *  @endcode
 *  @lib 
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmGridModel ) : public CAknGridM
    {
public:
    /**
     * Two-phased constructor.
     */
    static CMmGridModel* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CMmGridModel* NewLC();
    
    /**
     * Destructor.
     * 
     * @since S60 v3.0
     */
    ~CMmGridModel();


    /**
     * Returns the number of grid items in model.
     *
     * @since S60 v3.0
     * @return Number of items in model.
     */
    TInt NumberOfItems() const;
    
    /**
     * Returns the Multimedia Menu listbox model.
     *
     * @since S60 v3.0
     * @return Listbox model.
     */
    CMmListBoxModel* MmListBoxModel();

private:
    /**
     * Default constructor.
     * 
     * @since S60 v3.0 
     */
    CMmGridModel();

    /**
     * 2nd phase constructor.
     * 
     * @since S60 v3.0 
     */
    void ConstructL();
	
    /**
     * 2nd phase constructor.
     * Do not delete!!! This constructor does not do anything 
     * and is overloaded to make sure AVKON sets up the grid only by calling 
     * ConstructL() without any parameters.
     * 
     * @since S60 v5.0 
     */
	void ConstructL(MDesCArray* /*aItemTextArray*/, TListBoxModelItemArrayOwnership /*aOwnershipType*/);
	
private: // Data
	
    /**
     * The Multimedia Menu Listbox model which is in fact the grid model used.
     */
    CMmListBoxModel* iMmListBoxModel;

    };

#endif // C_MMGRIDMODEL_H
