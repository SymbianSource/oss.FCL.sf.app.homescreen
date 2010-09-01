/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Document class
 *
*/


#ifndef TSDOCUMENT_H
#define TSDOCUMENT_H

#include <e32base.h> 
#include <AknDoc.h>

class CTsDocument : public CAknDocument
    {
public:
    /**
     * Construct a CTsDocument for the AVKON application aApp
     * using two phase construction, and return a pointer to the created object
     * aApp application creating this document
     *
     * @since S60 v5.0
     * @return a pointer to the created instance of CTsDocument
     */
    static CTsDocument* NewL( CEikApplication& aApp );

    /**
     * Construct a CTsDocument for the AVKON application aApp
     * using two phase construction, and return a pointer to the created object
     *
     * @since S60 v5.0
     * @param aApp application creating this document
     * @return a pointer to the created instance of CTsDocument
     */
    static CTsDocument* NewLC( CEikApplication& aApp );

    /**
     * Destroy the object and release all memory objects
     *
     * @since S60 v5.0
     */
    ~CTsDocument( );

    /**
     * Makes Startup-application hidden in menu shell and fastswap window
     *
     * @since S60 v5.0
     * @param aWgName application windows group name
     */
    void UpdateTaskNameL( CApaWindowGroupName* aWgName );

public:
    // from CAknDocument
    /**
     * Create a CTsAppUi object and return a pointer to it
     *
     * @since S60 v5.0
     * @return a pointer to the created instance of the AppUi created
     */
    CEikAppUi* CreateAppUiL( );

private:
    /**
     * Perform the second phase construction of a CTsDocument object
     *
     * @since S60 v5.0
     */
    void ConstructL( );

    /**
     * Perform the first phase of two phase construction
     *
     * @since S60 v5.0
     * @param aApp application creating this document
     */
    CTsDocument( CEikApplication& aApp );
    };

#endif // TSDOCUMENT_H
