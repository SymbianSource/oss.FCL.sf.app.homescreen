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
* Description:  Common utility functions
*
*/


#include "xnchilditeratorbuilder.h"
#include "xnnodeappif.h"
#include "xnproperty.h"
#include "xndomlist.h"
#include "mxndomlistitem.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xnuiengineappif.h"

#include <AknsItemID.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <gulicon.h>
#include <SVGEngineInterfaceImpl.h>
#include <gdi.h>
#include <mifconvdefs.h>
#include "xnnode.h"	

#include "aixmluiutils.h"
#include "aixmluiconstants.h"
#include "csspropertymap.h"
#include "aipolicyelement.h"
#include "transaction.h"
#include "debug.h"

#include "xnbreadthfirsttreeiterator.h"
namespace
    {
    
    /**
     * CSS Property value update types.
     */
    enum TPropertyUpdateType
        {
        EAbsoluteValue = 0,
        EAddition,
        ESubtraction,
        EIncrementalAddition,
        EIncrementalSubtraction
        };
    
    /**
     *
     */
    _LIT8( KAdditionOperator, "+" );

    _LIT8( KIncrementalAdditionOperator, "++" );
    
    _LIT8( KSubtractionOperator, "-" );

    _LIT8( KIncrementalSubtractionOperator, "--" );
    
    const TInt KLenAddSubOperator = 1;

    const TInt KLenIncrementalOperator = 2;
         
    _LIT( KMIFExtension, ".mif" );
    
   // _LIT( KSVGExtension, ".svg" );
    
    _LIT( KHashmark, "#" );
    
    const TUint8 KDoubleQuote = '"';
    
    const TUint8 KSingleQuote = '\'';
    
    const TInt KLenDoubleQuotes = 2;
    const TInt KDepthT = 1;
    
    class TIconProvider : public MAknIconFileProvider
    {
    public:
        /**
        * Constructor
        * @param aFile File handle to use
        */
        TIconProvider(RFile& aFile) : iFile(aFile)
            {
            }
    public:

        /**
        * Destructor.
        */
        virtual ~TIconProvider()
            {
            iFile.Close();
            }
        // Functions from base classes
        /**
        * From MAknIconFileProvider Returns an open file handle to the icon file.
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
        virtual void RetrieveIconFileHandleL(
            RFile& aFile, const TIconFileType /*aType*/ )
            {
            aFile.Duplicate(iFile);
            }

        /**
        * From MAknIconFileProvider With this method, AknIcon framework informs that it does not use
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
        virtual void Finished()
            {
            iFile.Close();
            delete this;
            }
    private:
        // file to use
        RFile iFile;        
    }; 
    
     /**
     * Tests if string ends with given pattern
     * 
     * @param aString input string
     * @param aPattern test pattern
     * @return ETrue if string ends with given pattern.
     */
    TBool EndsWith( const TDesC8& aString, const TDesC8& aPattern )
        {
        return ( aString.Right( aPattern.Length() ) == aPattern );
        }

  /**
     * Tests if string ends with given pattern
     * 
     * @param aString input string
     * @param aPattern test pattern
     * @return ETrue if string ends with given pattern.
     */
    TBool EndsWith( const TDesC& aString, const TDesC& aPattern )
        {
        return ( aString.Right( aPattern.Length() ) == aPattern );
        }    
    /**
     * Removes pattern from the end of string. Function does not validate
     * that the pattern is in the end of string. It just cuts out the number
     * of aPattern length characters from the string.
     *
     * @param aString input string
     * @param aPattern pattern
     * @return string without pattern.
     */
    TPtrC8 Strip( const TDesC8& aString, const TDesC8& aPattern )
        {
        return aString.Left( aString.Length() - aPattern.Length() );
        }
    
    /**
     * Selects a proper string data value type
     */
    CXnDomPropertyValue::TPrimitiveValueType SelectStringDataType(
                                                    const TDesC8& aPropertyName,
                                                    const TDesC8& aPropertyValue )
        {
        if ( aPropertyName == XnPropertyNames::style::common::KDisplay
             && ( aPropertyValue == XnPropertyNames::style::common::display::KBlock
                  || aPropertyValue == XnPropertyNames::style::common::display::KNone ) )
            {
            return CXnDomPropertyValue::EString;
            }
        
        if ( aPropertyName == XnPropertyNames::style::common::KVisibility
             && ( aPropertyValue == XnPropertyNames::style::common::visibility::KVisible
                  || aPropertyValue == XnPropertyNames::style::common::visibility::KHidden ) )
            {
            return CXnDomPropertyValue::EString;
            }
        if ( aPropertyName == XnPropertyNames::style::common::KNavIndex )
            {
            return CXnDomPropertyValue::EIdent;
            }
            
        return CXnDomPropertyValue::EUnknown;
        }
    
    /**
     * Validates string
     */
    TBool IsValidCssString( const TDesC8& aString )
        {
        // Null string or unquoted string is not a CSS string
        if ( aString.Length() < KLenDoubleQuotes )
            {
            return EFalse;
            }
        
        const TUint8 quotationMark( aString[0] );    
        if ( ( quotationMark == KDoubleQuote || quotationMark == KSingleQuote )
             && aString[ aString.Length() - 1 ] == quotationMark )
            {
            return ETrue;
            }
        
        return EFalse;
        }
    
    /**
     * Removes extra quotes from strings. Assumes that the string is quoted.
     */ 
    TPtrC8 RemoveQuotes( const TDesC8& aString )
        {
        return aString.Mid( 1, aString.Length() - KLenDoubleQuotes );
        }
    
    /**
     * Parses a primitive data type of a string.
     *
     * @param aInputValue[in]   input value with primitive type information.
     * @param aStringValue[out] value without type information.
     * @param aFloatValue       value as float if type is a numerical type.
     * @param aPrimitiveType[out]        primitive type.
     * @param aUpdateType[out]  update type
     */
    void ParseCSSValue( const TDesC8& aInputValue,
                        TPtrC8& aStringValue,
                        TReal& aFloatValue,
                        CXnDomPropertyValue::TPrimitiveValueType& aPrimitiveType,
                        TPropertyUpdateType& aUpdateType )
        {
        aPrimitiveType = CXnDomPropertyValue::EUnknown;
        aUpdateType = EAbsoluteValue;
        
        TPtrC8 unit;
        
        // Recognise type
        
        if ( EndsWith( aInputValue, AiUiDef::xml::css::KPercentage ) )
            {
            unit.Set( AiUiDef::xml::css::KPercentage );
            aPrimitiveType = CXnDomPropertyValue::EPercentage;
            }
        else if ( EndsWith( aInputValue, AiUiDef::xml::css::KPixel ) )
            {
            unit.Set( AiUiDef::xml::css::KPixel );
            aPrimitiveType = CXnDomPropertyValue::EPx;
            }
        else if ( EndsWith( aInputValue, AiUiDef::xml::css::KUnitValue ) )
            {
            unit.Set( AiUiDef::xml::css::KUnitValue );
            aPrimitiveType = CXnDomPropertyValue::EUnitValue;
            }
        else if ( IsValidCssString( aInputValue ) )
            {
            aPrimitiveType = CXnDomPropertyValue::EString;
            aStringValue.Set( RemoveQuotes( aInputValue ) );
            return;
            }
            
        // Validate type
        switch ( aPrimitiveType )
            {
            // Supported float types
            case CXnDomPropertyValue::EPercentage:
            case CXnDomPropertyValue::EPx:
            case CXnDomPropertyValue::EUnitValue: // fallthrough
                {
                // Remove unit
                aStringValue.Set( Strip( aInputValue, unit ) );
                
                // Determine update type
                if ( aStringValue.Find( KIncrementalAdditionOperator ) == 0 )
                    {
                    aUpdateType = EIncrementalAddition;
                    
                    // Remove sign
                    aStringValue.Set( aStringValue.Mid( KLenIncrementalOperator ) );
                    }
                else if ( aStringValue.Find( KAdditionOperator ) == 0 )
                    {
                    aUpdateType = EAddition;
                    
                    // Remove sign
                    aStringValue.Set( aStringValue.Mid( KLenAddSubOperator ) );
                    }
                else if ( aStringValue.Find( KIncrementalSubtractionOperator ) == 0 )
                    {
                    // Use addition, so only one sign has to be removed
                    aUpdateType = EAddition;
                    
                    aStringValue.Set( aStringValue.Mid( KLenAddSubOperator ) );
                    }
                else if ( aStringValue.Find( KSubtractionOperator ) == 0 )
                    {
                    // Use addition, so sign does not have to be removed
                    aUpdateType = EAddition;
                    }

                if ( TLex8( aStringValue ).Val( aFloatValue ) != KErrNone )
                    {
                    // Not a numerical value. Restore value
                    aPrimitiveType = CXnDomPropertyValue::EUnknown;
                    aUpdateType = EAbsoluteValue;
                    aStringValue.Set( aInputValue );
                    }
                
                break;
                }
            
            // string types    
            default:
                {
                aStringValue.Set( aInputValue );
                break;
                }
            
            }
        }
    
    /**
     * Updates property value
     *
     * @param aValue property value to update.
     * @param aStringValue new value as string
     * @param aFloatValue new value as float (for number types)
     * @param aPrimitiveType type of value
     * @param aUpdateType type to update the value.
     */
    void UpdatePropertyValueL( CXnDomPropertyValue& aValue,
                               const TDesC8& aStringValue,
                               const TReal& aFloatValue,
                               CXnDomPropertyValue::TPrimitiveValueType aPrimitiveType,
                               TPropertyUpdateType aUpdateType )
        {
        switch ( aPrimitiveType )
            {
            // Supported float types
            case CXnDomPropertyValue::EPercentage:
            case CXnDomPropertyValue::EPx:
            case CXnDomPropertyValue::EUnitValue: // fallthrough
                {
                TReal floatValue( aFloatValue );
                // Select update type
                switch ( aUpdateType )
                    {
                    case EAddition:  // fallthrough
                    case EIncrementalAddition:
                        {
                        floatValue += aValue.FloatValueL();
                        break;
                        }
                        
                    case ESubtraction: // fallthrough
                    case EIncrementalSubtraction:
                        {
                        floatValue = aValue.FloatValueL() - aFloatValue;
                        break;
                        }
                    
                    case EAbsoluteValue:
                    default:
                        {
                        break;
                        }
                    }
                
                // Set new value as float
                aValue.SetFloatValueL( aPrimitiveType, floatValue );
                
                break;
                }
            
            // string types
            case CXnDomPropertyValue::EUnknown:
                {
                // Use current type
                aPrimitiveType = Max( aValue.PrimitiveValueType(), CXnDomPropertyValue::EUnknown );
                
                // Fall through to update...
                }
                
            default: // fallthrough
                {
				if( aPrimitiveType == CXnDomPropertyValue::ENumber )
					{
					TReal floatValue;
					TLex8( aStringValue ).Val( floatValue );
					aValue.SetFloatValueL( aPrimitiveType, floatValue );
					}
				else
					{
                    // Set new value as string
                     aValue.SetStringValueL( aPrimitiveType, aStringValue );
					}
                break;
                }
            }
        }
    
    /**
     * Creates a new property.
     *
     * @param aUiElement ui element to hold the property.
     * @param aPropertyName name of the property.
     * @param aPropertyValue value of the property.
     */
    void CreatePropertyL( CXnNodeAppIf& aUiElement,
                          const TDesC8& aPropertyName,
                          const TDesC8& aPropertyValue )
        {
        // Create new property value and property
        CXnDomPropertyValue* value = CXnDomPropertyValue::NewL(
                                                &aUiElement.UiEngineL()->StringPool() );
        CleanupStack::PushL( value );
        
        CXnProperty* property = CXnProperty::NewL(
                                                aPropertyName,
                                                value,
                                                aUiElement.UiEngineL()->StringPool() );
        CleanupStack::Pop( value );
        CleanupStack::PushL( property );
        
        // Parse CSS value
        TPtrC8 stringValue;
        TReal floatValue( 0 );
        CXnDomPropertyValue::TPrimitiveValueType primitiveType(
                                                CXnDomPropertyValue::EUnknown );
        TPropertyUpdateType updateType( EAbsoluteValue );
        
        ParseCSSValue( aPropertyValue,
                       stringValue,
                       floatValue,
                       primitiveType,
                       updateType );
        
        if ( primitiveType == CXnDomPropertyValue::EUnknown )
            {
            primitiveType = SelectStringDataType( aPropertyName, aPropertyValue );
            }
            
        // Update CSS value               
        UpdatePropertyValueL( *value,
                              stringValue,
                              floatValue,
                              primitiveType,
                              updateType );
        
        // Add property to UI element                      
        aUiElement.SetPropertyL( property );
        
        CleanupStack::Pop( property );
        }
    
    
    // Icon utilities
    
    _LIT( KSkin, "skin" );
    const TUint KLeftParenthesis = '(';
    
    /**
     * Resolves skin item id from pattern SKIN( <majorId> <minorId> (<colourGroupId>) ).
     * The colourGroupId in the syntax is optional, and if no value found then
     * aColourValue will be -1
     *
     * @param aItemId skin item id to fill
     * @param aColourValue colour value to fill. 
     * @param aPath   skin item id string     
     * 
     * @return ETrue if id was succesfully parsed.
     */
    TBool ResolveSkinItemId( TAknsItemID& aItemId, TInt& aColourValue, const TDesC& aPath )
        {
        // Syntax: skin( <major> <minor> )
        TInt pos = aPath.FindF( KSkin );
        aColourValue = -1;
        if( pos != KErrNotFound )
            {
            // Skip skin token
            pos += KSkin().Length();
            
            // Initialize lexer
            TLex lex( aPath.Mid( pos ) );
            lex.SkipSpace();
            
            // Check left parenthesis
            if (lex.Get() != KLeftParenthesis )
                {
                return EFalse;
                }
            
            lex.SkipSpace();
            
            TInt majorId( 0 );        
            TInt minorId( 0 );
            
            // Resolve major id        
            TInt error = lex.Val( majorId );
            
            // Resolve minor id
            lex.SkipSpace();
            error |= lex.Val( minorId );
            
            // initilize skin item id object
            aItemId.Set( majorId, minorId );
            
            lex.SkipSpace();
            TInt colorError = lex.Val( aColourValue );
            if ( colorError != KErrNone || aColourValue < 0)
                {
                aColourValue = -1;
                }
                        
            // Check error
            return ( error == KErrNone );
            }
         
        return EFalse;
        }
        
	 /**
     * Resolves MIF files filename and id from syntax
     * mif_filename.mif#id. If the syntax is incorrect
     * aId is -1 and filename zeroed and EFalse is returned
     *
     * @param aPath The path to extract the data from
     * @param aId	Id to fill
     * @param aFilename Filename to fill
     * @return ETrue if id was succesfully parsed.
     */
    TBool ResolveMifIdAndPathL( const TDesC& aPath, TInt& aId, TDes& aFilename )
        {
        // Syntax: mif_filename.mif#index
        TInt pos = aPath.FindF( KHashmark );
        aFilename.Zero();
        if( pos != KErrNotFound )
            {
            aFilename = (aPath.Left(pos));
            if ( ! EndsWith( aFilename, KMIFExtension ) )
            	{
            	aFilename.Zero();
            	return EFalse;
            	}
            
            TLex lex(aPath.Mid(pos+1));
            TInt error = lex.Val(aId);
            if ( error != KErrNone )
            	{
            	aId = -1;
            	return EFalse;
            	}
            return ETrue;
            }
           return EFalse;
            
            
      
        }        

    /**
    * Loads a bitmap icon from a MIF-file loaded to Xuikon by using
    * <FileResource> tag
    *
    * @param aNode Resource node
    * @param aFilename The filename to load
    * @param aBitmap The bitmap to fill
    * @param aBitmapMask The mask to fill
    * @param aMifId The id of the icon in the MIF-file
    *
	* @return KErrNone if loading was successful KErrNotFound if not
	*/
    TInt LoadMIFBitmapL( CXnNodeAppIf &aNode, 
    	const TDesC &aPath,
    	CFbsBitmap*& aBitmap,
    	CFbsBitmap*& aBitmapMask,
    	TInt aMifId )
    	{
    	RFile tempFile;
    	    	
    	if ( aMifId < 0 )
    		{
    		return KErrNotFound;
    		}
    	  
    	TInt err = aNode.UiEngineL()->GetThemeResource( aPath, tempFile );

		if ( err != KErrNone )
			{
			tempFile.Close();
			return err;
			}
    	
    	//Requires an open RFile handle
    	TIconProvider *iconProvider = 
    		new (ELeave) TIconProvider(tempFile);

    	CleanupStack::PushL( iconProvider );
    	// Mask is next after bitmap
        AknIconUtils::CreateIconL(aBitmap,aBitmapMask, *iconProvider,
            aMifId+KMifIdFirst,aMifId+KMifIdFirst+1);

    	CleanupStack::Pop(iconProvider); // iconProvider
          
        return KErrNone;
    	}
    }

