/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_MMAPPLICATION_H
#define C_MMAPPLICATION_H

// INCLUDES
#include <aknapp.h>

/**
 *  @ingroup group_matrixmenu
 *  Matrix Menu Application.
 *  AVKON Application class.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CMmApplication) : public CAknApplication
	{
	
public:

// from base class CApaApplication

	/**
     * From CApaApplication.
     * Returns application's UID.
     *
     * @since S60 v5.0
     * @return Application's UID (KUidMatrixMenuApp).
     */
	TUid AppDllUid() const;

protected:

// from base class CApaApplication

	/**
     * From CApaApplication.
     * Creates the Document.
     *
     * @since S60 v5.0
     * @return Instance of the Document.
     */
	CApaDocument* CreateDocumentL();
	
	};

#endif // C_MMAPPLICATION_H

// End of File
