/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Version     : 2 << Don't touch! Updated by Synergy at check-out.
*
*/

#ifndef MMSUBCELLSSETUPCODE_H
#define MMSUBCELLSSETUPCODE_H

#include <e32def.h>

const TInt KNumberOfBitsToStoreSubcellType = 3;
const TInt KNumberOfBitsToStoreTemplateId = 8;
const TInt KNumberOfBitsToStoreIsCurrentInfo = 1;

/**
 * The purpose of this class is to provide a very fast way
 * of determining whether two particular ways of setting up
 * subcells are compatible (compatible == the same).
 * 
 * It is important that the methods of this class are used
 * in a fixed order:
 * 1. (Optional) Clear() - only needed if this object was
 *    used before.
 * 2. AddSubcellInfo() - for every subcell being set up
 *    and in the same order.
 * 3. AddTemplateInfo
 * 4. AddIsCurrentInfo
 * It is currently possible to store information about up to 21
 * subcells (type of iCode is TUint64 and single subcell info
 * takes 3 bits). It can be easily proven that CMmListBoxItemDrawer
 * will never try to store more than
 * (2 * MmTemplateContants::KSubCellsCount) subcell infos
 * (2 * 6 = 12; 12 <= 21, so everything is fine).
 * Default assignment operator and copy constructor are ok
 * for this class.
 */
NONSHARABLE_CLASS( TMmSubcellsSetupCode )
    {
public:
    /**
     * Values of this enum must be low enough to be stored in
     * just KNumberOfBitsToStoreSubcellType bits (currently the
     * maximum value allowed is 7).
     */
    enum TSubcellType
        {
        ENoSubcell = 0,
        EGraphicsSubcell = 1,
        ETextSubcell = 2,
        EBackdropSubcell = 3,
        ESkippedSubcell = 4
        };
public:
    /**
     * Standard C++ constructor.
     */
    inline TMmSubcellsSetupCode();
    /**
     * Adds the information about what type of subcell was just
     * set up.
     * @param aSubcellType Subcell type (any value defined in the
     *        TSubcellType enum).
     */
    inline void AddSubcellInfo( TSubcellType aSubcellType );
    /**
     * Adds information about the template used.
     * 
     * @param aTemplateIdentifier Template idetifier obtained from
     *        CMmItemsDataCache::GetTemplateIdentifierL().
     */
    inline void AddTemplateInfo( TInt aTemplateIdentifier );
    /**
     * Adds information whether subcells setup was made for a
     * highlighted item.
     */
    inline void AddIsCurrentInfo( TBool aIsCurrent );
    /**
     * Clears all the information stored in this object so that
     * it can be-reused.
     */
    inline void Clear();
    /**
     * Comparison operator ==.
     * @param aOther object to compare with this object.
     * @return ETrue if objects equal.
     */
    inline TBool operator==( const TMmSubcellsSetupCode aOther ) const;
    /**
     * Comparison operator !=.
     * @param aOther object to compare with this object.
     * @return ETrue if objects differ.
     */
    inline TBool operator!=( const TMmSubcellsSetupCode aOther ) const;

protected: // data
    /**
     * The subcells setup code.
     */
    TUint64 iCode;
    };

#include "mmsubcellssetupcode.inl"

#endif // MMSUBCELLSSETUPCODE_H 
