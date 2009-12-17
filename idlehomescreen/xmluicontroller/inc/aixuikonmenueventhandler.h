/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used for handling XUIKON menu events
*
*/


#ifndef C_MENUEVENTHANDLER_H
#define C_MENUEVENTHANDLER_H

#include <e32base.h>
#include "xnmenu.h"
#include "xnbitmap.h"

class CXnNodeAppIf;
class TXnUiEngineAppIf;
class MAiStrParser;
class MAiFwEventHandler;
class MAiPSPropertyObserver;
class CXnAppUiAdapter;
class CXnDomNode;
class XnMenuInterface::MXnMenuObserver;

enum TAICBA
    {
    EAIMenuOff = 1,
    EAIMenuOn
    };


namespace AiXmlUiController
{
    
/**
 *  @ingroup group_xmluicontroller
 * 
 *  This class is used for handling XUIKON events.
 *
 *  Menu events are handled in here. This class parses
 *  the events and decides what to do with them.
 *
 *  @lib AiXmlUiMain
 */
class CAIXuikonMenuEventHandler : public CBase, public XnMenuInterface::MXnMenuObserver
    {
public:

    static CAIXuikonMenuEventHandler* NewL(TXnUiEngineAppIf& aUiEngine,
                                           MAiFwEventHandler* aFwEventHandler,
                                           CXnAppUiAdapter* aAppUi);
    
    ~CAIXuikonMenuEventHandler();

    TBool HandleEventL( const TDesC8& aEventString8, CXnNodeAppIf* aOrigin = NULL, CXnDomNode* aEvent = NULL );
        	
private:
    CAIXuikonMenuEventHandler(TXnUiEngineAppIf& aUiEngine,
                              MAiFwEventHandler* aFwEventHandler,
                              CXnAppUiAdapter* aAppUi);
                          
    void ConstructL();
    
    static TInt HandleKeylockEvent(TAny* aSelf);
    
    void SetCBALabelsL(TAICBA aCBA, CXnDomNode* aEvent);

    void ShowMenuL(TBool aShow, const TDesC8& aMenuId, CXnDomNode* aEvent);

    void ShowMenuItemL(const TDesC8& aMenuItemId, TBool aShow, CXnDomNode* aEvent );
    
    TBool IsMenuItemVisibleL( const TDesC8& aMenuItemId, CXnDomNode* aEvent );

    void ParseParameterStringL(const TDesC8& aSourcePtr, TPtrC8& aTargetPtr);

    void RecurseChildrenL(TBool aShow, CXnNodeAppIf* aNode);

    void DoMacrosetL(const TDesC8& aMacrosetNodeId, CXnDomNode* aEvent);

    void GetTextFromElementL(HBufC8*& aMemberPtr, const TDesC8& aItemId, CXnDomNode* aEvent);
    
    TBool HandleMacroOperationL(const TDesC8& aSourcePtr, CXnNodeAppIf& aTargetNode, CXnDomNode& aEvent );

    TBool AttemptRestoreSoftkeyIconL(
                            XnMenuInterface::MXnMenuInterface* aControl,
                            XnImageInterface::MXnImageInterface* aIconControl,
                            CXnNodeAppIf* aSourceNode,
                            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );

    void SetSoftkeyLabelL(
                            XnMenuInterface::MXnMenuInterface* aControl,
                            const TDesC8* aLabel,
                            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );
                            
    void SetObserverL( const TDesC8& aParam, CXnDomNode* aEvent );                            
    
    TBool InitEditModeMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused, const TDesC8& aSource );
    
    TBool InitDefaultModeMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused, const TDesC8& aSource );
    
    CXnProperty* GetWidgetPropertyL(CXnNodeAppIf& aPlugin, const TDesC8& aProperty);

private: // from XnMenuInterface::MXnMenuObserver
    TBool DynInitMenuItemL( CXnNodeAppIf& aMenuItem );
    
private:    
    TBool DynInitItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aFocused );

private: // data

    /**
     * Current CBA mode, menu/normal(shortcut).
     */
    TAICBA iCurrentCBA;

    /**
     * Pointer to UI engine IF. Not own.
     */
    TXnUiEngineAppIf* iUiEngine;

    /**
     * String parser utility. Own.
     */
    MAiStrParser* iStrParser;
    
    /**
     * Menu item Open event string.
     * Own.
     */
    HBufC* iActivationEvent;

    /**
     * Menu item settings event string.
     * Own.
     */
    HBufC* iSettingsEvent;
    
    /**
     * Close action id.
     * Own.
     */
    TPtrC8 iCloseActionId;
    
    /**
     * Menu mode softkey labels.
     * Own.
     */
    HBufC8* iMenuModeLskLabel;
    HBufC8* iMenuModeRskLabel;

    /**
     * Normal mode softkey labels.
     * Own.
     */
    HBufC8* iNormalModeLskLabel;
    HBufC8* iNormalModeRskLabel;
    
    /**
     * Normal mode softkey label ids.
     * Own.
     */
    TPtrC8 iNormalModeLskLabelId;
    TPtrC8 iNormalModeRskLabelId;

    /**
     * Normal mode softkey icon ids.
     * Own.
     */
    TPtrC8 iNormalModeLskIconId;
    TPtrC8 iNormalModeRskIconId;

    /**
     * App softkey ids.
     * Own.
     */
    TPtrC8 iLskId;
    TPtrC8 iRskId;

    TPtrC8 iLskMenuModeNode;
    TPtrC8 iRskMenuModeNode;
    /**
     * Plug-in event handler.
     * Doesn't own.
     */
    MAiFwEventHandler* iFwEventHandler;
    
    /**
    * Observers keylock status
    * Owned
    */
    MAiPSPropertyObserver* 	iKeylockObserver;
    
    /**
    * Pointer to Appui for closing the menubar when necessary
    * Not owned
    */
    CXnAppUiAdapter* iAppUi;
    /**
     * For comparing has the bitmap changed. RSK
     * Not owned.
     */
    CFbsBitmap* iMenumodeBitmapRsk;
    /**
     * For comparing has the bitmap changed. LSK
     * Not owned.
     */
    CFbsBitmap* iMenumodeBitmapLsk;
    
    };

} // namespace AiXmlUiController

#endif // C_MENUEVENTHANDLER_H
