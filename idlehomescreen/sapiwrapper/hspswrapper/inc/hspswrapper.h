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
* Description:  Encapsulates hsps liw service
*
*/



#ifndef C_CHSPSWRAPPER_H
#define C_CHSPSWRAPPER_H

#include <e32base.h>
#include <liwcommon.h>      // for MLiwNotifyCallback

class CLiwServiceHandler;
class MLiwInterface;
class CLiwCriteriaItem;
class CLiwMap;
class CLiwList;
class MDesC8Array;
class CLiwDefaultMap;
class CLiwDefaultList;
class MLiwNotifyCallback;
class CLiwGenericParamList;

namespace hspswrapper{


class CHspsConfiguration;
class CItemMap;
class CPluginInfo;
class CAddPluginResult;
class CPropertyMap;

/**
 *  Observer class for hspswrapper
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib hspswrapper.lib
 *  @since S60 v5.0
 */
class MHspsWrapperObserver
    {
public:
    /**
     * Handle HSPS wrappper notifications 
     *
     * @param aEvent Event
     * @param aAppConfUid App configuration uid
     * @param aPluginName Plugin name
     * @param aOrigUid Originator uid
     * @param aPluginUid Plugin uid
     * @param aPluginId Plugin id
     */
    virtual TInt HandleNotifyL( const TDesC8& aEvent,
                                const TDesC8& aAppConfUid,
                                const TDesC8& aPluginName,
                                const TDesC8& aOrigUid,
                                const TDesC8& aPluginUid,
                                const TDesC8& aPluginId ) = 0;                                  
protected:
    /**
    * Destructor. Protected to prevent deletion through this interface.
    */
    ~MHspsWrapperObserver() { }
    };
    
/**
 *  Return value from add plugin
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib hspswrapper.lib
 *  @since S60 v5.0
 */
class CAddPluginResult: public CBase
    {
public:
    IMPORT_C static CAddPluginResult* NewL();
    IMPORT_C ~CAddPluginResult();
    /**
     * Set plugin id
     *
     * @param aPluginId Plugin id
     */
    IMPORT_C void SetPluginIdL(const TDesC8& aPluginId);
    /**
     * Get plugin id
     *
     * @return Plugin id or KNullDesC8
     */
    IMPORT_C const TDesC8& PluginId()const;
    
    /**
     * Set operation status
     *
     * @param aStatus Operation status
     */
    IMPORT_C void SetStatus(TInt aStatus);
    /**
     * Get operation status
     *
     * @return KErrNotFound, if operation fails
     */
    IMPORT_C TInt Status()const;
private:
    CAddPluginResult();
private:
    /**
     * Status of operation. KErrNotFound, if operation fails
     */
    TInt    iStatus;
    /**
     * Own. Id of the added plugin in the configuration
     */
    HBufC8* iPluginId;    
    };


/**
 *  Wrapper for hsps liw service
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib hspswrapper.lib
 *  @since S60 v5.0
 */
class CHspsWrapper : public CBase, public MLiwNotifyCallback
    {
public:
    /**
     * Two-phased constructor.
     * @param aAppUid Application uid in integer format
     * @param aObserver Observer
     */
    IMPORT_C static CHspsWrapper* NewL(const TDesC8& aAppUid,
                                       MHspsWrapperObserver* aObserver = NULL);
    /**
     * Two-phased constructor.
     * @param aAppUid Application uid in integer format
     * @param aObserver Observer
     */
    IMPORT_C static CHspsWrapper* NewLC(const TDesC8& aAppUid,
                                        MHspsWrapperObserver* aObserver = NULL);
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CHspsWrapper();

    /**
     * Fetch active application configuration from hsps and
     * creates CHspsConfiguration out of liw message
     *
     * @return Configuration. Caller has ownership.
     */
    IMPORT_C CHspsConfiguration* GetAppConfigurationL();

    /**
     * Fetch root configurations from hsps. On return aConfs holds 
     * configuration info instancies. Caller takes ownership of 
     * configuration info instancies.
     *
     * @param aPlugins Array to hold configuration info instancies
      */
    IMPORT_C void GetAppConfigurationsL( RPointerArray<CPluginInfo>& aPlugins );
    
    /**
      * Sets root configuration to active. 
      *
      * @param aConfigurationUid Configuration uid to be activated
      * @return Operation status. KErrNone (success), KErrNotFound
      */
    IMPORT_C TInt SetAppConfigurationL(const TDesC8& aConfigurationUid );    
     
    /**
     * Fetch plugin configuration from hsps and
     * creates CHspsConfiguration out of liw message
     *
     * @param aPluginId Id of plugin configuration
     * @return Configuration. Caller has ownership.
     */
    IMPORT_C CHspsConfiguration* GetPluginConfigurationL(const TDesC8& aPluginId);
    /**
     * Fetch plugins from hsps. On return aPlugins holds plugin info
     * instancies. Caller takes ownership of plugin info instancies.
     *
     * @param aPlugins Array to hold plugin info instancies
     * @param aPluginInterface Plugin interface id
     * @param aPluginType view, widget or template
      */
    IMPORT_C void GetPluginsL(   RPointerArray<CPluginInfo>& aPlugins,
                        const TDesC8& aPluginInterface,
                        const TDesC8& aPluginType);
    
