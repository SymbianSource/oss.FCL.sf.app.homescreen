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
* Description:  Base interface for component data APIs
*
*/


#ifndef __MXNCOMPONENTINTERFACE_H__
#define __MXNCOMPONENTINTERFACE_H__

//  INCLUDES
#include    <e32base.h>
#include	"xnnodeappif.h"

// CLASS DECLARATION
/**
*  Base interface for component data APIs
*  @lib xnlayoutengine.lib
*  @since Series 60 3.1
*/
namespace XnComponentInterface
    {
    _LIT8(KType, "");
   
    class MXnComponentInterface
		    {
        public:
        static inline const TDesC8& Type()
            {
            return KType;
            }
   			};
		
    template<class T> TBool MakeInterfaceL(T*& aTargetInterface, CXnNodeAppIf& aNode)
        {
        aTargetInterface = static_cast<T*>(aNode.MakeInterfaceL(T::Type()));
        return aTargetInterface ? ETrue : EFalse;
        }
      }
#endif //__MXNCOMPONENTINTERFACE_H__
