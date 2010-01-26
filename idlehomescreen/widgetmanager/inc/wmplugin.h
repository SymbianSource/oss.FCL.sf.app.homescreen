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
* widget manager plugin declaration
*
*/

#ifndef __WMPLUGIN_
#define __WMPLUGIN_

//  INCLUDES
#include <e32base.h>
#include <vwsdef.h>

#include <hscontentcontrolui.h>
#include <hscontentcontroller.h>
#include <hscontentinfo.h>

class CAknViewAppUi;
class CWmResourceLoader;
class CWmMainContainer;
class CWmEffectManager;

/**
 * CWmPlugin 
 */
NONSHARABLE_CLASS( CWmPlugin ) : public CHsContentControlUi
    {
public:

    /**
     * Two-phased constructor.
     */
    static CWmPlugin* NewL();
    
    /** Destructor */
    ~CWmPlugin();

public: // From MHsContentControlUi

    /** 
     * shows the widget manager UI 
     * 
     * @see MHsContentControlUi::Activate
     */
    void Activate();
    
    /** 
     * Notifies widget list changes 
     * 
     * @see MHsContentControlUi::NotifyWidgetListChanged
     */
    void NotifyWidgetListChanged();
        
public: // plugin services

    /** 
     * whether widget manager UI is showing 
     * 
     * @return return ETrue if wm UI is showed 
     */
    TBool IsActive();

    /** 
     * hides widget manager UI 
     */
    void Deactivate();

    /** 
     * main view activated. Sets the previous view ID and main container 
     *
     * @param aPreviousViewId
     * @param aWmMainContainer 
     */
    void MainViewActivated( 
            const TVwsViewId& aPreviousViewId,
            CWmMainContainer* aWmMainContainer );

    /** 
     * main view was deactivated 
     */
    void MainViewDeactivated();

    /** Postponed command to execute when widgetmanager has deactivated */
    enum TCommand
        {
        /** no command */
        ENone,
        /** add content to home screen */
        EAddToHomescreen
        };
    
    /**
     * Sets a postponed command. The postponed command will be executed after
     * widget manager has been deactivated.
     * 
     * @param aCommand the command to be executed
     * @param aContent content parameter related to the command
     */
    void SetPostponedCommandL(
            TCommand aCommand, CHsContentInfo& aContent );
    
    /** 
     * the CAknViewAppUi reference
     * 
     * @return returns CAknViewAppUi 
     */
    CAknViewAppUi& ViewAppUi();

    /** 
     * the resource loader 
     * 
     * @return returns CWmResourceLoader
     */
    CWmResourceLoader& ResourceLoader();
    
    /** 
     * the content controller ref
     * 
     * @return returns MHsContentController
     */
    MHsContentController& ContentController();

    /**
     * the shared file server reference
     */
    RFs& FileServer();
    
    /** prestored action to be executed. */
    void ExecuteCommandL();

private:
    /** constructor */
    CWmPlugin();
    /** 2nd phase constructor */
    void ConstructL();
    
    /**
     * Sets forwarding numeric keys to phone.
     * 
     * @param aEnabled ETrue if numeric keys are forwarded to phone
     */
    void ForwardNumericKeysToPhone( TBool aEnabled );

private: // data members

    /**
     * the host application view app UI (not owned)
     */
    CAknViewAppUi*  iViewAppUi;

    /**
     * main container (not owned)
     */
    CWmMainContainer*  iWmMainContainer;


    /**
     * resource loader
     */
    CWmResourceLoader* iResourceLoader;

    /**
     * previous view ID (shown when main view closed)
     */
    TVwsViewId iPreviousViewUid;
	
    /**
     * Takes care of fancy UI effects
     */    
    CWmEffectManager* iEffectManager;
    
    /**
     * The file server session
     */
    RFs* iFs;

    /**
     * command to be executed when widgetmanager has
     * been deactivated
     */
    TCommand iPostponedCommand;
    
    /**
     * content info as parameter to iPostponedCommand
     */
    CHsContentInfo* iPostponedContent;
    
    /**
     * ActiveSchedulerWait used to wait while previous view is
	 * being activated upon deletion.
     */
    CActiveSchedulerWait* iWait;
    };

#endif // __WMPLUGIN_

