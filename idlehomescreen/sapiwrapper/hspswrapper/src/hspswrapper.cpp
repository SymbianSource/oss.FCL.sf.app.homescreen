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


#include <liwservicehandler.h>
#include <liwvariant.h>
#include <bamdesca.h>

#include "hspswrapper.h"
#include "hspsconfiguration.h"
#include "pluginmap.h"
#include "itemmap.h"
#include "propertymap.h"
#include "objectmap.h"
#include "plugininfo.h"


_LIT8( KHSPS, "Service.HSPS" );
_LIT8( KHSPSConfigurationIf, "IConfiguration" );

_LIT8( KHSPSCommandGetActiveAppConf, "GetActiveAppConf" );
_LIT8( KHSPSCommandGetPluginConf, "GetPluginConf" );
_LIT8( KHSPSCommandGetPlugins, "GetPlugins" );
_LIT8( KHSPSCommandAddPlugin, "AddPlugin" );
_LIT8( KHSPSCommandRemovePlugin, "RemovePlugin" );
_LIT8( KHSPSCommandReplacePlugin, "ReplacePlugin" );
_LIT8( KHSPSCommandSetActivePlugin, "SetActivePlugin" );
_LIT8( KHSPSCommandSetConfState, "SetConfState" );
_LIT8( KHSPSCommandGetAppConfs, "GetAppConfs" );
_LIT8( KHSPSCommandSetAppConf, "SetActiveAppConf" );
_LIT8( KHSPSMovePlugins, "MovePlugins");
_LIT8( KHSPSSetPluginSettings, "SetPluginSettings" );
_LIT8( KAppConfKey, "appConf" );
_LIT8( KAppConfsKey, "appConfs" );
_LIT8( KPluginConfKey, "pluginConf" );
_LIT8( KPluginsKey, "plugins" );
_LIT8( KConfId, "confId" );
_LIT8( KPluginUid, "pluginUid" );
_LIT8( KOutKeyStatus ,"status");
_LIT8( KHspsAppUid, "appUid" );
_LIT8( KKeyPluginId, "pluginId" );
_LIT8( KKeyConfUid, "confUid" );
_LIT8( KInterface, "interface" );
_LIT8( KType, "type" );
_LIT8( KKeyPlugins, "plugins" );
_LIT8( KKeyItemId, "itemId" );
_LIT8( KKeyName, "name" );
_LIT8( KKeyValue, "value" );
_LIT8( KKeyProperties, "properties" );
_LIT8( KKeySettings, "settings" );
_LIT8( KIndex, "position" );
_LIT8( KConfState, "state" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KHSPSCommandRestoreConfigurations, "RestoreConfigurations" );
_LIT8( KRestore, "restore" );
_LIT8( KActive, "active" );
_LIT8( KAll, "all" );

