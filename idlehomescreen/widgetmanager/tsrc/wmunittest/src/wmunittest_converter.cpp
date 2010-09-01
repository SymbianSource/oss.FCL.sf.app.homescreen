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
    iImageConverter = CWmImageConverter::NewL();
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

    TInt expectedResult;
    User::LeaveIfError( aItem.GetNextInt( expectedResult ) ); 
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TInt err = iImageConverter->HandleIconString( 
            TSize (40, 40), inputStr, bitmap, mask );
    if ( err != KErrNone && expectedResult == -1 )
        {
        err = KErrNone;
        }

    _RETURN("ImageConverterConvertL End", ret );
    }

// End of File
