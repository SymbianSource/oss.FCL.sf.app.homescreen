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
*  Version     : %version:  2 % << Don't touch! Updated by Synergy at check-out.
*
*/



#ifndef C_HNXMLSUITEFILESREG_H
#define C_HNXMLSUITEFILESREG_H

#include <bautils.h>
#include <e32hashtab.h>

/**
 * Structure binding file information together.
 * All of this information is essential when synchronising suite files.
 */
NONSHARABLE_STRUCT(THnFileInfo)
	{
	TBuf< KMaxFileName > iFileName;
	TBuf< KMaxPath > iFilePath;
	TTime iLastModified;
	TInt iSize;
	};

/**
 * Registry of suite definition files.
 * 
 * Keeps track of the suite files' sync. 
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnXmlSuiteFilesReg) : public CBase
    {
public:
    /**
     * Standard factory method.
     *
     * @since S60 v5.0
     * @return Fully constructed object.
     */
    static TBool SynchronizeL( RFs & aFs, const TDesC &aSuiteName );
 
    /**
     * Standard virtual destructor.
     */
    virtual ~CHnXmlSuiteFilesReg();
   
private:
	
    /**
     * Standard C++ constructor.
     */     
	CHnXmlSuiteFilesReg( RFs & aFs );

    /**
     * Standard symbian 2nd phase constructor.
     */     
    void ConstructL( const TDesC &aSuiteName );
    
    /**
     * Destroys suite file maps.
     */
    void DestroySuiteFilesMaps();

    /**
     * Returns the valid suite import dir.
     *
     * @param aDriveNumber Number of the drive for which to create path.
     * @return Path.
     */	
    HBufC* GetSuiteImportDirL( TInt aDriveNumber ) const;

    /**
     * Searches all the drives for import files for this particular suite.
     */	
    void SearchDrivesForSuiteFilesL();

    /**
     * Processes suite import dir.
     *
     * @param aSuitePath Path of the suite import.
     */	
    void SearchDirForSuiteFilesL( const TDesC& aSuitePath );

    /**
     * Adds a file to the suite map. The logics in the methods always prefers
     * the newer files over the older. If a file by the same name already exists 
     * with a newer date file will not be added.
     *
     * @param aDir Directory of the new file.
     * @param aFile New file entry.
     */	
    void AddSuiteFileToMapL( const TDesC& aDir, const TEntry& aFile );

    /**
     * Calculates the current size of the dir in the installation.\
     *
     * @return Size in bytes.
     */
    TInt CalculateCurrentSuiteSizeL();

    /**
     * Calculates the size of the data to be synchronised.
     *
     * @return Size in bytes.
     */
    TInt CalculateSynchronizationSuiteSize();
	
    /**
     * Synchronizes the suite files.
     */
    TBool SynchronizeSuiteFilesL();

    /**
     * Constructs the valid installation path.
     *
     * @return The installation path.
     */
    HBufC* GetSuiteInstallPathL();
	
private:
	
   /**
    * Name of the suite (suite folder).
    */
    RBuf iSuiteName;
	
    /**
     * File server session reference
     */
    RFs &iFs;
    
    /**
     * File manager
     */
    CFileMan* iFileMan;  
        
    /**
     * Hashmap containing paths to the suite files.
     */
    RHashMap< const TDesC*, THnFileInfo* > iSuiteFiles;

    };
#endif // C_HNXMLSUITEFILESREG_H
