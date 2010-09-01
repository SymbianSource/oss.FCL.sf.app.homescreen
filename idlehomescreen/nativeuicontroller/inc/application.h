/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_APPLICATION_H
#define C_APPLICATION_H

#include <aknapp.h> 

namespace AiNativeUiController
{
    
class CNativeUiController;

/**
 * Application class of Native UI Controller application 
 *
 *  @lib ainatiui
 *  @since S60 3.2
 */
class CApplication : public CAknApplication
    {
public:   
    
// Constructors and destructor

    static CApplication* New(CNativeUiController* aUiCtl);

    virtual ~CApplication();
    

// Functions from base classes

    CApaDocument* CreateDocumentL();

    virtual TUid AppDllUid() const;

    virtual TFileName ResourceFileName() const;

private:

    CApplication(CNativeUiController* aUiCtl);

private:     // Data

    /**
     * UI Controller. Not own
     */
    CNativeUiController*   iUiCtl;

    };

} // namespace AiNativeUiController

#endif  // C_APPLICATION_H

// End of File.
