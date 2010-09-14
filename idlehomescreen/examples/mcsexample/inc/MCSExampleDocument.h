/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MCSEXAMPLEDOCUMENT_h__
#define __MCSEXAMPLEDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CMCSExampleAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CMCSExampleDocument application class.
 * An instance of class CMCSExampleDocument is the Document part of the
 * AVKON application framework for the MCSExample example application.
 */
class CMCSExampleDocument : public CAknDocument
    {
public:
    // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a CMCSExampleDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CMCSExampleDocument.
     */
    static CMCSExampleDocument* NewL(CEikApplication& aApp);

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a CMCSExampleDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CMCSExampleDocument.
     */
    static CMCSExampleDocument* NewLC(CEikApplication& aApp);

    /**
     * ~CMCSExampleDocument
     * Virtual Destructor.
     */
    virtual ~CMCSExampleDocument();

public:
    // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a CMCSExampleAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CMCSExampleDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CMCSExampleDocument(CEikApplication& aApp);

    };

#endif // __MCSEXAMPLEDOCUMENT_h__
// End of File
