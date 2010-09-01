/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for CXnNewsticker.
*
*/


#ifndef _XNNEWSTICKER_H
#define _XNNEWSTICKER_H

//  INCLUDES
#include "xncomponent.h"
#include "mxncomponentinterface.h"

class CGulIcon;

// CLASS DECLARATION
/**
*  Component UI factory interface for creating newsticker text element.
*
*  @lib xnnewstickerfactory.dll
*  @since Series 60 3.2
*/
namespace XnNewstickerInterface
{
_LIT8(KType, "newsticker");

class MXnNewstickerInterface : public XnComponentInterface::MXnComponentInterface
    {
    public: // New functions

        static inline const TDesC8& Type()
            {
            return KType;
            }

        /**
         * Append new title to the end of the title list.
         * @param aTitle The new title to add.
         */
        virtual void AppendTitleL(const TDesC& aTitle) = 0;

        /**
         * Insert new title to the end of the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
         */
        virtual void InsertTitleL(const TDesC& aTitle, TInt aIndex) = 0;

        /**
         * Update title text.
         * @param aTitle The new title to add.
         * @param aIndex The place to update the title.
         */
        virtual void UpdateTitleL(const TDesC& aTitle, TInt aIndex) = 0;

        /**
         * Delete the title.
         * @param aIndex The title index to delete.
         */
        virtual void DeleteTitle(TInt aIndex) = 0;

    	/**
    	 * Returns index of the currently selected title.
    	 * @return The index of the current title.
    	 */
        virtual TInt CurrentTitleIndex() = 0;

    	/**
    	 * Return the title of the given index.
    	 * @param aIndex The index to return the title.
    	 * @return The title of the given index.
    	 */
        virtual const TDesC& Title(TInt aIndex) = 0;

        /**
         * Delete all titles.
         */
        virtual void ClearTitles() = 0;
    };
}

/**
* @ingroup group_xnnewstickerfactory
* 
*  Newsticker class implementation.
*
*  @lib xnnewstickerfactory.dll
*  @since Series 60 3.2
*/
class CXnNewsticker : public CXnComponent, public XnNewstickerInterface::MXnNewstickerInterface
    {
    public:
    	/**
    	 * 2 phase construction.
    	 */
    	static CXnNewsticker* NewL();

    	/**
    	 * Destructor.
    	 */
    	virtual ~CXnNewsticker();

    public:
         // Routed from MXnNewstickerInterface.

        /**
         * Append new title to the end of the title list.
         * @param aTitle The new title to add.
         */
        void AppendTitleL(const TDesC& aTitle);

        /**
         * Insert new title to the end of the title list.
         * @param aTitle The new title to add.
         * @param aIndex The place to add the title.
         */
        void InsertTitleL(const TDesC& aTitle, TInt aIndex);

        /**
         * Update title text.
         * @param aTitle The new title to add.
         * @param aIndex The place to update the title.
         */
        void UpdateTitleL(const TDesC& aTitle, TInt aIndex);
        
        /**
         * Delete the title.
         * @param aIndex The title index to delete.
         */
        void DeleteTitle(TInt aIndex);

    	/**
    	 * Returns index of the currently selected title.
    	 * @return The index of the current title.
    	 */
        TInt CurrentTitleIndex();

    	/**
    	 * Return the title of the given index.
    	 * @param aIndex The index to return the title.
    	 * @return The title of the given index.
    	 */
        const TDesC& Title(TInt aIndex);

        /**
         * Delete all titles.
         */
        void ClearTitles();

	   	/**
        * Create a component interface according to the given type.
        * @param aType Type of the interface to create
        * @return Created interface or NULL if the provided type is not supported.
        */ 
        virtual XnComponentInterface::MXnComponentInterface* MakeInterfaceL(
            const TDesC8& aType);        
    	      
    private:

    	CXnNewsticker();

    	void ConstructL();
    };


#endif      // _XNNEWSTICKER_H
            
// End of File
