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

// User include files
#include "hscontentcontrolfactory.h"

// Local constants

// TODO: Content controller ECom interface UID
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
EXPORT_C CHsContentControlFactory* CHsContentControlFactory::NewL()
    {
    CHsContentControlFactory* self = new ( ELeave ) CHsContentControlFactory();
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
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::CHsContentControlFactory()
// ----------------------------------------------------------------------------
//
CHsContentControlFactory::CHsContentControlFactory()
    {
    }

// ----------------------------------------------------------------------------
// CHsContentControlFactory::~CHsContentControlFactory()
// ----------------------------------------------------------------------------
//
EXPORT_C CHsContentControlFactory::~CHsContentControlFactory()
    {
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
                
                if ( information->OpaqueData().Compare( aControlType ) == 0 )
                    {
                    CHsContentControlUi* ccUi = CHsContentControlUi::NewL( 
                        information->ImplementationUid() );
                    
                    ccUi->SetContentControlTypeL( information->OpaqueData() );
                    
                    iHsContentControlUis.AppendL( ccUi );
                    
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
        
        if ( cc->ContentControlType().Compare( aControlType ) == 0 )                
            {
            return cc;
            } 
        }
    
    return NULL;
    }

// End of file