namespace AiXmlUiController
{

RPointerArray< CXnNodeAppIf >
    FindPropertyElementL( CXnNodeAppIf& aUiElement, const TDesC8& aPropertyClass )
    {
    CXnBreadthFirstTreeIterator< CXnNodeAppIf >* iterator = 
            CXnBreadthFirstTreeIterator< CXnNodeAppIf >::NewL( aUiElement, 
            KDepthT );
    CleanupDeletePushL( iterator );
    
    RPointerArray< CXnNodeAppIf > nodes;
    
    CleanupClosePushL( nodes );
    
    CXnNodeAppIf* child = iterator->Value() ;
    
    while ( child )
        {
        CXnProperty* classProperty = child->GetPropertyL(
                                            XnPropertyNames::common::KClass );
        
        if ( classProperty && ( classProperty->StringValue() == aPropertyClass ) )
            {
            User::LeaveIfError( nodes.Append( child ) );
            }
        
        child = iterator->NextL();
        }
    
    CleanupStack::Pop( &nodes );
    
    CleanupStack::PopAndDestroy( iterator );
    
    return nodes;
    }

void FindPropertyElementL( RPointerArray< CXnNodeAppIf >& aArray, CXnNodeAppIf& aUiElement, const TDesC8& aPropertyClass )
    {
    CXnBreadthFirstTreeIterator< CXnNodeAppIf >* iterator = 
            CXnBreadthFirstTreeIterator< CXnNodeAppIf >::NewL( aUiElement,
            KDepthT );
    CleanupDeletePushL( iterator );
       
    CXnNodeAppIf* child =  iterator->Value();
    
    while ( child )
        {
        CXnProperty* classProperty = child->GetPropertyL(
                                            XnPropertyNames::common::KClass );
        
        if ( classProperty && ( classProperty->StringValue() == aPropertyClass ) )
            {
            User::LeaveIfError( aArray.Append( child ) );
            }
        
        child = iterator->NextL();
        }
   
    CleanupStack::PopAndDestroy( iterator );
    }

HBufC* PropertyValueL( const CXnNodeAppIf& aUiElement,
                       const TDesC8& aPropertyName )
    {
    CXnProperty* property = aUiElement.GetPropertyL( aPropertyName );
    
    if ( property )
        {
        return property->StringValueL();
        }
        
    return NULL;
    }

const TDesC8* PropertyValue( const CXnNodeAppIf& aUiElement,
                             const TDesC8& aPropertyName )
    {
    CXnProperty* property = NULL;
    TRAP_IGNORE( property = aUiElement.GetPropertyL( aPropertyName ) );
    
    if ( property )
        {
        return &property->StringValue();
        }
        
    return NULL;
    }

void SetPropertyL( CXnNodeAppIf& aUiElement,
                   const TDesC8& aPropertyName,
                   const TDesC8& aPropertyValue,
                   CCssPropertyMap& aPropertyMap )
    {
    CXnProperty* property = aUiElement.GetPropertyL( aPropertyName );
    
    if ( !property )
        {
        // Create new property
        CreatePropertyL( aUiElement, aPropertyName, aPropertyValue );
        return;
        }
    
    // Clone property for update
    property = property->CloneL();
    CleanupStack::PushL( property );
    
    CXnDomProperty* domProperty = property->Property();
    MXnDomListItem* item = domProperty->PropertyValueList().First();

    CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >( item );
    
    if ( value )
        {
        TPtrC8 stringValue;
        TReal floatValue(0);
                
        CXnDomPropertyValue::TPrimitiveValueType primitiveType(
                                                CXnDomPropertyValue::EUnknown );
        TPropertyUpdateType updateType( EAbsoluteValue );
        
        ParseCSSValue( aPropertyValue,
                       stringValue,
                       floatValue,
                       primitiveType,
                       updateType );
        
        if ( primitiveType == CXnDomPropertyValue::EUnknown )
            {
            primitiveType = SelectStringDataType( aPropertyName, aPropertyValue );
            }
        
        if ( updateType != EAbsoluteValue &&
             updateType != EIncrementalAddition &&
             updateType != EIncrementalSubtraction )
            {
            // Use original value from property map
            const TDesC8* elementId = PropertyValue( aUiElement,
                                                     XnPropertyNames::common::KId );
            CXnDomPropertyValue* original = NULL;
            if( elementId )
                {
                original = aPropertyMap.FindPropertyValue(
                                                        *elementId,
                                                        aPropertyName );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            
            if ( original )
                {
                // Clone original value 
                value = original->CloneL();
                CleanupStack::PopAndDestroy( property ); // Old clone
                CleanupStack::PushL( value );
                property = CXnProperty::NewL( aPropertyName,
                                              value,
                                              aUiElement.UiEngineL()->StringPool() );
                CleanupStack::Pop( value );
                CleanupStack::PushL( property );
                }
            else
                {
                // store original value
                aPropertyMap.StorePropertyValueL( *elementId, *property );
                }
            }
        
        UpdatePropertyValueL( *value,
                              stringValue,
                              floatValue,
                              primitiveType,
                              updateType );
            
            aUiElement.SetPropertyL( property );
            CleanupStack::Pop( property );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

void SetPropertiesToHashMapL( RArray<TAiPolicyElement>& aArray,
                              CCssPropertyMap& aPropertyMap,
                              RPropertyHashMap& aPropertyHashMap )
    {
    TBool propertyAdded = EFalse;
    if( !aArray.Count() )
        {
        return;
        }
        
    CXnNodeAppIf& uiElement = aArray[0].Target();
    RPointerArray<CXnProperty>* propertyArray = NULL;
    
    propertyArray = aPropertyHashMap.Find( uiElement );
    
    if( !propertyArray )
        {
        // Create new one.
        propertyArray = new(ELeave) RPropertyArray;
        CleanupStack::PushL( propertyArray );
        aPropertyHashMap.InsertL( &uiElement, propertyArray );
        CleanupStack::Pop( propertyArray );
        }
    
    for( TInt i = 0; i < aArray.Count(); ++i )
        {
        const TDesC8& propertyName = aArray[i].Name();
        const TDesC8& propertyValue = aArray[i].Value();

        CXnProperty* property = uiElement.GetPropertyL( propertyName );
        
        if ( !property )
            {
            // Create new property
            CreatePropertyL( uiElement, propertyName, propertyValue );
            propertyAdded = ETrue;
            continue;
            }
        
        // Clone property for update
        property = property->CloneL();
        CleanupStack::PushL( property );
        
        CXnDomProperty* domProperty = property->Property();
        MXnDomListItem* item = domProperty->PropertyValueList().First();

        CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >( item );
        
        if ( value )
            {
            TPtrC8 stringValue;
            TReal floatValue(0);
                    
            CXnDomPropertyValue::TPrimitiveValueType primitiveType(
                                                    CXnDomPropertyValue::EUnknown );
            TPropertyUpdateType updateType( EAbsoluteValue );
            
            ParseCSSValue( propertyValue,
                           stringValue,
                           floatValue,
                           primitiveType,
                           updateType );
            
            if ( primitiveType == CXnDomPropertyValue::EUnknown )
                {
                primitiveType = SelectStringDataType( propertyName, propertyValue );
                }
            
            if ( updateType != EAbsoluteValue )
                {
                // Use original value from property map
                const TDesC8* elementId = PropertyValue( uiElement,
                                                         XnPropertyNames::common::KId );
                CXnDomPropertyValue* original = NULL;
                if( elementId )
                    {
                    original = aPropertyMap.FindPropertyValue(
                                                            *elementId,
                                                            propertyName );
                    }
                else
                    {
                    User::Leave( KErrNotSupported );
                    }
                
                if ( original )
                    {
                    // Clone original value 
                    value = original->CloneL();
                    CleanupStack::PopAndDestroy( property ); // Old clone
                    CleanupStack::PushL( value );
                    property = CXnProperty::NewL( propertyName,
                                                  value,
                                                  uiElement.UiEngineL()->StringPool() );
                    CleanupStack::Pop( value );
                    CleanupStack::PushL( property );
                    }
                else
                    {
                    // store original value
                    aPropertyMap.StorePropertyValueL( *elementId, *property );
                    }
                }
            
                UpdatePropertyValueL( *value,
                                      stringValue,
                                      floatValue,
                                      primitiveType,
                                      updateType );
                propertyArray->AppendL( property );
                CleanupStack::Pop( property );
            }
        }
        
    if( !propertyAdded && !propertyArray->Count() )
        {
        User::Leave( KErrNotSupported );
        }
    }

void SetPropertyArraysL( RPropertyHashMap& aPropertyHashMap )
    {
    TPtrHashMapIter<CXnNodeAppIf, RPropertyArray> iter( aPropertyHashMap );
    iter.Reset();
    
    const CXnNodeAppIf* targetNode = (const CXnNodeAppIf*)iter.NextKey();
    
    while( targetNode )
        {
        CXnNodeAppIf* targetNonConst = const_cast<CXnNodeAppIf*>(targetNode);
        RPointerArray<CXnProperty>* propertyArray = aPropertyHashMap.Find(*targetNode);
        if( propertyArray )
            {
            targetNonConst->SetPropertyArrayL( propertyArray ); // Assumes ownership
                                                                // of CXnProperty's in the array
                                                                // but not the array itself.
            propertyArray->Close(); // Close and delete the array to free all memory.
            delete propertyArray;
            aPropertyHashMap.Remove( targetNode ); // Remove the key from hash map
                                                   // as the array is now destroyed.
            iter.Reset(); // Reset the iterator to avoid panic.
            }
        targetNode =  (CXnNodeAppIf*)iter.NextKey();
        }

    }

CGulIcon* LoadIconLC( CXnNodeAppIf& aResource )
    {
    // Resolve icon path
    HBufC* pathBuffer = PropertyValueL( aResource, XnPropertyNames::image::KPath );
    CleanupStack::PushL( pathBuffer );
  	
    LeaveIfNull( pathBuffer, KErrNotFound );
    
    CFbsBitmap *bitmap = NULL;
    CFbsBitmap *mask = NULL;
    CGulIcon* icon = NULL;

	TFileName fileName;
	TInt mifId(0);
  
  	//Assume that the filetype is not supported
    TInt err = KErrNotSupported;
    
    // Resolve icon id    
    TAknsItemID iconId;
    
    // Possible color index
    TInt colorValue(-1);
    
    TBool inSkin = ResolveSkinItemId( iconId, colorValue, *pathBuffer );
    
    // SKIN(<major> <minor> (<optionalColourGroup>))
    if ( inSkin )
        {
        // Load from skin
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        err = KErrNone;
        // Load the icon with colour group support but only if it is valid
        // see AknsConstants.h for these   
        if ( colorValue >= EAknsCIQsnTextColorsCG1 && 
             colorValue <= EAknsCIQsnTextColorsCG62 )
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            AknsUtils::CreateColorIconLC(
                skin,
                iconId,
                KAknsIIDQsnTextColors,
                colorValue,
                bitmap,
                mask,
                KNullDesC,  /* no backup */
                0,          /* no backup */
                0,          /* no backup */
                KRgbBlack );
            
            if ( bitmap )
                {
                icon = CGulIcon::NewL( bitmap, mask );              
                }
            else
                {
                err = KErrNotFound;
                }
            CleanupStack::Pop( 2 ); // bitmap, mask
            }
        // No colour group support
        else
            {
            icon = AknsUtils::CreateGulIconL( skin, iconId, KNullDesC, 0, 0 );            
            }

    	if ( !icon ) // Syntax correct but icon not found
    		{
    		err = KErrNotFound;	
    		} 
        }
   	
	// MIF-file <filename.mif>#<index_in_mif>
    else if ( ResolveMifIdAndPathL( *pathBuffer, mifId, fileName ) )
    	{
		err = LoadMIFBitmapL( aResource, fileName, bitmap, mask, mifId );
		
		if ( err == KErrNone && bitmap )
			{
			// Ownership of bitmap and mask transferred to CGulIcon
			icon = CGulIcon::NewL( bitmap, mask );	
			}
		else // Syntax correct but the icon was not found
			{
			err = KErrNotFound;
			}
    	}
	CleanupStack::PopAndDestroy( pathBuffer );      
    CleanupStack::PushL( icon );
    
  	User::LeaveIfError( err );
    return icon;
    }
    
void SetPropertyToNodeL( 
    CXnNodeAppIf& aNode, 
    const TDesC8& aPropertyName, 
    const TDesC8& aNewValueString )
	{
	// Set defined property to defined node.
    CXnDomPropertyValue* newValue = CXnDomPropertyValue::NewL( &aNode.UiEngineL()->StringPool() );
    CleanupStack::PushL(newValue);
    newValue->SetStringValueL(CXnDomPropertyValue::EString, aNewValueString);
    CXnProperty* prop = CXnProperty::NewL(aPropertyName, newValue, aNode.UiEngineL()->StringPool());
    CleanupStack::PushL(prop);
	aNode.SetPropertyL(prop);
    CleanupStack::Pop(prop);
    CleanupStack::Pop(newValue);				        
	}
	
} // namespace AiXmlUiController
