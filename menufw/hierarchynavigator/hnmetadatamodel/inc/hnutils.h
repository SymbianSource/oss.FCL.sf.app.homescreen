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
*
*/



#ifndef C_HNUTILS_H
#define C_HNUTILS_H

#include <e32base.h>

class CHnMdLocalization;
class CHnMdBaseKey;
class CLiwGenericParamList;
class TLiwVariant;
class CHnSuiteModel;
class TXmlEngElement;


/**
 * Utils
 * 
 * Utility class.
 *
 * @lib hnmetadatamodel
 * @since S60 3.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( HnUtils )
    {   
public:
    
   /**
    * Read file.
    *
    * @since S60 v3.0
    * @param aPath Path to a file.
    * @return Buffer.
    */    
    IMPORT_C static HBufC8* ReadFileLC(const TDesC& aPath);
   
   /**
    * Read file.
    *
    * @since S60 v3.0
    * @param aPath Path to a file.
    * @return Buffer.
    */    
    IMPORT_C static HBufC8* ReadFileL(const TDesC& aPath);
    
   /**
    * Locates file.
    *
    * @since S60 v3.0
    * @param aFile File name.
    * @return File path.
    */
    IMPORT_C static HBufC* LocateFileLC( const TDesC& aFile );
    
    /**
     * Locates nearest Language file.
     *
     * @since S60 v3.0
     * @param aFile File name.
     * @return File path.
     */
    IMPORT_C static HBufC* LocateNearestLanguageFileLC( const TDesC& aFile );

   

    /**
     * Sets given descriptor using xml element textual value.
     * 
     * @since S60 v5.0
     * @param aElement Xml element.
     * @param aAttribute Descriptor to be set using aElement.
     */
    IMPORT_C static void SetAttributeL( const TXmlEngElement& aElement, 
            RBuf8& aAttribute );
    
    /**
     * Sets given descriptor using xml attribute. 
     * 
     * @since S60 v5.0
     * @param aElement Xml element from which the attribute will be retrieved
     *                 by aAttributeName.
     * @param aAttributeName The name of the attribute to be read from
     *                 aElement.
     * @param aAttribute A descriptor to be set using a value of the attribute
     *                 from aElement.
     */
    IMPORT_C static void SetAttributeL( const TXmlEngElement& aElement, 
            const TDesC8& aAttributeName, RBuf8& aAttribute );
    
    /**
     * Fetches given columns string.
     * 
     * @since S60 v5.0
     * @param aColumn Which column to fetch.
     * @param aSourceText Source texts.
     * @param aColumnSeparator Character that separates columns.
     */
    IMPORT_C static HBufC8 * GetColumnTextLC(TInt aColumn, const TDesC8 & aSourceText, 
            TChar aColumnSeparator = TChar('|'));
    
    /**
     * Fetches number of columns.
     * 
     * @since S60 v5.0
     * @param aSourceText Source texts.
     * @param aColumnSeparator Character that separates columns.
     */
    IMPORT_C static TInt GetColumnTextColumnCount(const TDesC8 & aSourceText, 
                TChar aColumnSeparator = TChar('|'));
    
private:
    };

#endif // C_HNUTILS_H

