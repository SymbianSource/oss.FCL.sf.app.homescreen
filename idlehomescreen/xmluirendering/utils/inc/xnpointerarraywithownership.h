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
* Description:  Heap-allocated pointer array which owns its objects
*
*/



#ifndef CXNPOINTERARRAYWITHOWNERSHIP_H
#define CXNPOINTERARRAYWITHOWNERSHIP_H

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
//class ?FORWARD_CLASSNAME;
// CLASS DECLARATION

/**
*  Heap-allocated pointer array which owns its objects
*  @since Series 60 3.1
*/
class CXnPointerArrayWithOwnership : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnPointerArrayWithOwnership* NewL();

        /**
        * Destructor.
        */
        virtual ~CXnPointerArrayWithOwnership();
        
    public: // New functions
        
        /**
        * Get the underlying container
        * @since Series 60 3.1
        * @return Underlying container
        */

        IMPORT_C RPointerArray<CBase>& Container();

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
        CXnPointerArrayWithOwnership();

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
        // ?one_line_short_description_of_data
        //?data_declaration;
        // Reserved pointer for future extension
        //TAny* iReserved;
        // Internal container
        RPointerArray<CBase> iContainer;
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CXnPointerARRAYWITHOWNERSHIP_H
            
// End of File
