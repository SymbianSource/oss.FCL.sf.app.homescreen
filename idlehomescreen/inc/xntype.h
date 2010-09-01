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
* Description:  Hand-made RTTI type object.
*
*/



#ifndef CXNTYPE_H
#define CXNTYPE_H

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
* @ingroup group_xnutils
* 
*  Hand-made RTTI type object.
*  @since Series 60 3.1
*/
class CXnType : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aType Type identifier
        */
        IMPORT_C static CXnType* NewL(const TDesC8& aType);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXnType();
        
    public: // New functions
        
        /**
        * Get the type identifier
        * @since Series 60 3.1
        * @return Type identifier
        */
		IMPORT_C const TDesC8& Type();


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
        * C++ default constructor. Protected in order to allow for derivation.
        */
        IMPORT_C CXnType();

        /**
        * 2nd phase constructor. Protected in order to allow for derivation.
        * @param aType Type identifier
        */
        IMPORT_C void ConstructL(const TDesC8& aType);

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();


    private:


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
        // Type identifier
        const TDesC8* iType;
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CXnTYPE_H
            
// End of File
