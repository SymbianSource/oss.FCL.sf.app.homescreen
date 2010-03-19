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
* Description:  Taskswitcher application ecom plugin
*
*/


#ifndef TSAPPECOM_H
#define TSAPPECOM_H

#include <e32base.h>
#include <akncapserveralternatefsplugin.h>

/**
 * Taskswitcher ecom plugin which is loaded by AknCapServer as an alternate Fast 
 * Swap. Uses P&S to show/dismiss the Taskswitcher application. 
 */
class CTsEcomPlugin : public CAknCapAppServerAlternateFs
    {
public:
    
    /**
     * Symbian two phased constructors.
     *
     * @return CTsEcomPlugin pointer.
     */
    static CTsEcomPlugin* NewL();
    
    /**
     * Destructor.
     */
    ~CTsEcomPlugin();
    
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
    CTsEcomPlugin();
    
    /**
     * 2nd phase construction, called by NewL().
     */
    void ConstructL();
    
    /**
     * Sets the P&S property to given value.
     * 
     * @param aValue Value
     */
    void SetTaskswitcherStateProperty( TInt aValue );
    
    /**
     * Defines the P&S property.
     */
    void DefineTaskswitcherStateProperty();

    };
    
#endif // TSAPPECOM_H
