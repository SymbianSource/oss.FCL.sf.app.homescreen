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
* Description:  Loads polymorphic dlls
*
*/



#ifndef CXNLIBRARYLOADER_H
#define CXNLIBRARYLOADER_H

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
class CXnPathRepository;
class RFs;
class CXnArray;
class RXnClientRequest;

// CLASS DECLARATION

/**
*  Locates files from a set of defined paths
*  @lib ?library
*  @since Series 60 3.1
*/
class CXnLibraryLoader : public CBase
    {
    public:  // Constructors and destructor
        struct CLibraryItem;  
              
        /**
        * Two-phased constructor.
        * @param aRFs File server session
        */
        IMPORT_C static CXnLibraryLoader* NewL();
        
        /**
        * DEPRECATED
        * Two-phased constructor.
        * @param aPathRepository Path repository for handling multiple directories
        * @param aRFs File server session
        */
        IMPORT_C static CXnLibraryLoader* NewL(CXnPathRepository& aPathRepository, RFs& aRFs);
        
        /**
        * Destructor.
        */
        virtual ~CXnLibraryLoader();

    public: // New functions
        /**
        * Load libraries by wildcard
        * @since Series 60 3.1
        * @param aWildCard Wildcard to search
        */
        IMPORT_C void LoadL(const TDesC& aWildCard);
        
        /**
        * Load libraries given by Definition Engine
        * @since Series 60 3.1
        * @param aXnClient Client interface to Definition Engine
        * @param aAppUid Uid of the current application
        */
        IMPORT_C void LoadL(RXnClientRequest& aXnClient, const TUid& aAppUid);
        /**
        * Get pointers to library functions
        * @since Series 60 3.1
        */
        IMPORT_C void Functions(CXnArray& aFunctions) const;
        
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
        CXnLibraryLoader();

        /**
        * 2nd phase constructor.
        * @param aRFs File server session
        */
        void ConstructL();

        /**
        * DEPRECATED
        * 2nd phase constructor.
        * @param aPathRepository Path repository for handling multiple directories
        * @param aRFs File server session
        */
        void ConstructL(CXnPathRepository& aPathRepository, RFs& aRFs);

        /**
        * Add a library
        * @param aItem Library to add
        * @since Series 60 3.1
        */
        TInt AddItem(CLibraryItem* aItem);

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
        // Path repository, not owned by the loader
        CXnPathRepository* iPathRepository;
        // array of libraries
        RPointerArray<CLibraryItem> iLibraries;
        // file server session
        RFs* iRFs;
    };

#endif      // CXnLIBRARYLOADER_H
            
// End of File
