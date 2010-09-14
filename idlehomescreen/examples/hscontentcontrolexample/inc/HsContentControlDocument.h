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


#ifndef HSCONTENTCONTROLDOCUMENT_H
#define HSCONTENTCONTROLDOCUMENT_H

#include <akndoc.h>
        
class CEikAppUi;

/**
* @class    CHsContentControlDocument HsContentControlDocument.h
* @brief    A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CHsContentControlDocument : public CAknDocument
    {
public: 
    // constructor
    static CHsContentControlDocument* NewL( CEikApplication& aApp );

private: 
    // constructors
    CHsContentControlDocument( CEikApplication& aApp );
    void ConstructL();
    
public: 
    // from base class CEikDocument
    CEikAppUi* CreateAppUiL();
    };
#endif // HSCONTENTCONTROLDOCUMENT_H
