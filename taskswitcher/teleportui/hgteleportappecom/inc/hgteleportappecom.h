/*
* ============================================================================
*  Name        : hgteleportappecom.h
*  Part of     : Teleport
*  Description : Teleport application ecom plugin
*  Version     : %version: sa1spcx1#4.1.3 %
*
*  Copyright © 2009 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#ifndef HGTELEPORTAPPECOM_H
#define HGTELEPORTAPPECOM_H

#include <e32base.h>
#include <akncapserveralternatefsplugin.h>

/**
 * Teleport ecom plugin which is loaded by AknCapServer as an alternate Fast 
 * Swap. Uses P&S to show/dismiss the Teleport application. 
 */
class CHgTeleportEcomPlugin : public CAknCapAppServerAlternateFs
    {
public:
    
    /**
     * Symbian two phased constructors.
     *
     * @return CHgTeleportEcomPlugin pointer.
     */
    static CHgTeleportEcomPlugin* NewL();
    
    /**
     * Destructor.
     */
    ~CHgTeleportEcomPlugin();
    
public: 
    
    // From CAknCapAppServerAlternateFs
    /**
     * Shows the alternate fast swap.
     */
    virtual void Show();

    /**
     * Dismisses the alternate fast swap.
     */
    virtual void Dismiss();

    /**
     * Checks if alternate fast swap is currently visible.
     * 
     * @return ETrue If alternate FS is visible.
     */    
    virtual TBool IsVisible();

    /**
     * Checks if alternate fast swap is ready to be shown.
     * 
     * @return ETrue If alternate FS is ready.
     */    
    virtual TBool IsReady();

    /**
     * Forward long app key to the alternate fast swap.
     */
    virtual void HandleLongAppKeyPress();

    /**
     * Forward short app key to the alternate fast swap.
     */
    virtual void HandleShortAppKeyPress();

private:

    /**
     * Default constructor, implicitly called by NewL().
     */
    CHgTeleportEcomPlugin();
    
    /**
     * 2nd phase construction, called by NewL().
     */
    void ConstructL();
    
    /**
     * Sets the P&S property to given value.
     * 
     * @param aValue Value
     */
    void SetTeleportStateProperty( TInt aValue );
    
    /**
     * Defines the P&S property.
     */
    void DefineTeleportStateProperty();

    };
    
#endif // HGTELEPORTAPPECOM_H
