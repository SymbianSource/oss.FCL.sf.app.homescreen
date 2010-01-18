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
* Description:  
*
*/

#ifndef HSCONTENTCONTROLFACTORY_H
#define HSCONTENTCONTROLFACTORY_H

// System includes
#include <e32base.h>
#include <hscontentcontrolui.h>

// User includes
#include "hscontentcontrolecomobserver.h"
#include "hscontentcontroluninstallobserver.h"

// Forward declarations
class CHsContentControlEComListener;
class CHsContentControlUninstallMonitor;

/**
 *  Content control UI base class
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHsContentControlFactory ) : public CBase,
                                                public MHsContentControlEComObserver,
                                                public MHsContentControlUninstallObserver
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CHsContentControlFactory* NewL();

    /**
     * Destructor.
     */
    IMPORT_C ~CHsContentControlFactory();

private: // from MHsContentControlEComObserver
    
    /**
     * Notification of Ecom registry change.
     */
     void HandleEComChangeEvent();

private: // from MHsContentControlUninstallObserver
    
    /**
     * Notification of Uninstall event from SWI.
     * @param aPkgUid The package UID which is being uninstalled.
     */
     void HandleUninstallEvent( const TUid& aPkgUid );
    
private: // Constructors
    /**
     * Constructor
     */
    CHsContentControlFactory();

    /** Second phase constructor */
    void ConstructL();


public: // New functions
    /**
     * 
     */
    IMPORT_C MHsContentControlUi* GetHsContentController(
        const TDesC8& aControllerType );

private:
    /** 
     * Finds and returns loaded ContentControlUi object from array.
     */
    MHsContentControlUi* FindHsContentController(
        const TDesC8& aControllerType );
    
    /** 
     * Checks if a plugin is removed/upgraded/downgraded.
     */
    void CheckPluginChangesL();
    
    /**
     * Finds plugin implementation info in the ECOM registry.
     * @param aUid The plugin UID which is to be checked.
     * @param aPluginArray The array of plugins which have been implemented.
     * @return ImplementationInfo of plugin.
     */
    CImplementationInformation* FindPluginImplInfo( 
            const TUid& aUid, const RImplInfoPtrArray& aPlugInArray );

    /**
     * Checks if an upgrade or downgrade of the existing plugins happened.
     * @param aPluginImplInfo ImplInfo of plugin to be checked for upgrade/downgrade.
     * @return ETrue if upgrade/downgrade, EFalse otherwise. 
     */
    TBool PluginUpgradeDowngrade( const CImplementationInformation& aPluginImplInfo );

private: // Data

    /**
     * An array of type CHsContentControlUi ( Owned ).
     */
    RPointerArray< CHsContentControlUi > iHsContentControlUis;
    
    /**
    * An array of type CImplementationInformation ( Owned ).
    */
    RImplInfoPtrArray               iImplArray;

    /**
     * An object of type CHsContentControlEComListener ( Owned ).
     */
    CHsContentControlEComListener*  iHsContentControlEComListener;

    /**
     * An object of type CHsContentControlUninstallMonitor ( Owned ).
     */
    CHsContentControlUninstallMonitor*  iHsContentControlUninstallMonitor;
    };


#endif // HSCONTENTCONTROLFACTORY_H

// End of file
