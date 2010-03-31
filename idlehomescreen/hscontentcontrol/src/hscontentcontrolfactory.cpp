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

// System include files
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <aknview.h>

// User include files
#include "xnappuiadapter.h"
#include "hscontentcontrolfactory.h"
#include "hscontentcontrolecomlistener.h"
#include "hscontentcontrolswilistener.h"

// Local constants

// Content controller ECom interface UID
const TUid KInterfaceUidContentController = { 0x20026F51 };

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CHsContentControlFactory::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CHsContentControlFactory* CHsContentControlFactory::NewL( 
        CXnAppUiAdapter& aAdapter )
    {
    CHsContentControlFactory* self = 
            new ( ELeave ) CHsContentControlFactory( aAdapter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::ConstructL()
// ----------------------------------------------------------------------------
//
void CHsContentControlFactory::ConstructL()
    {
    iHsContentControlEComListener = 
            CHsContentControlEComListener::NewL( *this );
    iHsContentControlSwiListener = 
            CHsContentControlSwiListener::NewL( *this );

    REComSession::ListImplementationsL( 
        KInterfaceUidContentController, iImplArray );
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::CHsContentControlFactory()
// ----------------------------------------------------------------------------
//
CHsContentControlFactory::CHsContentControlFactory( CXnAppUiAdapter& aAdapter )
    :iAdapter( aAdapter ) 
    {
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::~CHsContentControlFactory()
// ----------------------------------------------------------------------------
//
EXPORT_C CHsContentControlFactory::~CHsContentControlFactory()
    {    
	delete iHsContentControlEComListener;
    delete iHsContentControlSwiListener;
    
    iImplArray.ResetAndDestroy();
    iImplArray.Close();

    iHsContentControlUis.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------------
// CHsContentControlFactory::GetHsContentController()
// ---------------------------------------------------------------------------------
//
EXPORT_C MHsContentControlUi* CHsContentControlFactory::GetHsContentController(
    const TDesC8& aControlType )
    {
    MHsContentControlUi* retval( FindHsContentController( aControlType ) ); 
    
    if ( !retval )
        {
        TRAP_IGNORE(
            // Get implementations via ECom
            RImplInfoPtrArray plugins;
            CleanupResetAndDestroyPushL( plugins );
                    
            REComSession::ListImplementationsL( 
                KInterfaceUidContentController, plugins );
            
            for ( TInt i = 0; i < plugins.Count(); i++ )
                {
                CImplementationInformation* information( plugins[i] );
                
                if ( information->OpaqueData().CompareF( aControlType ) == 0 )
                    {
                    CHsContentControlUi* ccUi = CHsContentControlUi::NewL( 
                        information->ImplementationUid() );
                    CleanupStack::PushL( ccUi );
                    
                    ccUi->SetContentControlTypeL( information->OpaqueData() );
                    iHsContentControlUis.AppendL( ccUi );
                    
                    CleanupStack::Pop(); //ccUi
                    retval = ccUi;
                    // All done
                    break;
                    }
                }
            
            CleanupStack::PopAndDestroy(); // plugins
            ); // TRAP_IGNORE
        }
    
    return retval;
    }

// ---------------------------------------------------------------------------------
// CHsContentControlFactory::PrepareToExit()
// ---------------------------------------------------------------------------------
//
EXPORT_C void CHsContentControlFactory::PrepareToExit()
    {
    if ( iHsContentControlUis.Count() > 0 )
        {
        for( TInt index( iHsContentControlUis.Count() - 1 ); index >= 0; --index )
            {
            CHsContentControlUi* cc( iHsContentControlUis[ index ] );
            ReleaseHsCcUi( cc );
            }
        }
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::FindHsContentController()
// ----------------------------------------------------------------------------
//
MHsContentControlUi* CHsContentControlFactory::FindHsContentController(
    const TDesC8& aControlType )
    {       
    for ( TInt i = 0; i < iHsContentControlUis.Count(); i++ )
        {
        CHsContentControlUi* cc( iHsContentControlUis[ i ] );
        
        if ( cc->ContentControlType().CompareF( aControlType ) == 0 )
            {
            return cc;
            } 
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::HandleEComChangeEvent()
// ----------------------------------------------------------------------------
//
void CHsContentControlFactory::HandleEComChangeEvent()
    {
    // ignore event if no plugin loaded.
    if ( iHsContentControlUis.Count() > 0 )
        {
        TRAP_IGNORE( CheckPluginChangesL(); );
        }
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::HandleSwiEvent()
// ----------------------------------------------------------------------------
//
void CHsContentControlFactory::HandleSwiEvent( const RArray<TUid>& aUidList )
    {
    // ignore event if no plugin loaded.
    if ( iHsContentControlUis.Count() > 0 && aUidList.Count() > 0 )
        {
        for( TInt index( iHsContentControlUis.Count() - 1 ); index >= 0; --index )
            {
            CHsContentControlUi* cc( iHsContentControlUis[ index ] );
            for( TInt uidIndex( aUidList.Count() - 1 ); uidIndex >= 0 && cc; --uidIndex )
                {
                // ImplUid of plugin must match Sis pkg uid            
                if ( cc->ImplUid() == aUidList[uidIndex] )
                    {
                    // release ui and keep checking, multiple package might be being
                    // processed by SWI, this events comes once for all packages. 
                    ReleaseHsCcUi( cc );
                    iHsContentControlUis.Remove( index );
                    delete cc;
                    cc = NULL;                    
                    }                
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::CheckPluginChangesL
// ----------------------------------------------------------------------------
//
void CHsContentControlFactory::CheckPluginChangesL()
    {
    // Array to return all implementations in an interface
    RImplInfoPtrArray plugInArray;
    CleanupResetAndDestroyPushL( plugInArray );

    // Get the list of all implementations.
    REComSession::ListImplementationsL( 
            KInterfaceUidContentController, plugInArray );
        
    TUid uid( KNullUid );
    TBool done( EFalse );
    
    // If an implementation is not present in present in the plugInArray then its removed. 
    for( TInt index( iImplArray.Count() - 1 ); index >= 0 && !done; --index )
        {
        uid = iImplArray[ index ]->ImplementationUid();
        CImplementationInformation* implInfo = 
                FindPluginImplInfo( uid, plugInArray );
        if ( implInfo && PluginUpgradeDowngrade( *implInfo ) )
            {
            done = ETrue;
            }
        else
            {
            // check if ContenControlUi is loaded, unload it
            for( TInt innerIndex( iHsContentControlUis.Count() - 1 );
                    innerIndex >= 0 && !done; --innerIndex )
                {
                CHsContentControlUi* cc( iHsContentControlUis[ innerIndex ] );
                if ( cc && cc->ImplUid() == uid )
                    {
                    ReleaseHsCcUi( cc );
                    iHsContentControlUis.Remove( innerIndex );
                    delete cc;
                    cc = NULL;
                    done = ETrue;
                    }
                }
            done = ETrue;
            }
        }

    // Cleanup.
    CleanupStack::PopAndDestroy(); // plugInArray
    
    // Reset the array and refresh the plugin list.
    iImplArray.ResetAndDestroy();
    iImplArray.Close();    
    REComSession::ListImplementationsL( 
            KInterfaceUidContentController, iImplArray );
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::FindPluginImplInfo
// ----------------------------------------------------------------------------
//
CImplementationInformation* CHsContentControlFactory::FindPluginImplInfo( 
        const TUid& aUid, const RImplInfoPtrArray& aPlugInArray )
    {
    CImplementationInformation* implInfo( NULL );
    for( TInt index( aPlugInArray.Count() - 1 ); index >= 0; --index )
        {
        implInfo = aPlugInArray[ index ];
        if( aUid == implInfo->ImplementationUid() )
            {
            break;
            }
        }
    return implInfo;
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::PluginUpgradeDowngrade
// ----------------------------------------------------------------------------
//
TBool CHsContentControlFactory::PluginUpgradeDowngrade( 
        const CImplementationInformation& aPluginImplInfo )
    {
    // Check for each plugin in the array if the version matches with the plugin we have
    TUid uid = aPluginImplInfo.ImplementationUid();
    for( TInt outterIndex( iImplArray.Count() - 1 ); outterIndex >= 0; --outterIndex )
        {
        if( uid == iImplArray[ outterIndex ]->ImplementationUid() )
            {
            if( aPluginImplInfo.Version() != iImplArray[ outterIndex ]->Version() ||
               aPluginImplInfo.Drive() != iImplArray[ outterIndex ]->Drive() ||
               aPluginImplInfo.DisplayName() != iImplArray[ outterIndex ]->DisplayName() ||
               aPluginImplInfo.OpaqueData() != iImplArray[ outterIndex ]->OpaqueData() )
                {
                // If control reaches here, it means we either have an upgrade or downgrade.
                // check if we have loaded this plugin, reload it if found in array.
                for( TInt innerIndex( iHsContentControlUis.Count() - 1 ); 
                        innerIndex >= 0; --innerIndex )
                    {
                    CHsContentControlUi* cc( iHsContentControlUis[ innerIndex ] );
                    if ( cc && cc->ImplUid() == uid )
                        {
                        ReleaseHsCcUi( cc );
                        iHsContentControlUis.Remove( innerIndex );
                        delete cc;
                        cc = NULL;
                        innerIndex = KErrNotFound;
                        }
                    }                    
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::ReleaseHsCcUi
// ----------------------------------------------------------------------------
//
void CHsContentControlFactory::ReleaseHsCcUi( 
        CHsContentControlUi* aHsContentControlUi )
    {
    if ( &iAdapter && aHsContentControlUi )
        {
        RPointerArray<CAknView> views;

        // notify plugin about deactivation
        aHsContentControlUi->DeActivate();
        
        // get all views from HsContentControlUi
        aHsContentControlUi->Views( views );

        for ( TInt i=0; i<views.Count(); i++ )
            {
            CAknView* view = views[ i ];
            views.Remove( i );
            // remove/deregister/delete all views from appui
            TRAP_IGNORE( iAdapter.RemoveViewL( *view ) );
            }

        // reset views array
        views.Reset();
        }
    }

// End of file
