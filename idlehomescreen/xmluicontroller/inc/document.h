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
* Description:  Document
*
*/


#ifndef C_DOCUMENT_H
#define C_DOCUMENT_H

#include "xndocument.h"

namespace AiXmlUiController
{
    
class CXmlUiController;

/**
 *  @ingroup group_xmluicontroller
 * 
 * Document class of XML UI Controller application. 
 *
 * @lib AiXmlUiMain
 */
class CDocument : public CXnDocument
    {
    
public:   // Constructors and destructor

    static CDocument* NewL(CEikApplication &aApp, CXmlUiController& aUiCtl);

    virtual ~CDocument();

public:  // Functions from CXnDocument

    CEikAppUi* CreateAppUiL();

private:

    CDocument(CEikApplication &aApp, CXmlUiController& aUiCtl);

private:     // Data

    CXmlUiController& iUiCtl;

    };

} // namespace AiXmlUiController
    
#endif  // C_DOCUMENT_H

// End of File.
