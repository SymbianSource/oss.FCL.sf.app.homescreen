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
* Description:  Helper for online/offline state handling
*
*/

#ifndef ONLINEOFFLINEHELPER_H
#define ONLINEOFFLINEHELPER_H

#include <e32base.h>
#include <NWHandlingEngine.h>
#include <MProfileChangeObserver.h>
#include <babitflags.h>

// Forward declrations
class CNWSession;
class CProfileChangeNotifyHandler;
class CXnNodeAppIf;

namespace AiXmlUiController
{
// Forward declrations
class CXmlUiController;

// Classe declaration


/**
 * Online/offline state helper
 *
 * @ingroup group_aixmluimain
 * @lib ai3xmluimain.lib
 * @since S60 5.0
 */

class COnlineOfflineHelper : public CBase, 
    public MNWMessageObserver, public MProfileChangeObserver
    {    
public:
    // Constructors

    /**
     * Two-phased constructor.
     */
    static COnlineOfflineHelper* NewL( CXmlUiController& aUiCtl );
    
public:
    // Destructor
    
    ~COnlineOfflineHelper();
        
private:
    /*
     * C++ default constructor
     */
    COnlineOfflineHelper( CXmlUiController& aUiCtl );
    
    /**
     *  2nd phase constructor
     */
    void ConstructL();    

public:
    // New functions
    
    /*
     * Determines if online menuitem can be shown
     * 
     * @since S60 5.0
     * @return ETrue if item can be shown, EFalse otherwise
     */
    TBool ShowOnlineItem() const;

    /*
     * Determines if offline menuitem can be shown
     * 
     * @since S60 5.0
     * @return ETrue if item can be shown, EFalse otherwise
     */    
    TBool ShowOfflineItem() const;
    
    /*
     * Check and set whether online/offline state is needed by plugins
     * 
     * @since S60 5.0
     * @param aList List of plugins 
     */
    void ProcessOnlineStateL( RPointerArray< CXnNodeAppIf >& aList );

    /*
     * Set online or offline state based on user selection
     * 
     * @since S60 5.0
     * @param aOnline ETrue to set online, EFalse to offline 
     */    
    void ProcessOnlineStateL( TBool aOnline );
    
private:    
    // from MNWMessageObserver

    /**
     * From MNWMessageObserver.
     * Called by network handling engine when network info changes.
     *
     * @since S60 5.0
     * @param aMessage is type of the change.
     */
    void HandleNetworkMessage( const TNWMessages aMessage );

    /**
     * From MNWMessageObserver.
     * Called if network handling engine fails.
     *
     * @since S60 5.0
     * @param aOperation is failed operation.
     * @param aErrorCode is fail reason.
     */
    void HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode );

private:
    // from MProfileChangeObserver

    /**
     * Called when active profile changes.
     *
     * @since S60 5.0
     * @param aProfileEvent Profile event.
     * @param aProfileId New profile id.
     */
    void HandleActiveProfileEventL( TProfileEvent aProfileEvent, TInt aProfileId );
    
private:
    // New functions
    
    TBool HasNetworkInfoChanged( const TNWMessages aMsg );
    void InterpretNWMessageL( const TNWMessages aMessage, const TNWInfo );
    
    void SetOnlineL( TBool aOnline );
private:
    // Data types
    
    /**
     * Network related message flags
     */    
    enum TNetInfoFlags 
        {
        ENetworkProviderNameReceived        = 0x00000001,
        ENetworkProviderNameOk              = 0x00000002,
        EServiceProviderNameReceived        = 0x00000004,
        EServiceProviderNameOk              = 0x00000008,
        ERegistrationStatusReceived         = 0x00000010,
        ENetworkInfoChangeReceived          = 0x00000020,
        EProgrammableOperatorInfoReceived   = 0x00000040,
        EProgrammableOperatorInfoReceivedOk = 0x00000080
        };
    
    enum
        {
        EOnline,
        EUtilizeOnline,
        ERoamingShown,
        EOfflineProfile
        };
private: 
    // data
    
    /**
     * UI controller
     * Not own
     */    
    CXmlUiController& iUiCtl;
    
    /**
    * Session to network handling engine.
    * Own.
    */
   CNWSession* iSession;

   /**
   * Profile change handler.
   * Own.
   */   
   CProfileChangeNotifyHandler* iHandler;

   /**
    * Cached network info structure.
    */
   TNWInfo iInfo;
   
   /**
    * Previous network information.
    */
   TNWInfo iOldInfo;

   /**
    * Subset of sum of TNetInfoFlags.
    */    
   TUint iReceivedMessageFlags;
   
   /**
    * Subset of sum of old TNetInfoFlags.
    */    
   TUint iOldReceivedMessageFlags;
   
   /**
    * current state of network
    */    
   TNWRegistrationStatus iCurrentNwStatus;

   /**
    * Flags
    */
   TBitFlags32 iFlags;
   };

}// namespace AiXmlUiController
#endif //ONLINEOFFLINEHELPER_H





