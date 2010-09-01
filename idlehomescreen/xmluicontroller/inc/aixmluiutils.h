/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Collection of common utility functions
*
*/


#ifndef AIXMLUIUTILS_H
#define AIXMLUIUTILS_H

#include <e32std.h>

class CXnNodeAppIf;
class CGulIcon;
class TAiPolicyElement;
class RPropertyHashMap;

namespace AiXmlUiController
    {
    class CCssPropertyMap;
  
  
/**
 * Finds a property element identified by the specified class.
 *
 * @param aUiElement the UI element associated with the property.
 * @param aPropertyClass the class of the property.
 * @return pointer array to the property elements. Ownership of the array
 *         is transferred. Ownership of the elements is not transferred.
 */
RPointerArray< CXnNodeAppIf > FindPropertyElementL( CXnNodeAppIf& aUiElement,
                                                    const TDesC8& aPropertyClass );

/**
 * Finds a property element identified by the specified class.
 *
 * @param aArray The Array where to append the nodes. 
 * @param aUiElement the UI element associated with the property.
 * @param aPropertyClass the class of the property.
 * @return pointer array to the property elements. Ownership of the array
 *         is transferred. Ownership of the elements is not transferred.
 */
void FindPropertyElementL( RPointerArray< CXnNodeAppIf >& aArray, CXnNodeAppIf& aUiElement, const TDesC8& aPropertyClass );

/**
 * Gets a property value from an UI element. The ownership of value is
 * transferred to caller.
 *
 * @param aUiElement the UI element to fecth the value from
 * @param aPropertyName the name of the property to fetch.
 * @return the value or NULL.
 */
HBufC* PropertyValueL( const CXnNodeAppIf& aUiElement,
                       const TDesC8& aPropertyName );

/**
 * Gets a property value from an UI element.
 *
 * @param aUiElement the UI element to fecth the value from
 * @param aPropertyName the name of the property to fetch.
 * @return the value or NULL.
 */
const TDesC8* PropertyValue( const CXnNodeAppIf& aUiElement,
                             const TDesC8& aPropertyName );

/**
 * Sets a property value into an UI element
 *
 * @param aUiElement the UI element to set the value into.
 * @param aPropertyName the name of the property to set.
 * @param aPropertyValue the new value.
 * @param aPropertyMap map of the original property values
 */
void SetPropertyL( CXnNodeAppIf& aUiElement,
                   const TDesC8& aPropertyName,
                   const TDesC8& aPropertyValue,
                   CCssPropertyMap& aPropertyMap );

/**
 * Sets a property values into hash map
 *
 * @param aArray the element array.
 * @param aPropertyMap map of the original property values
 * @param aPropertyHashMap hash map of the new property value arrays
 */
void SetPropertiesToHashMapL( RArray<TAiPolicyElement>& aArray,
                              CCssPropertyMap& aPropertyMap,
                              RPropertyHashMap& aPropertyHashMap );

/**
 * Sets all property values into an UI element(s)
 *
 * @param aPropertyMap map of the property arrays
 */
void SetPropertyArraysL( RPropertyHashMap& aPropertyHashMap );

/**
 * Sets a property value directly into an UI element
 *
 * @param aUiElement the UI element to set the value into.
 * @param aPropertyName the name of the property to set.
 * @param aPropertyValue the new value.
 */
void SetPropertyToNodeL( 
    CXnNodeAppIf& aNode, 
    const TDesC8& aPropertyName, 
    const TDesC8& aNewValueString );

/**
 * Leaves with specific error if given pointer is null.
 *
 * @param aPtr pointer to test.
 * @param aError error for leave.
 * @return aPtr if it is not NULL.
 */
template< class PtrT > inline
PtrT* LeaveIfNull( PtrT* aPtr, TInt aError );

/**
 * Loads a resource icon from the skin or from a mif-file 
 * and puts it into cleanup stack.
 *
 * Leavecodes: 
 * - KErrNotSupported: Filetype is not supported
 * - KErrNotFound: Syntax is incorrect or icon not found 
 *
 * Syntaxes:
 * - Skin: SKIN(<majorid> <minorid>)
 * - Mif: file_name.mif#<index_in_mif>
 *
 * @param aResource resource node.
 * @return icon.
 */
CGulIcon* LoadIconLC( CXnNodeAppIf& aResource );

    }

#include "aixmluiutils.inl"

#endif // AIXMLUIUTILS_H
