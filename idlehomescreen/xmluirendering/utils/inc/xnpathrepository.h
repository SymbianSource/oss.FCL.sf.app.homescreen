/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Locates files from a set of defined paths
*
*/



#ifndef CXNPATHREPOSITORY_H
#define CXNPATHREPOSITORY_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class RFs;
// CLASS DECLARATION

/**
*  Locates files from a set of defined paths
*  @lib ?library
*  @since Series 60 3.1
*/
class CXnPathRepository : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aRFs File server session
        */
        IMPORT_C static CXnPathRepository* NewL(RFs& aRFs);
        
        /**
        * Destructor.
        */
        virtual ~CXnPathRepository();

    public: // New functions
        /**
        * Add a search path
        * @since Series 60 3.1
        * @param aPath Path to add
        * @exception KXnErrAddPathFailed Adding path to repository failed.
        */
        IMPORT_C void AddPathL(const TDesC& aPath);
        /**
        * Locate a file and build a full path to the file. The path search order is last-in-first-out, the last added path is searched first.
        * @since Series 60 3.1
        * @param aFileName Name of the file to locate
        * @return Full path of the file. Caller must delete the allocated pointer.
        */
        IMPORT_C HBufC* FindFileL(const TDesC& aFileName);
        /**
        * Get paths stored in the repository. Ownership is not transferred. The paths are ordered first-in-first-out, the last added path is last in the array.
        * @since Series 60 3.1
        * @return Paths in the repository
        */
        IMPORT_C RPointerArray<HBufC>& Paths();

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

        
    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CXnPathRepository();

        /**
        * 2nd phase constructor.
        * @param aRFs File server session
        */
        void ConstructL(RFs& aRFs);

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // search paths to use
        RPointerArray<HBufC> iPaths;
        // File server session
        RFs* iRFs;
    };

#endif      // CXnPATHREPOSITORY_H   
            
// End of File
