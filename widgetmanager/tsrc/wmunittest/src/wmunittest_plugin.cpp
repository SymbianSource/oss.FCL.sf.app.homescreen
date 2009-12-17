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
* Description: test blocks for CWmPlugin
*
*/

//INCLUDE FILES
#include <e32svr.h>
#include <stdlib.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <bautils.h>
#include <aknsskininstance.h>
#include "wmunittest.h"
// components to test
#include "wmplugin.h"
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>

// MACROS
#define _LOG(a) iLog->Log(_L(a))
#define _RETURN(a, b) iLog->Log(_L("\treturn: %d"),b); \
                     iLog->Log(_L(a)); \
                     return b; \

#define _CLEANUPCHECK_BEGIN \
    CBase* __check = new (ELeave) CBase(); \
    CleanupStack::PushL( __check );

#define _CLEANUPCHECK_END \
    CleanupStack::PopAndDestroy( __check );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CWmUnitTest::PluginCreateL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::PluginCreateL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmPlugin != 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    _CLEANUPCHECK_BEGIN
    iWmPlugin = CWmPlugin::NewL();
    iWmPlugin->SetContentController( this );
    _CLEANUPCHECK_END
    _RETURN("PluginCreateL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::PluginDeleteL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::PluginDeleteL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmPlugin == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    delete iWmPlugin;
    iWmPlugin = 0;
    _RETURN("PluginDeleteL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::PluginActivateL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::PluginActivateL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmPlugin == 0 ) User::Leave( KErrArgument );
    iWmPlugin->Activate();
    _RETURN("PluginActivateL End", KErrNone);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::PluginDeactivateL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::PluginDeactivateL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmPlugin == 0 ) User::Leave( KErrArgument );
    iWmPlugin->Deactivate();
    _RETURN("PluginActivateL End", KErrNone);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::PluginWidgetsChangedL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::PluginWidgetsChangedL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmPlugin == 0 ) User::Leave( KErrArgument );
    iWmPlugin->NotifyWidgetListChanged();
    _RETURN("PluginWidgetsChangedL End", KErrNone);
    }

// =============================================================================
// Methods from MHsContentController
// =============================================================================

TInt CWmUnitTest::WidgetListL( CHsContentInfoArray& aArray )
    {
    CreateContentInfoArrayL( aArray, iMass );
    return KErrNone;
    }

TInt CWmUnitTest::ViewListL( CHsContentInfoArray& /*aArray*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::AppListL( CHsContentInfoArray& /*aArray*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::AddWidgetL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::RemoveWidgetL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::AddViewL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::RemoveViewL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::ActivateViewL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }

TInt CWmUnitTest::ActivateAppL( CHsContentInfo& /*aInfo*/ )
    {
    return KErrNone;
    }




// -----------------------------------------------------------------------------
// CWmUnitTest::CreateContentInfoArrayL
// -----------------------------------------------------------------------------
//
void CWmUnitTest::CreateContentInfoArrayL( CHsContentInfoArray& aArray, TInt aCount )
    {
    for( TInt i=0; i<aCount; ++i )
        {
        CHsContentInfo* info = CHsContentInfo::NewLC();
        TBuf<64> name;
        name.Format( _L("Widget_%d"), i );
        info->SetNameL( name );
        TBuf8<20> id;
        id.Num( i );
        info->SetUidL( id );
        info->SetTypeL( _L8("template") );
        info->SetIconPathL( _L("uid(0x2000DAD2)") );
        info->SetCanBeAdded( ETrue );
        info->SetDescriptionL(_L("Default plaa plaa plaa plaa plaa plaa plaa"));
        
        aArray.Array().AppendL( info );
        CleanupStack::Pop( info );
        }
    }

// End of File
