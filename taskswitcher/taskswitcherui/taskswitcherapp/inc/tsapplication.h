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
* Description:  Application class
 *
*/


#ifndef TSAPPLICATION_H
#define TSAPPLICATION_H

#include <e32base.h> 
#include <aknapp.h>

class CTsApplication : public CAknApplication
    {
public:
    // from CAknApplication
    /**
     * Returns the application DLL UID value
     *
     * @since S60 v5.0
     * @return the UID of this Application/Dll
     */
    TUid AppDllUid( ) const;

protected:
    // from CAknApplication
    /**
     * Create a CApaDocument object and return a pointer to it
     *
     * @since S60 v5.0
     * a pointer to the created document
     */
    CApaDocument* CreateDocumentL( );
    };

#endif // TSAPPLICATION_H
