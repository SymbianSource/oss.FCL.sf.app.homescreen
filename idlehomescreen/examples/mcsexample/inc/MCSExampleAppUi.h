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

#ifndef __MCSEXAMPLEAPPUI_h__
#define __MCSEXAMPLEAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CMCSExampleAppView;

// CLASS DECLARATION
/**
 * CMCSExampleAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CMCSExampleAppUi : public CAknAppUi
    {
public:
    // Constructors and destructor

    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CMCSExampleAppUi.
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CMCSExampleAppUi();

    /**
     * ~CMCSExampleAppUi.
     * Virtual Destructor.
     */
    virtual ~CMCSExampleAppUi();

private:
    // Functions from base classes

    /**
     * From CEikAppUi, HandleCommandL.
     * Takes care of command handling.
     * @param aCommand Command to be handled.
     */
    void HandleCommandL(TInt aCommand);

    /**
     *  HandleStatusPaneSizeChange.
     *  Called by the framework when the application status pane
     *  size is changed.
     */
    void HandleStatusPaneSizeChange();

    /**
     *  From CCoeAppUi, HelpContextL.
     *  Provides help context for the application.
     *  size is changed.
     */
    CArrayFix<TCoeHelpContext>* HelpContextL() const;

private:
    // Data

    /**
     * The application view
     * Owned by CMCSExampleAppUi
     */
    CMCSExampleAppView* iAppView;

    };

#endif // __MCSEXAMPLEAPPUI_h__
// End of File
