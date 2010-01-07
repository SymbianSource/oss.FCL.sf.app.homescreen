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
* Description: test blocks for CWmImageConverter
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
#include "wmimageconverter.h"


// -----------------------------------------------------------------------------
// CWmUnitTest::ImageConverterCreateL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ImageConverterCreateL( CStifItemParser& /*aItem*/ )
    {
    if ( iImageConverter != 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    _CLEANUPCHECK_BEGIN
    iImageConverter = CWmImageConverter::NewL( this );
    _CLEANUPCHECK_END
    _RETURN("ImageConverterCreateL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ImageConverterDeleteL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ImageConverterDeleteL( CStifItemParser& /*aItem*/ )
    {
    if ( iImageConverter == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    delete iImageConverter;
    iImageConverter = 0;
    _RETURN("ImageConverterDeleteL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ImageConverterConvertL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ImageConverterConvertL( CStifItemParser& aItem )
    {
    if ( iImageConverter == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    iConversionReady = EFalse;
    TPtrC inputStr;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    User::LeaveIfError( aItem.GetNextString( inputStr ) );
    if ( inputStr.Length() == 0 ) User::Leave( KErrArgument );

    TRAPD( err, iImageConverter->HandleIconStringL( 40, 40, inputStr ) );
    if ( err != KErrNone )
        {
        _LOG("ImageConverterConvertL:HandleIconStringL returns with leave");
        iConversionReady = ETrue;
        iConversionError = err;
        }

    _RETURN("ImageConverterConvertL End", ret );
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ImageConverterWaitResultL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ImageConverterWaitResultL( CStifItemParser& aItem )
    {
    if ( iImageConverter == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    TInt expectedResult;

    User::LeaveIfError( aItem.GetNextInt( expectedResult ) );    

    iWaiting = EFalse;
    if ( !iConversionReady )
        {
        iWaiting = ETrue;
        CActiveScheduler::Start();
        iWaiting = EFalse;
        }

    // check conversion error
    if ( expectedResult == KErrNone && iConversionError != KErrNone )
        {
        _LOG("ImageConverterWaitResultL: expected ok, returns error");
        ret = iConversionError;
        }
    else if ( expectedResult != KErrNone && iConversionError == KErrNone )
        {
        _LOG("ImageConverterWaitResultL: expected error, returns ok");
        ret = KErrGeneral;
        }

    _RETURN("ImageConverterWaitResultL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::NotifyCompletion
// -----------------------------------------------------------------------------
//
void CWmUnitTest::NotifyCompletion( TInt aError )
    {
    if ( !iConversionReady )
        {
        iConversionReady = ETrue;
        iConversionError = aError;
        if ( iWaiting )
            {
            CActiveScheduler::Stop();
            }
        }
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::ImageConverterConversionMethodL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::ImageConverterConversionMethodL( CStifItemParser& aItem )
    {
    if ( iImageConverter == 0 ) User::Leave( KErrArgument );
    TInt ret = KErrNone;
    TInt conversionMethod;

    User::LeaveIfError( aItem.GetNextInt( conversionMethod ) );    

    // check conversion method
    if ( conversionMethod != iImageConverter->ConversionMethod() )
        ret = KErrGeneral;

    _RETURN("ImageConverterConversionMethodL End", ret);
    }

// -----------------------------------------------------------------------------
// CWmUnitTest::CancelConvertL
// -----------------------------------------------------------------------------
//
TInt CWmUnitTest::CancelConvertL( CStifItemParser& /*aItem*/ )
    {
    if ( iImageConverter == 0 ) User::Leave( KErrArgument );
    iImageConverter->Cancel();
    _RETURN("CancelConvertL End", KErrNone);
    }
// End of File
