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
*  Version     : %version:  9 % << Don't touch! Updated by Synergy at check-out.
*
*/

#include <sysutil.h> 
#include "hnxmlsuitefilesreg.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "menudebug.h"
#include "hnhelper.inl"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnXmlSuiteFilesReg::SynchronizeL( RFs & aFs, const TDesC &aSuiteName )
	{
	CHnXmlSuiteFilesReg* self = new (ELeave) CHnXmlSuiteFilesReg( aFs );
	CleanupStack::PushL(self);
	self->ConstructL( aSuiteName );
	TBool ret = self->SynchronizeSuiteFilesL();
	CleanupStack::PopAndDestroy( self );
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnXmlSuiteFilesReg::~CHnXmlSuiteFilesReg()
	{
	iSuiteName.Close();
	DestroySuiteFilesMaps();
	delete iFileMan;
	}
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnXmlSuiteFilesReg::CHnXmlSuiteFilesReg( RFs & aFs ) : iFs(aFs),
	iSuiteFiles(&TDesC16Hash, &TDesC16Ident)
	{

	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
HBufC* CHnXmlSuiteFilesReg::GetSuiteInstallPathL()
	{
	TBuf<KMaxPath> installPath;	
	User::LeaveIfError( iFs.PrivatePath(installPath) );
	TChar drive;
	TBuf<1> driveLetter;
	iFs.DriveToChar( EDriveC, drive );
	installPath.Insert(0, KColon );
	driveLetter.Append( drive );
	installPath.Insert(0, driveLetter );
	installPath.Append(KEntriesSuffix);
	installPath.Append( iSuiteName );
	installPath.Append( Kbackslash );
	return installPath.AllocL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlSuiteFilesReg::ConstructL( const TDesC &aSuiteName )
	{
	iSuiteName.CreateL( aSuiteName );
	iFileMan = CFileMan::NewL(iFs);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlSuiteFilesReg::DestroySuiteFilesMaps()
	{
	DestroyMapValue( iSuiteFiles );
	iSuiteFiles.Close();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnXmlSuiteFilesReg::GetSuiteImportDirL( TInt aDriveNumber ) const
	{
	TBuf<KMaxPath> outputDir; 
			
	TChar driveLetter;
    User::LeaveIfError( iFs.DriveToChar( aDriveNumber, driveLetter ) );  
    
    TBuf< KSingleChar > driveLetterConst;
    driveLetterConst.Append( driveLetter );
      
    User::LeaveIfError( iFs.PrivatePath( outputDir ) );

    outputDir.Insert( 0, driveLetterConst );
    outputDir.Insert( 1, KColon );
    outputDir.Append( KSuitesDir );
    outputDir.Append( iSuiteName );
    outputDir.Append( Kbackslash );
    
    return outputDir.AllocL();
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlSuiteFilesReg::SearchDrivesForSuiteFilesL()
    {
    DestroySuiteFilesMaps();
    
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    TFixedArray< TInt, KMaxDrives > driveSearchOrder( KDriveSearchOrder, KMaxDrives );
    
    for(TInt iterator(0); iterator < driveSearchOrder.Count(); iterator++ )
    	{ 
        if ( driveList[ driveSearchOrder[ iterator ] ] )
            {
            RBuf filePath( GetSuiteImportDirL( driveSearchOrder[ iterator ] ) );
            CleanupClosePushL( filePath );
            SearchDirForSuiteFilesL(filePath);
            CleanupStack::PopAndDestroy( &filePath );
            }
    	} 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlSuiteFilesReg::SearchDirForSuiteFilesL( const TDesC& aSuitePath )
    {
    CDir* fileList = NULL;
    if (!iFs.GetDir( aSuitePath, KEntryAttMaskSupported, ESortByName, fileList ))
	    {
	    CleanupStack::PushL( fileList );
        for( TInt i( 0 ); i < fileList->Count(); i++ ) 
            {  
            if (! (*fileList)[i].IsDir() )
        	    {
        	    AddSuiteFileToMapL( aSuitePath, (*fileList)[i] );
        	    }
            }
        CleanupStack::PopAndDestroy( fileList );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlSuiteFilesReg::AddSuiteFileToMapL( const TDesC& aDir, const TEntry& aFile )
	{
    THnFileInfo fileInfo = { aFile.iName, aDir, aFile.iModified, aFile.iSize };
	fileInfo.iFilePath.Append( aFile.iName );
		
	THnFileInfo** existingFileInfo = iSuiteFiles.Find( &aFile.iName );
	
    TInt newFileDrive( KErrNotFound );
    iFs.CharToDrive( fileInfo.iFilePath[0], newFileDrive );
    
    TBool newerThanExisting( EFalse );
	TInt existingFileDrive( KErrNotFound );
	if ( existingFileInfo )
		{
		iFs.CharToDrive( (*existingFileInfo)->iFilePath[0], existingFileDrive);
        newerThanExisting =
                (*existingFileInfo)->iLastModified < fileInfo.iLastModified &&
                newFileDrive != EDriveZ;
		}
	
    if ( !existingFileInfo || newerThanExisting || existingFileDrive == EDriveZ )
        {
        if( existingFileInfo )
            {
            iSuiteFiles.Remove( &aFile.iName );
            }
        THnFileInfo* fileInfoPointer = new (ELeave) THnFileInfo( fileInfo );
        iSuiteFiles.InsertL( &fileInfo.iFileName, fileInfoPointer );
        }
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnXmlSuiteFilesReg::CalculateCurrentSuiteSizeL()
	{
	RBuf suiteInstallDir( GetSuiteInstallPathL() );
	CleanupClosePushL( suiteInstallDir );
	CDir* suiteFileList;                  
	iFs.GetDir( suiteInstallDir, KEntryAttMaskSupported, ESortByName, suiteFileList);
	CleanupStack::PushL(suiteFileList);

	TInt suiteSize(0); 
	for( TInt i(0); suiteFileList && i< suiteFileList->Count(); i++ )  
		{
		suiteSize+= (*suiteFileList)[i].iSize;
		}
	CleanupStack::PopAndDestroy( suiteFileList );
	CleanupStack::PopAndDestroy( &suiteInstallDir );
	return suiteSize;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnXmlSuiteFilesReg::CalculateSynchronizationSuiteSize()
	{
	TInt retSize( 0 );
	
	THashMapIter< const TDesC*, THnFileInfo* > iter( iSuiteFiles );
	
	while ( iter.NextKey() )
		{
		const THnFileInfo& fileInfo =  **(iter.CurrentValue());
		retSize += fileInfo.iSize;
		}
	
	return retSize;
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnXmlSuiteFilesReg::SynchronizeSuiteFilesL()
	{
	TBool ret( EFalse );
	SearchDrivesForSuiteFilesL();

	TInt spaceNeeded = CalculateSynchronizationSuiteSize() -
		CalculateCurrentSuiteSizeL();
	
	RBuf suiteInstallationDir( GetSuiteInstallPathL() );
	CleanupClosePushL( suiteInstallationDir );
	iFileMan->RmDir( suiteInstallationDir );
	if( iSuiteFiles.Count() && ( spaceNeeded < 0 || !SysUtil::DiskSpaceBelowCriticalLevelL(
	        &iFs, spaceNeeded , EDriveC ) ) )
        {
        THashMapIter< const TDesC*, THnFileInfo* > iter( iSuiteFiles );
        iFs.MkDirAll( suiteInstallationDir );
        while( iter.NextKey() )
            {
            const THnFileInfo& fileInfo =  **(iter.CurrentValue());
            TBuf<KMaxPath> filePath( suiteInstallationDir );
            filePath.Append( fileInfo.iFileName );
            if( filePath.Compare( fileInfo.iFilePath ) )
                {
                if( BaflUtils::FileExists( iFs, filePath ) )
                    {
                    iFs.SetAtt( filePath, NULL, KEntryAttReadOnly );
                    iFs.Delete( filePath );
                    }
                iFileMan->Copy(
                        fileInfo.iFilePath,
                        filePath, CFileMan::EOverWrite );
                ret = ETrue;
                }
            }
        }
	CleanupStack::PopAndDestroy( &suiteInstallationDir );
	return ret;
	}
