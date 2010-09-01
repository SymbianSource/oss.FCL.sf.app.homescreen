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


#ifndef HNMDTOOLBAR_H_
#define HNMDTOOLBAR_H_

#include <e32base.h>
#include <xmlengelement.h>

class CLiwGenericParamList;
class CHnMdButton;
struct THnMdCommonPointers;
class TXmlEngElement;
class CHnToolbarModel;

/**
 * Toolbar item.
 * 
 * This is the class which represents the toolbar item.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */

NONSHARABLE_CLASS( CHnMdToolbar ) : public CBase
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Constructed object.
     */
    static CHnMdToolbar* NewLC( TXmlEngElement aElement,
       THnMdCommonPointers* aCmnPtrs );
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     * @return Constructed object.
     */
     static CHnMdToolbar* NewL( TXmlEngElement aElement,
           THnMdCommonPointers* aCmnPtrs );


    /**
     * Standard C++ virtual destructor.
     *    
     * @since S60 5.0
     */
    ~CHnMdToolbar( );
    
    /**
     * Standard C++ virtual destructor.
     *    
     * @since S60 5.0
     * @param aToolbar Fills toolbar model with buttons.
     * @param aQueryResults CLiwGenericParamList object.
     * @param aPos Position of the record in the params list.
     */
    void EvaluateL( CHnToolbarModel* aToolbar, 
                    const CLiwGenericParamList& aQueryResults, 
                    TInt aPos = 0 );
    
private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdToolbar();

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void ConstructL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );
    
    /**
     * Creates buttons for toolbar.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void CreateButtonsL( TXmlEngElement aElement,
            THnMdCommonPointers* aCmnPtrs );
        
private: // data

    /**
     * Buttons for toolbar created from xml configuration.
     * Own.
     */
    RPointerArray< CHnMdButton > iButtons;
    };

#endif /*HNMDTOOLBAR_H_*/