namespace hspswrapper{

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CAddPluginResult::CAddPluginResult():iStatus(KErrNotFound)
    {}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CAddPluginResult::~CAddPluginResult()
    {
    delete iPluginId;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CAddPluginResult* CAddPluginResult::NewL()
    {
    CAddPluginResult* self = new( ELeave ) CAddPluginResult;
    return self;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CAddPluginResult::SetPluginIdL(
    const TDesC8& aPluginId )
    {
    delete iPluginId;
    iPluginId = NULL;
    iPluginId = aPluginId.AllocL();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CAddPluginResult::PluginId()const
    {
    if(iPluginId)
        {
        return *iPluginId;
        }
    else
        {
        return KNullDesC8;
        }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CAddPluginResult::SetStatus(TInt aStatus )
    {
    iStatus = aStatus;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAddPluginResult::Status()const
    {
    return iStatus;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHspsWrapper::CHspsWrapper(MHspsWrapperObserver* aObserver)
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ConstructL(const TDesC8& aAppUid)
    {
    // Attach to HSPS:
    iServiceHandler = CLiwServiceHandler::NewL();
    
    iHspsService = CLiwCriteriaItem::NewL( 1, KHSPSConfigurationIf, KHSPS );
    iHspsService->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    
    RCriteriaArray interestList;
    interestList.AppendL( iHspsService );
    iServiceHandler->AttachL( interestList );
    interestList.Reset();
    
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam appUid;
    TLiwVariant uidVar;
    uidVar.Set( aAppUid );
    appUid.SetNameAndValueL( KHspsAppUid, uidVar );
    appUid.PushL();
    inParamList.AppendL( appUid );
    CleanupStack::Pop();
    appUid.Reset();
    
    iServiceHandler->ExecuteServiceCmdL( *iHspsService,inParamList,outParamList );
    inParamList.Reset();
    TInt pos = 0;
    outParamList.FindFirst( pos, KHSPSConfigurationIf );
    if( pos != KErrNotFound )
        {
        iHspsInterface = outParamList[ pos ].Value().AsInterface();    
        outParamList.Reset();        
        }
    else
        {
        outParamList.Reset();       
        User::Leave( KErrNotFound );
        }   
    
    inParamList.Reset();
    outParamList.Reset();
        
    if( iObserver )
        {
        iTransactionId = -1;
        iHspsInterface->ExecuteCmdL( KRequestNotification,
                                     inParamList,
                                     outParamList,
                                     KLiwOptASyncronous,
                                     this );   
        const TLiwGenericParam* outParam( NULL );
                
       TInt pos( 0 );
       outParam = outParamList.FindFirst( pos, _L8("status") );
               
       if ( outParam )
           {
           TInt retval;
           retval = outParam->Value().AsTInt32();
           if(retval == KErrNone )
               {
               pos = 0;
               outParam = outParamList.FindFirst( pos, _L8("TransactionID") );
               if( outParam )
                   {
                   retval = outParam->Value().AsTInt32();
                   iTransactionId = retval;
                   }
               }
          
           }
                  
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsWrapper* CHspsWrapper::NewL(const TDesC8& aAppUid,
                                          MHspsWrapperObserver* aObserver)
    {
    CHspsWrapper* self = CHspsWrapper::NewLC(aAppUid, aObserver);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsWrapper* CHspsWrapper::NewLC(const TDesC8& aAppUid,
                                           MHspsWrapperObserver* aObserver)
    {
    CHspsWrapper* self = new( ELeave ) CHspsWrapper( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL(aAppUid);
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsWrapper::~CHspsWrapper()
    {
    if( iHspsInterface )
        {
        // Close interface
        iHspsInterface->Close();       
        }
    
    if( iServiceHandler && iHspsService )
        {
        // Detach services from the handler
        RCriteriaArray interestList;
        TRAP_IGNORE( interestList.AppendL( iHspsService ) );
        TRAP_IGNORE( iServiceHandler->DetachL( interestList ) );   
        interestList.Reset();        
        }
    
    delete iHspsService;
    delete iServiceHandler;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration* CHspsWrapper::GetAppConfigurationL()
    {
    CHspsConfiguration* configuration = CHspsConfiguration::NewLC();
    
    // Fetch from liw service
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
       
    iHspsInterface->ExecuteCmdL( KHSPSCommandGetActiveAppConf, 
                                 inParamList, 
                                 outParamList );
                                           
    inParamList.Reset();
   
    // 1: get appConf map
    TInt index(0);
    const TLiwGenericParam* conf = outParamList.FindFirst(index,KAppConfKey);
    if(conf)
        {
        const CLiwMap* confMap  = conf->Value().AsMap();
        // 2: Process Configuration map
        if(confMap)
           {
           ProcessConfigurationMapL(*confMap,*configuration);
           }
        }
    outParamList.Reset();
    
    CleanupStack::Pop(configuration);
    return configuration;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHspsWrapper::GetAppConfigurationsL(
    RPointerArray<CPluginInfo>& aPlugins )        
    {
    // Fetch from liw service
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
       
    iHspsInterface->ExecuteCmdL( KHSPSCommandGetAppConfs, 
                                 inParamList, 
                                 outParamList );
                                           
    inParamList.Reset();
   
    TInt index(0);
    const TLiwGenericParam* confs = outParamList.FindFirst(index,KAppConfsKey);
    if(confs)
        {
        const CLiwList* list = confs->Value().AsList();
        if(list)
            {
            TInt listCount = list->Count();
            TLiwVariant mapVar;
            mapVar.PushL();
            
            for ( TInt index(0); list->AtL( index, mapVar ); ++index )
                {                               
                const CLiwMap* confMap = mapVar.AsMap();
                
                if ( confMap )
                    {
                    CPluginInfo* plugin = CPluginInfo::NewLC();
                    
                    TLiwVariant confVar;
                    confVar.PushL();
                    
                    if( confMap->FindL( _L8("uid"), confVar ) )
                        {
                        plugin->SetUidL( confVar.AsData() );
                        }
                    
                    confVar.Reset();
                    
                    if( confMap->FindL(_L8("name"), confVar ) )
                        {
                        plugin->SetNameL( confVar.AsData() );
                        }
                    
                    CleanupStack::Pop(&confVar);
                    confVar.Reset();
                    
                    aPlugins.AppendL( plugin );
                    CleanupStack::Pop( plugin );
                    }
                }
            CleanupStack::Pop(&mapVar);
            mapVar.Reset();
            }
        }
    outParamList.Reset();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::SetAppConfigurationL(
    const TDesC8& aConfigurationUid )
    {
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CHspsWrapper::SetAppConfigurationL() - start" ) );
#endif //_XN_PERFORMANCE_TEST_        
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam confIdParam;
    confIdParam.SetNameAndValueL( KKeyConfUid, TLiwVariant(aConfigurationUid) );
    confIdParam.PushL();
    inParamList.AppendL( confIdParam );
    CleanupStack::Pop(&confIdParam);
    confIdParam.Reset();
      
    iHspsInterface->ExecuteCmdL( KHSPSCommandSetAppConf, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
#ifdef _XN_PERFORMANCE_TEST_
    RDebug::Print( _L( "CHspsWrapper::SetAppConfigurationL() - end" ) );
#endif //_XN_PERFORMANCE_TEST_            
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration* CHspsWrapper::GetPluginConfigurationL(
    const TDesC8& aPluginId)
    {
    CHspsConfiguration* configuration = CHspsConfiguration::NewLC();
        
    // Fetch from liw service
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    // Form input message
    
    TLiwGenericParam pluginIdParam;
    TLiwVariant pluginIdVar;
    pluginIdVar.Set( aPluginId );
    pluginIdParam.SetNameAndValueL( KKeyPluginId, pluginIdVar );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop(&pluginIdParam);
    pluginIdParam.Reset();
        
    
    iHspsInterface->ExecuteCmdL( KHSPSCommandGetPluginConf, 
                                 inParamList, 
                                 outParamList );
    inParamList.Reset();
    
    // 1: get pluginConf map
    TInt index(0);
    const TLiwGenericParam* conf = outParamList.FindFirst(index,KPluginConfKey);
    if(conf)
        {
        const CLiwMap* confMap  = conf->Value().AsMap();
        // 2: Process Configuration map
        if(confMap)
           {
           ProcessConfigurationMapL(*confMap,*configuration);
           }
        }
   
    outParamList.Reset();
    CleanupStack::Pop(configuration);
    
    return configuration;
    } 

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHspsWrapper::GetPluginsL(
    RPointerArray<CPluginInfo>& aPlugins,
    const TDesC8& aPluginInterface,
    const TDesC8& aPluginType)
    {
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    // plugin interface
    TLiwGenericParam interfaceParam( KInterface, TLiwVariant( aPluginInterface ) );
    interfaceParam.PushL();
    inParamList.AppendL( interfaceParam );
    CleanupStack::Pop(&interfaceParam);
    interfaceParam.Reset();
    
    // plugin type
    TLiwGenericParam typeParam;
    typeParam.SetNameAndValueL( KType, TLiwVariant(aPluginType ) );
    typeParam.PushL();
    inParamList.AppendL( typeParam );
    CleanupStack::Pop(&typeParam);
    typeParam.Reset();

    iHspsInterface->ExecuteCmdL( KHSPSCommandGetPlugins, inParamList, outParamList ); 
    inParamList.Reset();
    
    TInt index(0);
    const TLiwGenericParam* plugins = outParamList.FindFirst(index,KPluginsKey);
    if(plugins)
        {
        const CLiwList* list = plugins->Value().AsList();
        if(list)
            {
            ProcessPluginsL(*list,aPlugins);
            }
        }
    outParamList.Reset();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CAddPluginResult* CHspsWrapper::AddPluginL(
    const TDesC8& aTargetConfiguration, 
    const TDesC8& aPluginUid)
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
      
    TLiwGenericParam confIdParam;
    confIdParam.SetNameAndValueL( KConfId, TLiwVariant(aTargetConfiguration) );
    confIdParam.PushL();
    inParamList.AppendL( confIdParam );
    CleanupStack::Pop(&confIdParam);
    confIdParam.Reset();
       
    TLiwGenericParam pluginUidParam;
    pluginUidParam.SetNameAndValueL( KPluginUid, TLiwVariant(aPluginUid) );
    pluginUidParam.PushL();
    inParamList.AppendL( pluginUidParam );
    CleanupStack::Pop(&pluginUidParam);
    pluginUidParam.Reset();
       
    // add widget to configuration
    iHspsInterface->ExecuteCmdL( KHSPSCommandAddPlugin, 
                                 inParamList, 
                                 outParamList ); 
    inParamList.Reset();
    
    CAddPluginResult* result = CAddPluginResult::NewL();
    CleanupStack::PushL(result);
    // test success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, _L8("status") );
    if ( outParam )
       {
       result->SetStatus(outParam->Value().AsTInt32());
       }
    else
        {
        result->SetStatus(KErrNone);
        pos = 0;
        outParam = outParamList.FindFirst( pos, _L8("pluginId") );
        if ( outParam )
            {
            result->SetPluginIdL(outParam->Value().AsData());
            }
        else
            {
            // should never happen, but change status back to KErrNotFound 
            result->SetStatus(KErrNotFound);
            }
        }
  
    CleanupStack::Pop(result);
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CAddPluginResult* CHspsWrapper::AddPluginL(
    const TDesC8& aTargetConfiguration, 
    const TDesC8& aPluginUid,
    const TInt aIndex )
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
      
    TLiwGenericParam confIdParam;
    confIdParam.SetNameAndValueL( KConfId, TLiwVariant(aTargetConfiguration) );
    confIdParam.PushL();
    inParamList.AppendL( confIdParam );
    CleanupStack::Pop(&confIdParam);
    confIdParam.Reset();
   
    TLiwGenericParam pluginUidParam;
    pluginUidParam.SetNameAndValueL( KPluginUid, TLiwVariant(aPluginUid) );
    pluginUidParam.PushL();
    inParamList.AppendL( pluginUidParam );
    CleanupStack::Pop(&pluginUidParam);
    pluginUidParam.Reset();
    
    if(aIndex >= 0)
        {
        TBuf8<32> buf;
        buf.Num( aIndex );
        TLiwGenericParam indexParam;
        indexParam.SetNameAndValueL( KIndex, TLiwVariant( buf ) );
        indexParam.PushL();
        inParamList.AppendL( indexParam );
        CleanupStack::Pop(&indexParam);
        indexParam.Reset();
        }
    
    // add widget to configuration
    iHspsInterface->ExecuteCmdL( KHSPSCommandAddPlugin, 
                                 inParamList, 
                                 outParamList ); 
    inParamList.Reset();
    
    CAddPluginResult* result = CAddPluginResult::NewL();
    CleanupStack::PushL(result);
    // test success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, _L8("status") );
    if ( outParam )
       {
       result->SetStatus(outParam->Value().AsTInt32());
       }
    else
        {
        result->SetStatus(KErrNone);
        pos = 0;
        outParam = outParamList.FindFirst( pos, _L8("pluginId") );
        if ( outParam )
            {
            result->SetPluginIdL(outParam->Value().AsData());
            }
        else
            {
            // should never happen, but change status back to KErrNotFound 
            result->SetStatus(KErrNotFound);
            }
        }
  
    CleanupStack::Pop(result);
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::RemovePluginL(const TDesC8& aPluginId)
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop(&pluginIdParam);
    pluginIdParam.Reset();
      
    iHspsInterface->ExecuteCmdL( KHSPSCommandRemovePlugin, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::ReplacePluginL( 
        const TDesC8& aPluginId, 
        const TDesC8& aConfUid )
    {
    // Compose ReplacePlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop(&pluginIdParam);
    pluginIdParam.Reset();
    
    TLiwGenericParam confUidParam;
    confUidParam.SetNameAndValueL( KKeyConfUid, TLiwVariant( aConfUid ) );
    confUidParam.PushL();
    inParamList.AppendL( confUidParam );
    CleanupStack::Pop( &confUidParam );
    confUidParam.Reset();
      
    // Execute command
    iHspsInterface->ExecuteCmdL( KHSPSCommandReplacePlugin, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status( KErrGeneral );
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::SetActivePluginL(const TDesC8& aPluginId)
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop(&pluginIdParam);
    pluginIdParam.Reset();
      
    iHspsInterface->ExecuteCmdL( KHSPSCommandSetActivePlugin, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::RestoreActiveViewL()
    {
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam restoreTypeParam;
    restoreTypeParam.SetNameAndValueL( KRestore, TLiwVariant( KActive ) );
    restoreTypeParam.PushL();
    inParamList.AppendL( restoreTypeParam );
    CleanupStack::Pop( &restoreTypeParam );
    restoreTypeParam.Reset();
      
    iHspsInterface->ExecuteCmdL( KHSPSCommandRestoreConfigurations, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::RestoreRootL()
    {
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam restoreTypeParam;
    restoreTypeParam.SetNameAndValueL( KRestore, TLiwVariant( KAll ) );
    restoreTypeParam.PushL();
    inParamList.AppendL( restoreTypeParam );
    CleanupStack::Pop( &restoreTypeParam );
    restoreTypeParam.Reset();
      
    iHspsInterface->ExecuteCmdL( KHSPSCommandRestoreConfigurations, 
                                 inParamList, 
                                 outParamList ); 
      
    inParamList.Reset();
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::MovePluginsL(
    const TDesC8& aConfId, 
    const MDesC8Array& aPluginIds)
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam confIdParam;
    confIdParam.SetNameAndValueL( KConfId, TLiwVariant(aConfId) );
    confIdParam.PushL();
    inParamList.AppendL( confIdParam );
    CleanupStack::Pop( &confIdParam );
    confIdParam.Reset();    
       
    // add "plugins" parameter to the inParamList
    CLiwDefaultList* inPluginList = CLiwDefaultList::NewLC();
    for( int i = 0; i < aPluginIds.MdcaCount(); i++ )
        {
        const TDesC8& id = aPluginIds.MdcaPoint(i);
        inPluginList->AppendL( TLiwVariant(id) );
        }
       
    TLiwGenericParam pluginsParam;
    pluginsParam.SetNameAndValueL( KKeyPlugins, TLiwVariant(inPluginList) );
    pluginsParam.PushL();
    inParamList.AppendL( pluginsParam );
    CleanupStack::Pop( &pluginsParam );
    pluginsParam.Reset();

    iHspsInterface->ExecuteCmdL( KHSPSMovePlugins, 
                                 inParamList, 
                                 outParamList ); 

    CleanupStack::PopAndDestroy( inPluginList );    
    inParamList.Reset();
    
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::SetPluginSettingsL(
    const TDesC8& aPluginId, 
    const RPointerArray<CItemMap>& aSettings)
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop( &pluginIdParam );
    pluginIdParam.Reset();    
       
    CLiwDefaultList* itemMapList = CLiwDefaultList::NewLC();
    for( int i = 0; i < aSettings.Count(); i++ )
        {
        CItemMap* itemMap = aSettings[i];
        
        CLiwDefaultMap* inItemMap = CLiwDefaultMap::NewLC();
        FillMapFromItemL(*inItemMap,*itemMap);
        itemMapList->AppendL( TLiwVariant(inItemMap) );
        CleanupStack::PopAndDestroy(inItemMap);
        }
       
    TLiwGenericParam settingsParam;
    settingsParam.SetNameAndValueL( KKeySettings, TLiwVariant(itemMapList) );
    settingsParam.PushL();
    inParamList.AppendL( settingsParam );
    CleanupStack::Pop( &settingsParam );
    settingsParam.Reset();
    
    iHspsInterface->ExecuteCmdL( KHSPSSetPluginSettings, 
                                 inParamList, 
                                 outParamList ); 
    
    CleanupStack::PopAndDestroy( itemMapList );  
    inParamList.Reset();
    
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHspsWrapper::SetConfStateL( const TDesC8& aConfId, 
    const TDesC8& aState, TBool aIncludePlugins )
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
         
    // Compose Liw message
    TLiwGenericParam confIdParam;
    confIdParam.SetNameAndValueL( KConfId, TLiwVariant( aConfId ) );
    confIdParam.PushL();
    inParamList.AppendL( confIdParam );
    CleanupStack::Pop(&confIdParam);
    confIdParam.Reset();
    
    TLiwGenericParam pluginStateParam;
    pluginStateParam.SetNameAndValueL( KConfState, TLiwVariant( aState ) );
    pluginStateParam.PushL();
    inParamList.AppendL( pluginStateParam );
    CleanupStack::Pop(&pluginStateParam);
    pluginStateParam.Reset();

    iHspsInterface->ExecuteCmdL( KHSPSCommandSetConfState, 
                                 inParamList, 
                                 outParamList ); 

    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    
    outParamList.Reset();
        
    if( !status && aIncludePlugins )
        {
        _LIT8( KFilter, "filter" );
        _LIT8( KFilterPlugins, "Plugins" );
        
        TLiwGenericParam filterParam;
        filterParam.PushL();
        filterParam.SetNameAndValueL( KFilter, TLiwVariant( KFilterPlugins ) );        
        inParamList.AppendL( filterParam );
        CleanupStack::Pop(&filterParam);
        filterParam.Reset();
        
        iHspsInterface->ExecuteCmdL( KHSPSCommandSetConfState, 
                                     inParamList, 
                                     outParamList );
        
        // check success
        pos = 0;
        outParam = outParamList.FindFirst( pos, KOutKeyStatus );
        status = KErrGeneral;
        
        if ( outParam )
            {
            status = outParam->Value().AsTInt32();
            }

        outParamList.Reset();        
        }
            
    inParamList.Reset();
      
    return status;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CLiwServiceHandler* CHspsWrapper::ServiceHandler() const
    {
    return iServiceHandler;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
EXPORT_C CLiwCriteriaItem* CHspsWrapper::HspsService() const
    {
    return iHspsService;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C MLiwInterface* CHspsWrapper::HspsInterface() const
    {
    return iHspsInterface;
    }
        
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessConfigurationMapL(
    const CLiwMap& aSource, 
    CHspsConfiguration& aTarget)
    {
    TLiwVariant tempVariant;
    tempVariant.PushL();
        
    if ( aSource.FindL( _L8("id"), tempVariant ) )
        {
        aTarget.SetConfIdL(tempVariant.AsData());
        }
    if ( aSource.FindL( _L8("uid"), tempVariant ) )
        {
        aTarget.PluginInfo().SetUidL(tempVariant.AsData());
        }
    if ( aSource.FindL( _L8("type"), tempVariant ) )
        {
        aTarget.PluginInfo().SetTypeL(tempVariant.AsData());
        }
    if ( aSource.FindL( _L8("interface"), tempVariant ) )
        {
        aTarget.PluginInfo().SetInterfaceL(tempVariant.AsData());
        }        
    if ( aSource.FindL( _L8("name"), tempVariant ) )
        {
        aTarget.PluginInfo().SetNameL(tempVariant.AsData());
        }   
    if ( aSource.FindL( _L8("state"), tempVariant ) )
        {
        aTarget.PluginInfo().SetConfigurationStateL(tempVariant.AsData());
        }        
    if ( aSource.FindL( _L8("multiinstance"), tempVariant ) )
        {
        aTarget.PluginInfo().SetMultiInstanceL( tempVariant.AsTInt32() );
        }
    if ( aSource.FindL( _L8( "max_child" ), tempVariant ) )
        {
        aTarget.PluginInfo().SetMaxChild( tempVariant.AsTInt32() );
        }    
    if( aSource.FindL( _L8("plugins"), tempVariant ) )
        {
        const CLiwList* plugins( tempVariant.AsList() );
        if( plugins )
            {
            ProcessConfigurationPluginsL(*plugins,aTarget);
            }
        }
    if( aSource.FindL( _L8("settings"), tempVariant ) )
        {
        const CLiwList* settings( tempVariant.AsList() );
        if( settings )
            {
            ProcessConfigurationSettingsL(*settings,aTarget);
            }
        }
    if( aSource.FindL( _L8("resources"), tempVariant ) )
        {
        const CLiwList* resources( tempVariant.AsList() );
        if( resources )
            {
            ProcessConfigurationResourcesL(*resources,aTarget);
            }
        }
    CleanupStack::Pop(&tempVariant);
    tempVariant.Reset();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessConfigurationPluginsL(
    const CLiwList& aPluginsList, 
    CHspsConfiguration& aTarget)
    {
    TLiwVariant pluginMapVariant;
    pluginMapVariant.PushL();
    for( int i=0;i<aPluginsList.Count();++i )
        {
        if( aPluginsList.AtL(i,pluginMapVariant) )
            {
            const CLiwMap* pluginMap( pluginMapVariant.AsMap() );
            TLiwVariant pluginVariant;
            pluginVariant.PushL();
            if( pluginMap )
                {
                CPluginMap* plugin = CPluginMap::NewLC();
                if( pluginMap->FindL( _L8("id"),pluginVariant) )
                    {
                    plugin->SetPluginIdL( pluginVariant.AsData() );
                    }
                if( pluginMap->FindL( _L8("uid"),pluginVariant) )
                    {
                    plugin->SetPluginUidL( pluginVariant.AsData() );
                    }
                if( pluginMap->FindL( _L8("activationstate"),pluginVariant ) )
                    {
                    plugin->SetActivationStateL( pluginVariant.AsData() );
                    }
                if ( pluginMap->FindL( _L8( "locking_status" ), pluginVariant ) )
                    {
                    plugin->SetLockingStatusL( pluginVariant.AsData() );
                    }
                aTarget.AddPluginMapL(plugin);                    
                CleanupStack::Pop(plugin);
                }
            CleanupStack::Pop(&pluginVariant);
            pluginVariant.Reset();
            }
       
        }
    CleanupStack::Pop(&pluginMapVariant);
    pluginMapVariant.Reset();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessConfigurationSettingsL(
    const CLiwList& aItemList, 
    CHspsConfiguration& aTarget)
    {
    TLiwVariant itemMapVariant;
    itemMapVariant.PushL();
    for( int i=0;i<aItemList.Count();++i )
        {
        if( aItemList.AtL(i,itemMapVariant) )
            {
            const CLiwMap* itemMap( itemMapVariant.AsMap() );
            TLiwVariant itemVariant;
            itemVariant.PushL();
            if( itemMap )
                {
                CItemMap* item = CItemMap::NewLC();
                if( itemMap->FindL( _L8("itemId"),itemVariant) )
                    {
                    item->SetItemIdL( itemVariant.AsData() );
                    }
                if( itemMap->FindL( _L8("name"),itemVariant) )
                    {
                    item->SetItemNameL( itemVariant.AsData() );
                    }
                if( itemMap->FindL( _L8("properties"),itemVariant) )
                    {
                    const CLiwList* properties( itemVariant.AsList() );
                    if(properties)
                        {
                        ProcessConfItemPropertiesL(*properties,*item);
                        }
                    }
                    
                aTarget.AddItemMapL(item);                    
                CleanupStack::Pop(item);
                }
            CleanupStack::Pop(&itemVariant);
            itemVariant.Reset();
            }
       
        }
    CleanupStack::Pop(&itemMapVariant);
    itemMapVariant.Reset();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessConfigurationResourcesL(
    const CLiwList& aObjectList, 
    CHspsConfiguration& aTarget)
    {
    TLiwVariant objectMapVariant;
    objectMapVariant.PushL();
    for( int i=0;i<aObjectList.Count();++i )
        {
        if( aObjectList.AtL(i,objectMapVariant) )
            {
            const CLiwMap* objectMap( objectMapVariant.AsMap() );
            TLiwVariant objectVariant;
            objectVariant.PushL();
            if( objectMap )
                {
                CObjectMap* object = CObjectMap::NewLC();
                if( objectMap->FindL( _L8("name"),objectVariant) )
                    {
                    object->SetNameL( objectVariant.AsData() );
                    }
                if( objectMap->FindL( _L8("path"),objectVariant) )
                    {
                    object->SetPathL( objectVariant.AsData() );
                    }
                if( objectMap->FindL( _L8("mediatype"),objectVariant) )
                    {
                    object->SetMediaTypeL( objectVariant.AsData() );
                    }    
                if( objectMap->FindL( _L8("tag"),objectVariant) )
                    {
                    object->SetTagL( objectVariant.AsData() );
                    }
                aTarget.AddObjectMapL(object);                    
                CleanupStack::Pop(object);
                }
            CleanupStack::Pop(&objectVariant);
            objectVariant.Reset();
            }
        }
    CleanupStack::Pop(&objectMapVariant);
    objectMapVariant.Reset();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessConfItemPropertiesL(
    const CLiwList& aPropertyMapList,
    CItemMap& aItemMap)
    {
    TLiwVariant propertyMapVariant;
    propertyMapVariant.PushL();
    for( int i=0;i<aPropertyMapList.Count();++i )
       {
       if( aPropertyMapList.AtL(i,propertyMapVariant) )
           {
           const CLiwMap* propertyMap( propertyMapVariant.AsMap() );
           TLiwVariant propertyVariant;
           propertyVariant.PushL();
           if( propertyMap )
               {
               CPropertyMap* property = CPropertyMap::NewLC();
               if( propertyMap->FindL( _L8("name"),propertyVariant ) )
                   {
                   property->SetNameL( propertyVariant.AsData() );
                   }
               if( propertyMap->FindL( _L8("value"),propertyVariant ) )
                   {
                   property->SetValueL( propertyVariant.AsData() );
                   }
               aItemMap.AddPropertyMapL(property);                    
               CleanupStack::Pop(property);
               }
           CleanupStack::Pop(&propertyVariant);
           propertyVariant.Reset();
           }
      
       }
    CleanupStack::Pop(&propertyMapVariant);
    propertyMapVariant.Reset();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::ProcessPluginsL(
    const CLiwList& aPluginInfoMapList, 
    RPointerArray<CPluginInfo>& aPlugins)
    {
    // create widget array and populate it from HSPS list
    TLiwVariant mapVar;
    mapVar.PushL();
    
    for ( int index(0); aPluginInfoMapList.AtL( index, mapVar ); ++index )
        {
        const CLiwMap* pluginInfoMap = mapVar.AsMap();
        if ( pluginInfoMap )
            {
            TLiwVariant pluginVar;
            pluginVar.PushL();
            CPluginInfo* pluginInfo = CPluginInfo::NewLC();
            
            if( pluginInfoMap->FindL( _L8("uid"), pluginVar) )
                {
                pluginInfo->SetUidL( pluginVar.AsData() );
                }
            if( pluginInfoMap->FindL( _L8("interface"), pluginVar ) )
                {
                pluginInfo->SetInterfaceL( pluginVar.AsData() );
                }
            if( pluginInfoMap->FindL( _L8("type"), pluginVar ) )
                {
                pluginInfo->SetTypeL( pluginVar.AsData() );
                }
            if( pluginInfoMap->FindL( _L8("name"), pluginVar ) )
                {
                pluginInfo->SetNameL( pluginVar.AsData() );
                }
            if ( pluginInfoMap->FindL( _L8("multiinstance"), pluginVar ) )
                {
                pluginInfo->SetMultiInstanceL( pluginVar.AsTInt32() );
                }
            if ( pluginInfoMap->FindL( _L8( "max_child" ), pluginVar ) )
                {
                pluginInfo->SetMaxChild( pluginVar.AsTInt32() );
                }
            if ( pluginInfoMap->FindL( _L8("desc"), pluginVar ) )
                {
                pluginInfo->SetDescriptionL( pluginVar.AsData() );
                }
            if ( pluginInfoMap->FindL( _L8("logo"), pluginVar ) )
                {
                pluginInfo->SetLogoIconL( pluginVar.AsData() );
                }

            aPlugins.AppendL(pluginInfo);
            CleanupStack::Pop(pluginInfo);
            CleanupStack::Pop(&pluginVar);
            pluginVar.Reset();
            }
        }
    CleanupStack::Pop(&mapVar);
    mapVar.Reset();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::FillMapFromItemL( CLiwDefaultMap& aMap, const CItemMap& aItemMap )
    {
    aMap.InsertL(  KKeyItemId, TLiwVariant(aItemMap.ItemId()) );
    aMap.InsertL(  KKeyName, TLiwVariant(aItemMap.ItemName()) );
    
    CLiwDefaultList* inPropertyMapList = CLiwDefaultList::NewLC();
    FillMapFromPropertiesL(*inPropertyMapList,aItemMap.Properties());
    aMap.InsertL(  KKeyProperties, TLiwVariant(inPropertyMapList) );
    CleanupStack::PopAndDestroy(inPropertyMapList);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsWrapper::FillMapFromPropertiesL( 
    CLiwDefaultList& aInPropertyMapList, 
    const RPointerArray<CPropertyMap>& aProperties )
    {
    
    for(int i=0; i<aProperties.Count(); ++i)
        {
        CLiwDefaultMap* inPropertyMap = CLiwDefaultMap::NewLC();
        inPropertyMap->InsertL(  KKeyName, TLiwVariant(aProperties[i]->Name()) );
        inPropertyMap->InsertL(  KKeyValue, TLiwVariant(aProperties[i]->Value()) );
        aInPropertyMapList.AppendL(inPropertyMap);
        CleanupStack::PopAndDestroy(inPropertyMap);
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
TInt CHspsWrapper::HandleNotifyL( TInt aCmdId, TInt aEventId,                        
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    TInt retval( KErrNone );
   
    if( iObserver && iTransactionId == aCmdId )
        {                 
        const TLiwGenericParam* outParam( NULL );
        
        TInt pos( 0 );
        outParam = aEventParamList.FindFirst( pos, _L8("status") );
        
        if ( outParam )
            {    
            retval = outParam->Value().AsTInt32();
            }
        else
            {   
            pos = 0;
            
            retval = KErrNotFound;
                
            outParam = aEventParamList.FindFirst( pos, _L8("notification") );
            
            if( !outParam )
                {      
                // should never happen, but change status back to KErrNotFound 
                return retval;                
                }
                
            HBufC8* event( NULL );
            HBufC8* appConfUid( NULL );
            HBufC8* pluginUid( NULL );
            HBufC8* pluginName( NULL );
            HBufC8* origUid( NULL );
            TInt pushCount( 0 );
                                                                
            TLiwVariant variant;
            variant.PushL();
            pushCount++;
                
            variant = outParam->Value();
            
            const CLiwMap* notifMap( variant.AsMap() );
                                            
            if ( notifMap->FindL( _L8("event"), variant ) )
                {
                event = variant.AsData().AllocLC();
                pushCount++;
                }    
                
            variant.Reset();    
                                             
            if( notifMap->FindL( _L8("appConfUid"), variant ) )
                {  
                appConfUid = variant.AsData().AllocLC();
                pushCount++;
                }
                
            variant.Reset();
                                           
            if( notifMap->FindL( _L8("name"), variant ) )
                {
                pluginName = variant.AsData().AllocLC();
                pushCount++;
                }
                   
            variant.Reset();
            
            if( notifMap->FindL( _L8("origUid"), variant ) )
                {
                origUid = variant.AsData().AllocLC();
                pushCount++;
                }
            
            variant.Reset();
                                            
            if( notifMap->FindL( _L8("pluginUid"), variant ) )
                {
                pluginUid = variant.AsData().AllocLC();
                pushCount++;
                }        
                
           variant.Reset();
                             
           if( notifMap->FindL( _L8("pluginIds"), variant ) )
               {                                                                        
               const CLiwList* pluginIdList( variant.AsList() );
                    
               variant.Reset();
                    
               TInt count( pluginIdList->Count() );
               
               retval = KErrNone;
               
               for( TInt i = 0; i < count && retval == KErrNone; i++ )
                   {
                   pluginIdList->AtL( i, variant );
                                      
                   HBufC8* pluginId( NULL );
                   
                   pluginId = variant.AsData().AllocLC();
                                                         
                   retval = iObserver->HandleNotifyL( 
                                ( event ) ? *event : KNullDesC8(), 
                                ( appConfUid ) ? *appConfUid : KNullDesC8(), 
                                ( pluginName ) ? *pluginName : KNullDesC8(),
                                ( origUid ) ? *origUid : KNullDesC8(),
                                ( pluginUid ) ? *pluginUid : KNullDesC8(), 
                                ( pluginId ) ? * pluginId : KNullDesC8() );
                                                      
                   CleanupStack::PopAndDestroy( pluginId );                                                      
                   
                   variant.Reset();
                   }
               }
        
            CleanupStack::PopAndDestroy( pushCount );
            }
        }
        
    if( retval != KErrNone && aEventId != KLiwEventInProgress)
        {
        CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
        
        inParamList.Reset();
        outParamList.Reset();
        //cancel old notification request
        iHspsInterface->ExecuteCmdL( KRequestNotification,
                                     inParamList,
                                     outParamList,
                                     KLiwOptCancel,
                                     this ); 
        
        inParamList.Reset();
        outParamList.Reset();
        //request notification again
        iTransactionId = -1;
        iHspsInterface->ExecuteCmdL( KRequestNotification,
                                     inParamList,
                                     outParamList,
                                     KLiwOptASyncronous,
                                     this );
        
        const TLiwGenericParam* outParam( NULL );
                        
        TInt pos( 0 );
        outParam = outParamList.FindFirst( pos, _L8("status") );
                       
        if ( outParam )
            {
            TInt retval;
            retval = outParam->Value().AsTInt32();
            
            if(retval == KErrNone )
                {
                pos = 0;
                outParam = outParamList.FindFirst( pos, _L8("TransactionID") );
                       
                if( outParam )
                    {
                    retval = outParam->Value().AsTInt32();
                    iTransactionId = retval;
                    }
                }     
            }
        
        }
   
    
    return retval;    
    }    
}

//End of file
