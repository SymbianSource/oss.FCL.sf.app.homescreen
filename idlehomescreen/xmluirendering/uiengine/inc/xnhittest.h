/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to hold hit region information
*
*/


#ifndef _XNHITTEST_H
#define _XNHITTEST_H

// System includes
#include <e32base.h>
#include <coemain.h>

// Forward declarations
class CXnControlAdapter;

namespace XnHitTest
    {
    const TInt EIgnoreEventsUntilNextPointerUp = 0x01;
    const TInt EIgnoreStylusUpTriggers = 0x02;
    }
 
// Class declaration

/**
 *  Holds hit region information
 * 
 *  @ingroup group_xnlayoutengine
 *  @lib xn3layoutengine.lib
 *  @since S60 5.0
 */    
NONSHARABLE_CLASS( CXnHitTest ) : public CBase,
    public MCoeMessageMonitorObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CXnHitTest* NewL();
    
    /**
     * Two-phased constructor. Leaving on stack
     */    
    static CXnHitTest* NewLC();

    ~CXnHitTest();

private:
    CXnHitTest();

private: 
    // from MCoeMessageMonitorObserver
    
    /**
    * @see MCoeMessageMonitorObserver documentation
    */
    void MonitorWsMessage( const TWsEvent& aEvent );
    
public:
    // New functions

    /**
     * Adds a control to hit region.
     * 
     * @since S60 5.0
     * @param aControl The control to be added.
     */
    void AddControl( CXnControlAdapter* aControl );

    /**
     * Removes a control from hit region.
     * 
     * @since S60 5.0
     * @param aControl The control to be removed.
     */
    void RemoveControl( CXnControlAdapter* aControl );

    /**
     * Set flags
     * 
     * @since S60 5.0
     * @param aFlags flags to be added
     */
    void SetFlags( const TInt aFlags );

    /**
     * Clear flags
     * 
     * @since S60 5.0
     * @param aFlags flags to be cleared, 0x0F default 
     */
    void ClearFlags( const TInt aFlags = 0x0F );

    /**
     * Get flags
     * 
     * @since S60 5.0
     * @return Flags
     */
    TInt Flags() const;

    /**
     * Flushes hit test state
     * 
     * @since S60 5.0	
     */
    void Flush();

    /**
     * Gets hit region count
     * 
     * @since S60 5.0	
     * @return Count
     */
    TInt HitRegionCount() const;

    /**
     * Gets hit region
     * 
     * @since S60 5.0    
     * @return Region
     */
    CXnControlAdapter* HitRegion() const;

    /**
     * Tests whether a pointer event occurred inside the control's hit region.    
     * 
     * @since S60 5.0
     * @param aPointEvent The pointer event.
     * @param aControl
     * @return ETrue if the specified point lies inside the hit region, 
     *         EFalse if not.
     */
    TBool HitRegionContainsL( const TPointerEvent& aPointerEvent,
        const CXnControlAdapter& aControl );

    /**
     * Gets the last pointer event
     * 
     * @since S60 5.0		
     * @return Last pointer event 
     */
    const TPointerEvent& PointerEvent() const;
    
protected:
    /**
     * Two-phased constructor.
     */
    void ConstructL();

private:
    // data

    /** Hit region */
    RPointerArray< CXnControlAdapter > iHitRegion;
    /** Pointer event */
    TPointerEvent iPointerEvent;
    /** Flags */
    TInt iFlags;
    };

#endif // _XNHITTEST_H
