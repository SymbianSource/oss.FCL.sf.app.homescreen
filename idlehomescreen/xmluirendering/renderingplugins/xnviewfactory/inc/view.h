/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for CAknView.
*
*/



#ifndef _VIEWPLUGIN_H
#define _VIEWPLUGIN_H

// System includes
#include <e32base.h>

// User includes
#include "xncomponent.h"


// CLASS DECLARATION   
/**
*  @ingroup group_xnviewfactory
*  @lib xn3viewfactory.dll
*  @since S6060 3.1
*/ 
class CXnViewPlugin : public CXnComponent
   {
public:
	/**
	 * 2 phase construction.
	 */
	static CXnViewPlugin* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CXnViewPlugin();

private:

	CXnViewPlugin();
	void ConstructL();
	
private:
   };

#endif      // _VIEWPLUGIN_H
            
// End of File
