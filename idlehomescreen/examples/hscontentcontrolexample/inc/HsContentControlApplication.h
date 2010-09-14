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
* Description:  Content Control example
*
*/


#ifndef HSCONTENTCONTROLAPPLICATION_H
#define HSCONTENTCONTROLAPPLICATION_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknapp.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Constants]
const TUid KUidHsContentControlApplication = { 0xE88D7143 };
// ]]] end generated region [Generated Constants]

/**
 *
 * @class    CHsContentControlApplication HsContentControlApplication.h
 * @brief    A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CHsContentControlApplication : public CAknApplication
    {
private:
    TUid AppDllUid() const;
    CApaDocument* CreateDocumentL();
    
    };
            
#endif // HSCONTENTCONTROLAPPLICATION_H        
