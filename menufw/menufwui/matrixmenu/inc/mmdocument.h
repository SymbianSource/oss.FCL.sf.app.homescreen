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
* Description:  Application Document class
*
*/


#ifndef C_MMDOCUMENT_H
#define C_MMDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CMmAppUi;
class CEikApplication;

/**
 *  @ingroup group_matrixmenu
 *  Matrix Menu Application Document.
 *  AVKON Application Document class.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CMmDocument) : public CAknDocument
	{
	
public: // Constructors and destructor

	/**
	 * Two-phased constructor.
	 * Construct a CMatrixMenuDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * 
	 * @since S60 v5.0
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CMmDocument.
	 */
	static CMmDocument* NewL( CEikApplication& aApp );

	/**
	 * Two-phased constructor.
	 * Construct a CMatrixMenuDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * 
	 * @since S60 v5.0
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CMmDocument.
	 */
	static CMmDocument* NewLC( CEikApplication& aApp );

	/**
	 * Virtual Destructor.
	 * 
	 * @since S60 v5.0
	 */
	virtual ~CMmDocument();

public: // Functions from base classes

	/**
	 * From CEikDocument, CreateAppUiL.
	 * Create a CMmAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * 
     * @since S60 v5.0
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();
	
private: // Constructors

	/**
	 * 2nd phase constructor.
	 * 
	 * @since S60 v5.0
	 */
	void ConstructL();

	/**
	 * C++ default constructor.
	 * 
	 * @since S60 v5.0
	 * @param aApp Application creating this document.
	 */
	CMmDocument( CEikApplication& aApp );

	};

#endif // C_MMDOCUMENT_H

// End of File
