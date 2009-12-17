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
* Description:  
*
*/

#ifndef HSCONTENTCONTROLFACTORY_H
#define HSCONTENTCONTROLFACTORY_H

// System includes
#include <e32base.h>

// Forward declarations
#include <hscontentcontrolui.h>
/**
 *  Content control UI base class
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHsContentControlFactory ) : public CBase
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CHsContentControlFactory* NewL();

    /**
     * Destructor.
     */
    IMPORT_C ~CHsContentControlFactory();

private: // Constructors
    /**
     * Constructor
     */
    CHsContentControlFactory();

    /** Second phase constructor */
    void ConstructL();


public: // New functions
    /**
     * 
     */
    IMPORT_C MHsContentControlUi* GetHsContentController(
        const TDesC8& aControllerType );

private:
    /**
     * 
     */
    MHsContentControlUi* FindHsContentController(
        const TDesC8& aControllerType );
    
    
private: // Data

    /**
     *
     */
    RPointerArray< CHsContentControlUi > iHsContentControlUis;

    };


#endif // HSCONTENTCONTROLFACTORY_H

// End of file
