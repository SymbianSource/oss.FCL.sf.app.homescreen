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
* Description:  Associative container.
*
*/



#ifndef CXNMAP_H
#define CXNMAP_H

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
class MXnComparator;
// CLASS DECLARATION

/**
* @ingroup group_xnutils
* 
*  Associative container, works like a map or a hashtable
*  @since Series 60 3.1
*/
class CXnMap : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aComparatorFunctor A comparator used for testing object equality. Must not be NULL. Map assumes ownership of the comparator.
        */
        IMPORT_C static CXnMap* NewL(MXnComparator* aComparatorFunctor);

        /**
        * Destructor.
        */
        virtual ~CXnMap();
        
    public: // New functions
        
        /**
        * Puts an object to the map. The map will assume ownership of the object. If an equal object is already in the map, it will be replaced with the new object and the old object will be destroyed.
        * @since Series 60 3.1
        * @param aObject Object to add. 
        * @exception KXnErrAppendingMapItemFailed_1 Appending new map item failed.
        * @exception KXnErrAppendingMapItemFailed_2 Appending new map item failed.
        */
		IMPORT_C void PutL(CBase* aObject);

        /**
        * Gets an object from the map. The map will retain ownership of the object, the caller must not destroy the object. The returned pointer will become invalid if the object is replaced in the map by a subsequent PutL().
        * @since Series 60 3.1
        * @param aObject Object key
        * @return The object mapped to the key or NULL if not found. 
        */
		IMPORT_C CBase* Get(CBase& aObject) const;

        /**
        * Gets access to the underlying container
        * @since Series 60 3.1
        * @return The undelying container
        */
		IMPORT_C RPointerArray<CBase>& Container();

        /**
        * Find an object from the map. The map will retain ownership of the object, the caller must not destroy the object. The returned pointer will become invalid if the object is replaced in the map by a subsequent PutL().
        * @since Series 60 3.1
        * @param aObject Object key
		* @param aComparator Comparator used for finding the object
        * @return The object mapped to the key matching the comparator or NULL if not found. 
        */
		IMPORT_C CBase* Find(CBase& aObject, MXnComparator& aComparator) const;


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
        CXnMap();

        /**
        * 2nd phase constructor.
        * @param aComparatorFunctor A comparator used for testing object equality
        */
        void ConstructL(MXnComparator* aComparatorFunctor);

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
        // Comparator functor;
        MXnComparator* iComparator;
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CXnMAP_H
            
// End of File
