/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application
*
*/


#ifndef C_APPLICATION_H
#define C_APPLICATION_H

#include <aisystemuids.hrh>
#include "xnapplication.h"
#include "ai3.hrh"

#ifdef FF_HOMESCREEN_COMMON_IDLEFW
const TUid KUidXmlUiApp = { AI_UID3_AIFW_EXE };
#else
const TUid KUidXmlUiApp = { AI3_UID_EXE_TEST_APPLICATION };
#endif // FF_HOMESCREEN_COMMON_IDLEFW


namespace AiXmlUiController
{
    
class CXmlUiController;


/**
 *  @ingroup group_xmluicontroller
 * 
 * Application class of XML UI Controller application 
 *
 *  @lib AiXmlUiMain
 */
class CApplication : public CXnApplication
    {
    
public:   // Constructors and destructor

    static CApplication* New(CXmlUiController& aUiCtl);

    virtual ~CApplication();
    
public:  // Functions from base classes

    CApaDocument* CreateDocumentL();

    virtual TUid AppDllUid() const;

    virtual TFileName ResourceFileName() const;

private:

    CApplication(TUid aAppUid, CXmlUiController& aUiCtl);

private:     // Data

    /**
     * UI Controller. Not own
     */
    CXmlUiController&   iUiCtl;

    };

} // namespace AiXmlUiController

#endif  // C_APPLICATION_H

// End of File.
