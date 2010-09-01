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
* Description:
* Xuikon control adapter inline functions
*
*/

#ifndef XNCONTROLADAPTER_INL
#define XNCONTROLADAPTER_INL

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline RWindow& CXnControlAdapter::Window() const
    {
    return CCoeControl::Window();
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CCoeControl* CXnControlAdapter::GrabbingComponent() const
    {
    return CCoeControl::GrabbingComponent();
    };

#endif
