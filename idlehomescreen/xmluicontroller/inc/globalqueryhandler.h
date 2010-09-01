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
* Description:  Global query active object
*
*/

#ifndef GLOBALQUERYHANDLER_H
#define GLOBALQUERYHANDLER_H

#include <e32base.h>

// FORWARD DECLARATIONS
namespace AiXmlUiController
{
class CXmlUiController;

/**
 * Active object class to handle global query presses.
 */
class CGlobalQueryHandler : public CActive
    {
    
public:
    /**
     * @param Reference to CXmlUiController which SetPluginsOnline function is called when user accepts query
     */
    static CGlobalQueryHandler* NewL(CXmlUiController& aUiController);
    
    /**
     * 
     */
    ~CGlobalQueryHandler();
    
    /**
     * Value of the SetPluginsOnline when user accepts query.
     * Activates this active object
     * @param aOnline value of the function call, ETrue/EFalse.  
     */
    void SetOnlineParamAndActivate(TBool aOnline);
    
private:
    /**
     * @param Reference to CXmlUiController which SetPluginsOnline function is called when user accepts query
     */
    CGlobalQueryHandler(CXmlUiController& aUiController);
    
    /**
     *  
     */
    void ConstructL();
    
private: // from CActive
    /**
     * From CActive
     */
    void RunL();
    
    /**
     * From CActive
     */
    void DoCancel();
    
private: // data
    /**
     * Controller, which is used when query is accepted
     */
    CXmlUiController& iUiController;
    
    /**
     * Value of the CXmlUiController::SetPluginsOnline function call when user accepts query 
     */
    TBool iSetOnline;
    };


}// namespace AiXmlUiController
#endif //CONTENTPUBLISHER_H





