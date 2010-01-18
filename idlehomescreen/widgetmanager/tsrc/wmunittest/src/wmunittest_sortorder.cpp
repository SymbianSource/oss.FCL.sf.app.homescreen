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
* Description:  test blocks for CWmPersistentWidgetorder
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
#include "wmpersistentwidgetorder.h"
#include "wmwidgetdata.h"
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>
#include <widgetregistryclient.h> // widgetreqistry

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderCreateL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderCreateL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmWidgetOrder != 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    _CLEANUPCHECK_BEGIN
    iWmWidgetOrder = CWmPersistentWidgetOrder::NewL( iFs );
    _CLEANUPCHECK_END
    _RETURN("WidgetOrderCreateL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderDeleteL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderDeleteL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmWidgetOrder == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    delete iWmWidgetOrder;
    iWmWidgetOrder = 0;
    _RETURN("WidgetOrderDeleteL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderLoadL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderLoadL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmWidgetOrder == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    _CLEANUPCHECK_BEGIN
    iWmWidgetOrder->LoadL();
    _CLEANUPCHECK_END
    _RETURN("WidgetOrderLoadL End", ret);
    }


// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderSaveL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderSaveL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmWidgetOrder == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;

    _CLEANUPCHECK_BEGIN
    RWidgetDataValues array;
    CleanupClosePushL( array );
    CreateWidgetDataArrayL( array, iMass );
    iWmWidgetOrder->StoreL( array );
    array.ResetAndDestroy();
    CleanupStack::Pop( &array );
    _CLEANUPCHECK_END

    _RETURN("WidgetOrderSaveL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderCheckIsEmptyL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderCheckIsEmptyL( CStifItemParser& aItem )
    {
    if ( iWmWidgetOrder == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    TInt expectedResult;
    aItem.GetNextInt( expectedResult );

    if ( iWmWidgetOrder->IsEmpty() && !expectedResult )
        ret = KErrGeneral;
    if ( !iWmWidgetOrder->IsEmpty() && expectedResult )
        ret = KErrGeneral;

    _RETURN("WidgetOrderCheckIsEmptyL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::WidgetOrderUseL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::WidgetOrderUseL( CStifItemParser& /*aItem*/ )
    {
    if ( iWmWidgetOrder == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;

    _CLEANUPCHECK_BEGIN
    // test IndexOf
    RWidgetDataValues array;
    CreateWidgetDataArrayL( array, iMass );
    for( TInt i=0; i<array.Count() && ret==KErrNone; ++i )
        {
        TInt index = iWmWidgetOrder->IndexOf( *array[i] );
        if ( index < KErrNotFound || index >= array.Count() )
            ret = KErrUnknown; // illegal index
        }
    array.ResetAndDestroy();
    _CLEANUPCHECK_END

    _RETURN("WidgetOrderUseL End", ret);
    }


// -----------------------------------------------------------------------------
// CWmUnitTest::CreateWidgetDataArrayL
// -----------------------------------------------------------------------------
//
void CWmUnitTest::CreateWidgetDataArrayL( RWidgetDataValues& aArray, TInt aCount )
    {
    // first create an array of content info classes
    CHsContentInfoArray* contentArray = CHsContentInfoArray::NewL();
    CleanupStack::PushL( contentArray );
    CreateContentInfoArrayL( *contentArray, aCount );
    // connect to widget registry
    RWidgetRegistryClientSession registryClient;
    User::LeaveIfError( registryClient.Connect() );
    CleanupClosePushL( registryClient );
    TSize dummySize(40,40);
    CWmResourceLoader* resLoader = NULL; // not used in this case
    while( contentArray->Array().Count() > 0 )
        {        
        CWmWidgetData* data = CWmWidgetData::NewL(
                dummySize, *resLoader, 
                contentArray->Array()[0],
                &registryClient);

        contentArray->Array().Remove( 0 );
        CleanupStack::PushL( data );
        aArray.AppendL( data );
        CleanupStack::Pop( data );
        }
 
    registryClient.Disconnect();
    CleanupStack::PopAndDestroy( &registryClient );
    CleanupStack::PopAndDestroy( contentArray );
    }


// End of File
