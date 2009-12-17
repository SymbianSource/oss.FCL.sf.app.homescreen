/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* CWmIconFileProvider declaration
*
*/

#ifndef WMICONFILEPROVIDER_H_
#define WMICONFILEPROVIDER_H_

// INCLUDES
#include <f32file.h>
#include <AknIconUtils.h>

// CLASS DECLARATIONS

/**
* CWmIconFileProvider
*/
NONSHARABLE_CLASS( CWmIconFileProvider ) : public CBase,
                            public MAknIconFileProvider 
    {	
public:	
	/**
    * Two-phased constructor.
    *
    * @param aFs A handle to a file server session.
    * @return New instance of CWmIconFileProvider.
    */
	static CWmIconFileProvider* NewL( RFs& aFs );	
	
    /**
    * Destructor.
    */
	~CWmIconFileProvider();

    /**
    * @return icon file name.
    */
    inline const TDesC& IconFileName();

    /**
    * Setter for icon file name.
    */
    void SetIconFile( const TDesC& aFileName );
        
    
private:
	//from MAknIconFileProvider
	
    /**
    * Returns an open file handle to the icon file.
    * This method should leave if an icon file with specified type does
    * not exist. That may be the case e.g. with MBM file,
    * if there are no bitmap icons.
    *
    * Note! RFs::ShareProtected must be called to the RFs instance used
    * for opening the file.
    *
    * @param aFile Icon file should be opened in this file handle, which
    * is an empty file handle, when the AknIcon framework calls this method.
    * The AknIcon framework takes care of closing the file handle after
    * having used it.
    * @param aType Icon file type.
    */
	void RetrieveIconFileHandleL( RFile& aFile, const TIconFileType aType );
    
    /**
    * With this method, AknIcon framework informs that it does not use
    * this MAknIconFileProvider instance any more. After this call,
    * it is ok to delete the object. This can be implemented simply
    * e.g. by deleting self in this callback.
    * Normally, this callback is invoked when the icon in question
    * is deleted.
    * Note, however, that if the same MAknIconFileProvider instance is
    * supplied in multiple CreateIcon calls, then it must be accessible
    * by AknIcon framework until it has signalled a matching amount
    * of these callbacks.
    */
    void Finished();
    
private:
    /**
    * Constructor
    *
    * @param aFs A handle to a file server session.
    */
    CWmIconFileProvider( RFs& aFs );
    
    /** 2nd phase constructor */
    void ConstructL();
    
private: // data
    
    /** the file session */
    RFs& iFs;
    
    /** A buffer that can contain the name of a file. */
    TFileName iFileName;
    
    /** used for file session */
    RFile iFile;
    };

#include "wmiconfileprovider.inl"
#endif // WMICONFILEPROVIDER_H_
