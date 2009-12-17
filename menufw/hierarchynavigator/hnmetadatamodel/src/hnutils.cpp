/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <bautils.h>
//#include <mnaiwservices.h>
#include <liwservicehandler.h>
#include <e32hashtab.h>
#include <eikenv.h> 


#include "hnutils.h"
#include "hnliwutils.h"
#include "hnmdservicecommand.h"
#include "hnservicehandler.h"
#include "hnglobals.h"
#include "hnmdbasekey.h"
#include "menudebug.h"
#include "hnconvutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* HnUtils::ReadFileLC(const TDesC& aPath)
    {
    RFs& fs = CEikonEnv::Static()->FsSession();
    
    RFile file;
    User::LeaveIfError( file.Open(fs, aPath, EFileRead) );
    CleanupClosePushL( file );

    TInt fileSize(0);
    User::LeaveIfError( file.Size( fileSize ) );
    HBufC8* buf = HBufC8::NewLC( fileSize );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );

    // clean up
    CleanupStack::Pop(buf);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PushL(buf);

    return buf;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* HnUtils::ReadFileL( const TDesC& aPath )
    {
    HBufC8* buf = ReadFileLC( aPath );
    CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C HBufC* HnUtils::LocateFileLC( const TDesC& aFile )
    {
    HBufC* filePath = HBufC::NewLC(KMaxName);
    RFs& fs = CEikonEnv::Static()->FsSession();

    TDriveList driveList;
    TChar driveLetter;
    TInt driveNumber=EDriveY;
    User::LeaveIfError(fs.DriveList(driveList));
    for(; driveNumber>=EDriveA-1; driveNumber-- )
        {
        if (driveNumber==EDriveA-1)
        driveNumber = EDriveZ;
        if (driveList[driveNumber])
            {
            User::LeaveIfError(fs.DriveToChar(driveNumber, driveLetter));
            filePath->Des().Zero();
            filePath->Des().Append(driveLetter);
            filePath->Des().Append(KRscPath);
            filePath->Des().Append(aFile);

            if (BaflUtils::FileExists(fs,*filePath))
                {
                return filePath;
                }
            }
        if (driveNumber == EDriveZ)
        break;
        }

    User::Leave(KErrNotFound);
    return filePath;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
EXPORT_C HBufC* HnUtils::LocateNearestLanguageFileLC( const TDesC& aFile )
    {
    _LIT( KExtRSC, ".r");
    if (aFile.Find( KExtRSC ) == (aFile.Length() - 4) )
        {
        HBufC* filePath = HBufC::NewLC(KMaxName);
        RFs& fs = CEikonEnv::Static()->FsSession();
        TDriveList driveList;
        TChar driveLetter;
        TInt driveNumber=EDriveY;
        User::LeaveIfError(fs.DriveList(driveList));
        TFileName file;
        for(; driveNumber>=EDriveA-1; driveNumber-- )
            {
            if (driveNumber==EDriveA-1)
            driveNumber = EDriveZ;
            if (driveList[driveNumber])
                {
                User::LeaveIfError(fs.DriveToChar(driveNumber, driveLetter));
                filePath->Des().Zero();
                filePath->Des().Append(driveLetter);
                filePath->Des().Append(KRscPath);
                filePath->Des().Append(aFile);
    
                file = *filePath;
                BaflUtils::NearestLanguageFile( fs, file);
                if( BaflUtils::FileExists( fs, file))
                    {
                    CleanupStack::PopAndDestroy(filePath);    
                    return file.AllocLC();
                    }
                }
            if (driveNumber == EDriveZ)
            break;
            }
        CleanupStack::PopAndDestroy(filePath);
        }
    return KNullDesC().AllocLC();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void HnUtils::SetAttributeL( const TXmlEngElement& aElement,
        RBuf8& aAttribute )
    {
    TPtrC8 v = aElement.Value();

    if ( v.Length()> 0 )
        {
        HBufC8* trimmed = v.AllocLC();
        trimmed->Des().Trim();
        aAttribute.Close();
        aAttribute.CreateL( trimmed->Length() );
        aAttribute.Copy( *trimmed );
        CleanupStack::PopAndDestroy( trimmed );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void HnUtils::SetAttributeL( const TXmlEngElement& aElement,
        const TDesC8& aAttributeName, RBuf8& aValue8 )
    {
    if ( aElement.HasAttributeL( aAttributeName ) )
        {
        TPtrC8 value8 =
        aElement.AttributeNodeL( aAttributeName ).Value();

        aValue8.Close();
        aValue8.CreateL( value8.Length() );
        aValue8.Copy( value8 );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8 * HnUtils::GetColumnTextLC( TInt aColumn,
        const TDesC8 & aSourceText, TChar aColumnSeparator )
    {
    HBufC8 * ret = NULL;
    TLex8 processedText ( aSourceText );

    TInt column( 0 );
    processedText.Mark();
    
    while ( !processedText.Eos() )
        {
        if ( processedText.Get() == aColumnSeparator )
            {
            if ( column == aColumn )
                {
                processedText.UnGet();
                break;
                }
            processedText.Mark();
            column++;
            }
        }

    if ( aColumn <= column )
        {
        ret = processedText.MarkedToken().AllocLC();
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt HnUtils::GetColumnTextColumnCount(
        const TDesC8 & aSourceText, TChar aColumnSeparator )
    {
    TInt ret(1);
    TLex8 processedText(aSourceText);
    while ( !processedText.Eos() )
        {
        if (processedText.Get() == aColumnSeparator)
            {
            ret++;
            }
        }
    return ret;
    }

