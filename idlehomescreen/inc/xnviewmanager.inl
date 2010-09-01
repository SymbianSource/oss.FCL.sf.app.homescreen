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
* Viewmanager inline functions
*
*/

#ifndef XNVIEWMANAGER_INL
#define XNVIEWMANAGER_INL

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void CXnViewManager::SetUiEngine( CXnUiEngine& aUiEngine )
    {
    iUiEngine = &aUiEngine;
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CXnUiEngine& CXnViewManager::UiEngine() const
    {
    return *iUiEngine;
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CXnComposer& CXnViewManager::Composer() const
    {
    return *iComposer;
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CXnEditor& CXnViewManager::Editor() const
    {
    return *iEditor;
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline CXnAppUiAdapter& CXnViewManager::AppUiAdapter() const
    {
    return iAppUiAdapter;
    };

#endif
