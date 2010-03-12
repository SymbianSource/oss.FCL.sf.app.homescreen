/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Handles istalling for wm
*
*/

#ifndef __WMINSTALLER_
#define __WMINSTALLER_

#include <SWInstApi.h> //installer

class CIdle;
class CWmWidgetData;
class TUid;

/**
 * CWmInstaller 
 */
NONSHARABLE_CLASS( CWmInstaller ) : public CActive
    {
public:
    /**
     * Two-phased constructors.
     */
    static CWmInstaller* NewL();
    static CWmInstaller* NewLC();
    
    /** Destructor */
    ~CWmInstaller();
    
private:    
    /** constructor */
    CWmInstaller();
    
    /** 2nd phase constructor */
    void ConstructL();
    
protected: // implementation of CActive
    /**
     * Implements cancellation of an outstanding request.
     * 
     * @see CActive::DoCancel
     */
    void DoCancel();
    
    /**
     * Handles an active object's request completion event.
     * 
     * @see CActive::RunL
     */
    void RunL();
    
    /**
     * RunError
     * 
     * @see CActive::RunError
     */
    TInt RunError(TInt aError);

private:
    
    static TInt CloseSwiSession( TAny* aPtr );
    
public:
    /**
     *  Uninstall given widget
     *  
     *  @param aData widget to unistall
     */
    void UninstallL( CWmWidgetData* aData );
    
	/**
	 * Returns wrt widget uid current been unistalled. 
	 * If installer is not active returns KNullUid.
	 */
    TUid UninstallUid();
    
private:
    
    /** 
     * instance of the CIdle class for async swinstaller delete
     */
    CIdle* iIdle;
    
    /**
     * silent installer
     */
    SwiUI::RSWInstSilentLauncher iInstaller;

    /**
     * wrt widget that is currently been uninstalled
     */    
    TUid iUid;
    
    
    HBufC8* iMime;
    };

#endif // __WMPLUGIN_
