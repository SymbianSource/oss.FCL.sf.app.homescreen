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
* Description:  Manages ecom plugins
*
*/



#ifndef CXNECOMHANDLER_H
#define CXNECOMHANDLER_H

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
class CXnArray;

// CLASS DECLARATION

/**
* @ingroup group_xnutils
* 
*  Manages ecom plugins
*  @since Series 60 3.1
*/
class CXnEcomHandler : public CBase
    {
    public:  // Constructors and destructor
        struct CEcomItem;  
              
        /**
        * Two-phased constructor.
        * @param aRFs File server session
        */
        IMPORT_C static CXnEcomHandler* NewL();
        
        
        /**
        * Destructor.
        */
        virtual ~CXnEcomHandler();

    public: // New functions
        /**
        * Get pointers to plugins
        * @param aInterfaceUid Interface uid to use
        * @param aType Type of plugin to fetch
        * @param aFunctions Array in which to copy plugin pointers
        * @since Series 60 3.1
        */
        IMPORT_C void PluginsL(TUid aInterfaceUid, const TDesC8& aType, CXnArray& aFunctions);
        
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


    private:
        //?type ?member_function();
        /**
        * C++ default constructor.
        */
        CXnEcomHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();


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
        // array of ecom plugins
        RPointerArray<CEcomItem> iPlugins;
    };

#endif      // CXNECOMHANDLER_H
            
// End of File