    /**
     * Add plugin to hsps. 
     *
     * @param aTargetConfiguration Id of configuration where the new plugin is attached
     * @param aPluginUid Plugin uid which is added
     * @param aPos Position in configuration's plugin list
     */
    IMPORT_C CAddPluginResult* AddPluginL(const TDesC8& aTargetConfiguration, const TDesC8& aPluginUid);
    
    /**
     * Remove plugin from the active configuration. 
     *
     * @param aPluginId Plugin id to be removed
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt RemovePluginL(const TDesC8& aPluginId);
    
   /**
     * Sets plugin active into active configuration. 
     *
     * @param aPluginId Plugin id to be activated
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt SetActivePluginL(const TDesC8& aPluginId);	

    /**
      * Restore active view. Will remove all plugins in active view.
      *
      * @return Operation status. KErrNone (success), KErrNotFound
      */
     IMPORT_C TInt RestoreActiveViewL();        

     /**
       * Restore whole root configuration. Will remove
       * everything except one view.
       *
       * @return Operation status. KErrNone (success), KErrNotFound
       */
      IMPORT_C TInt RestoreRootL();      
     
    /**
     * Replace plugin in the active configuration.
     *
     * @param aPluginId Plugin id to be replaced
     * @param aConfUid Configuration uid of the new plugin
     * @return Operation status. KErrNone (success), KErrNotFound
     */  	   
    IMPORT_C TInt ReplacePluginL(
            const TDesC8& aPluginId,
            const TDesC8& aConfUid );
    
    /**
     * Move plugin inside configuration. 
     *
     * @param aConfId Configuration
     * @param aPluginIds List of plugins included in the configuration in new order
     * @return Operation status. KErrNone (success), KErrArgument, KErrNotFound
     */
    IMPORT_C TInt MovePluginsL(const TDesC8& aConfId, const MDesC8Array& aPluginIds);
    
    /**
     * Stores defined plugin’s settings. 
     * Plugin is identified with unique plugin ID used in application’s configuration 
     *
     * @param aPluginId Pluging’s ID which settings are stored
     * @param aSettings List of item maps
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt SetPluginSettingsL(const TDesC8& aPluginId, const RPointerArray<CItemMap>& aSettings);
    
    /**
     * Add plugin to hsps. 
     *
     * @param aTargetConfiguration Id of configuration where the new plugin is attached
     * @param aPluginUid Plugin uid which is added
     * @param aIndex Position to which plugin is added
     */
    IMPORT_C CAddPluginResult* AddPluginL(const TDesC8& aTargetConfiguration, const TDesC8& aPluginUid, const TInt aIndex);
    
    /**
     * Set plugin configure state to hsps. 
     *
     * @param aConfId Configuration id of a plugin that should be confirmed.
     * @param aState State of the plugin
     * @param aIncludePlugins ETrue if aState is set to child plugins, EFalse default.
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt SetConfStateL( const TDesC8& aConfId, const TDesC8& aState, TBool aIncludePlugins = EFalse );

    /**
     * Returns SAPI service handler.
     */
    IMPORT_C CLiwServiceHandler* ServiceHandler() const;

    /**
     * Returns HSPS SAPI service
     */   
    IMPORT_C CLiwCriteriaItem* HspsService() const;
    
    /**
     * Returns HSPS services interface
     */   
    IMPORT_C MLiwInterface* HspsInterface() const;

protected:
    CHspsWrapper(MHspsWrapperObserver* aObserver);
    void ConstructL(const TDesC8& aAppUid);
    
    void ProcessConfigurationMapL(const CLiwMap& aSource, CHspsConfiguration& aTarget);
    void ProcessConfigurationPluginsL(const CLiwList& aPluginsList, CHspsConfiguration& aTarget);
    void ProcessConfigurationSettingsL(const CLiwList& aItemList, CHspsConfiguration& aTarget);
    void ProcessConfigurationResourcesL(const CLiwList& aObjectList, CHspsConfiguration& aTarget);
    void ProcessConfItemPropertiesL(const CLiwList& aPropertyMapList,CItemMap& aItemMap);
    void ProcessPluginsL(const CLiwList& aPluginInfoMapList, RPointerArray<CPluginInfo>& aPlugins);
    void FillMapFromItemL( CLiwDefaultMap& aMap, const CItemMap& aItemMap );
    void FillMapFromPropertiesL( CLiwDefaultList& aInPropertyMapList, const RPointerArray<CPropertyMap>& aProperties );

protected: // from MLiwNotifyCallback 

    /**
     * Handle HSPS notifications 
     *
     * @param aCmdId Command id
     * @param aEventId Event id
     * @param aEventParamList Event parameters
     * @param aInParamList In parameters
     */
    TInt HandleNotifyL(TInt aCmdId, TInt aEventId,                        
                       CLiwGenericParamList& aEventParamList,
                       const CLiwGenericParamList& aInParamList);
    
private: // data

    /**
     * Owned. SAPI service handler.
     */
    CLiwServiceHandler* iServiceHandler;
    /**
     * Owned. HSPS SAPI service.
     */
    CLiwCriteriaItem* iHspsService;   
    /**
     * Owned. Provides hsps services.
     */
    MLiwInterface* iHspsInterface;
    /**
     * Not owned. Wrapper observer
     */    
    MHspsWrapperObserver* iObserver;
    /*
     * Asynchronous service request tarnsaction id
     */ 
    TInt iTransactionId;
    };
}

#endif // C_CHSPSWRAPPER_H
