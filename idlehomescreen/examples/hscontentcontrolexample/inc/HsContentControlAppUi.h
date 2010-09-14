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


#ifndef HSCONTENTCONTROLAPPUI_H
#define HSCONTENTCONTROLAPPUI_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknappui.h>
#include <eikmenub.h>
// ]]] end generated region [Generated Includes]

// [[[ begin generated region: do not modify [Generated Forward Declarations]
class CHsContentControlListBox;
// ]]] end generated region [Generated Forward Declarations]

/**
 * @class    CHsContentControlAppUi HsContentControlAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        container management and the default menu, control pane, and status pane.
 */
class CHsContentControlAppUi : public CAknAppUi
    {
public: 
    // constructor and destructor
    CHsContentControlAppUi();
    void ConstructL();
    virtual ~CHsContentControlAppUi();

public: 
    // from CEikAppUi
    void HandleCommandL( TInt aCommand );
    void HandleResourceChangeL( TInt aType );    
    
private:
    void InitializeContainersL();
    // [[[ begin generated region: do not modify [Generated Methods]
public: 
    // ]]] end generated region [Generated Methods]
    
    // [[[ begin generated region: do not modify [Generated Instance Variables]
private: 
    CHsContentControlListBox* iHsContentControlListBox;
    // ]]] end generated region [Generated Instance Variables]
    
    
    // [[[ begin [Overridden Methods]
protected: 
    // ]]] end [Overridden Methods]
    
    
    // [[[ begin [User Handlers]
protected: 
    // ]]] end [User Handlers]
    
    };

#endif // HSCONTENTCONTROLAPPUI_H            
