/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation wrapper for CEikEdwin
*
*/

#ifndef _XNTEXTEDITOR_H
#define _XNTEXTEDITOR_H

//  INCLUDES
#include "xncomponent.h"
#include <e32base.h>
#include "xnnode.h"
#include "mxncomponentinterface.h"


// CLASS DECLARATION

namespace XnTextEditorInterface
    {
    _LIT8( KType, "texteditor" );

    class MXnTextEditorInterface : public XnComponentInterface::MXnComponentInterface
        {
        public:
        		
            static inline const TDesC8& Type()
                {
                return KType;
                }
		
        public: // New functions

        /**
         * Sets the new text to the underlying CEikEdvin
         * @param aText The new content
         */
        virtual void SetTextL(const TDesC& aText) = 0;
        
        /**
         * Returns the text contained in the underlying CEikEdvin
         * @return HBufC* The text
         */
        virtual const HBufC* Text() = 0;

        private:
        };
    }   

/**
*  @ingroup group_xntexteditorfactory
*  @lib xntexteditorfactory.lib
*  @since S60
*/
class CXnTextEditor : public CXnComponent, public XnTextEditorInterface::MXnTextEditorInterface
    {
public:

	/**
	 * 2 phase construction.
	 */
	static CXnTextEditor* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnTextEditor();

public: // New functions

    /**
     * Sets the new content to the underlying CEikEdvin
     * @param aText The new content
     */
    void SetTextL(const TDesC& aText);
    
    /**
     * Returns the text contained in the underlying CEikEdvin
     * @return HBufC* The text
     */
    const HBufC* Text();

public: // from CCoeControl

   	/**
     * Create a component interface according to the given type.
     * @param aType Type of the interface to create
     * @return Created interface or NULL if the provided type is not supported.
     */ 
    virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL( const TDesC8& aType );        
            
private:

	CXnTextEditor();
	void ConstructL();
    };

#endif      // _XNTEXTEDITOR_H
            
// End of File
