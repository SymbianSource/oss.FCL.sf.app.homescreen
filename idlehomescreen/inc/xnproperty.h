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
* Xuikon property header file
*
*/

#ifndef XNPROPERTY_H
#define XNPROPERTY_H

// System includes
#include <e32base.h>

// User includes
#include "xndompropertyvalue.h"

// Forward declarations
class CXnDomProperty;
class CXnDomPropertyValue;
class CXnDomStringPool;

// Constants
namespace XnPropertyNames
    {
    _LIT8(KTrue, "true");
    _LIT8(KFalse, "false");
    _LIT8(KNone, "none");
    _LIT8(KWidget, "widget");
    _LIT8(KPlugin, "plugin");

    namespace common
        {
        _LIT8(KId, "id");
        _LIT8(KClass, "class");
        _LIT8(KPCData, "pcdata");
        _LIT8(KFocusable, "focusable");
        _LIT8(KInitialFocus, "_s60-initial-focus");
        _LIT8(KFocusAppearance, "_s60-focus-appearance");                
        _LIT8(KDisabled, "disabled");
        _LIT8(KLongTap, "_s60-longtap");
        _LIT8(KSwipe, "swipe");
        _LIT8(KSwipeDestination, "_s60-swipe-destination");
        _LIT8(KBackgroundMask, "backgroundmask" );
        _LIT8(KEffectId, "effectid");
        _LIT8(KSizeAware, "sizeaware");
        _LIT8(KVisualisationAware, "visualisationaware");
        }
    namespace tooltip
        {
        _LIT8(KStartDelay, "_s60-start-delay");
        _LIT8(KDisplayTime, "_s60-display-time");
        _LIT8(KPositionHint, "_s60-position-hint");
        namespace positionhint
            {
            _LIT8(KAboveLeft, "aboveleft");
            _LIT8(KAboveRight, "aboveright");
            _LIT8(KBelowLeft, "belowleft");
            _LIT8(KBelowRight, "belowright");
            _LIT8(KRight, "right");
            _LIT8(KLeft, "left");
            }
        }
    namespace popup
        {
        _LIT8(KPopupType, "_s60-popup-type");
        
        namespace popuptype
            {
            _LIT8(KNormal, "normal");
            _LIT8(KPermanent, "permanent");
            }
        }
    namespace view
        {
        _LIT8(KStatusPaneLayout, "_s60-status-pane-layout");
        namespace statuspanelayout
            {
            _LIT8( KNone, "none" );            
            _LIT8( KBasic, "basic" );            
            _LIT8( KBasicFlat, "basic-flat" );            
            _LIT8( KWideScreen, "widescreen" );
            _LIT8( KWideScreenFlat, "widescreen-flat" );            
            _LIT8( KWideScreenFlat3Softkeys, "widescreen-flat-3softkeys" );
            }
        }
    namespace newsticker
        {
        _LIT8(KXnNewsticker, "newsticker");
        _LIT8(KXnScrollAmount, "_s60-scroll-amount");
        _LIT8(KXnScrollDelay, "_s60-scroll-delay");
        _LIT8(KXnStartDelay, "_s60-start-delay");
        _LIT8(KDisplayTime, "_s60-display-time");
        _LIT8(KScrollBehaviour, "_s60-scroll-behaviour");
        _LIT8(KScrollLoop, "_s60-scroll-loop");
        _LIT8(KRestartAfterUpdate, "_s60-restart-after-update");
        namespace scroll_behaviour
            {
            _LIT8(KSlide, "slide");
            _LIT8(KAlternate, "alternate");
            _LIT8(KScrollAlternate, "scroll-alternate");
            }
        }
    namespace title
        {
        _LIT8(KTitleIndex, "titleindex" );
        _LIT8(KTitle, "title" );
        }
    namespace menu
        {
        namespace menutype
            {
            _LIT8(KMenuOptionsNoTaskSwapper, "menuoptionsnotaskswapper");
            }
        _LIT8(KMenuItem, "menuitem" );
        _LIT8(KDynMenuItem, "dynmenuitem" );
        _LIT8(KMenuExtension, "menuextension" );
        _LIT8(KMenuType, "menutype");                
        _LIT8(KEvent, "event");
        _LIT8(KLabel, "label");
        _LIT8(KChecked, "checked");
        _LIT(KMSKDotImage16,"#mskdot");       
        _LIT8(KItemType, "type");
        }
    namespace listquerydialog
        {
        _LIT8(KListQueryDialog, "listquerydialog" );
        _LIT8(KS60Heading, "_s60-heading");
        }        
    namespace softkey
        {
        _LIT8(KNodeName, "softkey");
        _LIT8(KTypeAttribute,"type");
        _LIT8(KModeAttribute,"mode");
        namespace type
            {
            _LIT8(KLeft, "left");
            _LIT8(KRight, "right");
            _LIT8(KMiddle, "middle");
            }
        namespace mode
            {
            _LIT8(KModeCallActive, "mode_callactive");
            _LIT8(KModeCallInactive, "mode_callinactive");
            _LIT8(KModeEdit, "mode_edit");
            }
        }
    namespace image
        {
        _LIT8(KPath, "path");
        _LIT8(KMaskPath, "maskpath");
        _LIT8(KMaskInverted, "maskinverted");
        _LIT8(KS60AspectRatio, "_s60-aspect-ratio");
        _LIT8(KPreserve, "preserve");
        _LIT8(KNone, "none");
        }
    namespace clock
        {
        _LIT8( KS60Format, "_s60-format" );
        namespace format
            {
            _LIT8( KDigital, "digital" );
            _LIT8( KAnalog, "analog" );
            }
        _LIT8( KS60AmPmFontSize, "_s60-ampm-font-size" );  
        _LIT8( KS60DigitalFontSize, "_s60-digital-font-size" );                 
        _LIT8( KS60DateFontSize, "_s60-date-font-size" );
        _LIT8( KS60FontSize, "_s60-font-size" );
        _LIT8( KFace, "face" );
        _LIT8( KFace12, "face12" );
        _LIT8( KFace24, "face24" );
        _LIT8( KDate, "date" );
        _LIT8( KDay, "day" );
        _LIT8( KAmpm, "ampm" );
        }
    namespace volumecontrol
        {
        _LIT8( KMinRange, "minrange" );
        _LIT8( KMaxRange, "maxrange" );
        _LIT8( KSlider, "slider" );
        _LIT8( KIncrease, "increase" );
        _LIT8( KDecrease, "decrease" );
        _LIT8( KMute, "mute" );
        _LIT8( KVolumeLevelAdjustment, "_s60-volume-level-adjustment" ); 
        }
    namespace texteditor
        {
        _LIT8( KEditorMarginLeft, "editor-margin-left" );
        _LIT8( KEditorMarginRight, "editor-margin-right" );
        _LIT8( KEditorMarginTop, "editor-margin-top" );
        _LIT8( KEditorMarginBottom, "editor-margin-bottom" );
        }
    namespace action
        {
        _LIT8(KActions, "actions");
        _LIT8(KAction, "action");
        _LIT8(KProperty, "property");
        _LIT8(KName, "name");
        _LIT8(KValue, "value");
        _LIT8(KTrigger, "trigger");
        _LIT8(KEvent, "event");
        namespace trigger
            {
            _LIT8(KName, "name");
            namespace name 
                {
                _LIT8(KLeft, "left");
                _LIT8(KRight, "right");
                _LIT8(KUp, "up");
                _LIT8(KDown, "down");
                _LIT8(KDownAndUp, "downup");
                _LIT8(KActivate, "activate");
                _LIT8(KHold, "hold");
                _LIT8(KFocus, "focus");
                _LIT8(KPassiveFocus, "passivefocus");
                _LIT8(KGainFocus, "gainfocus");
                _LIT8(KLoseFocus, "losefocus");
                _LIT8(KKeyEvent,     "keyevent");
                _LIT8(KViewActivate, "viewactivate");
                _LIT8(KViewDeactivate, "viewdeactivate");
                _LIT8(KScreenDeviceChange, "screendevicechange");
                _LIT8(KStylus, "stylus");
                _LIT8(KGainVisualisation, "gainvisualisation");
                _LIT8(KLoseVisualisation, "losevisualisation");
                _LIT8(KSwipe, "swipe");
                _LIT8(KSizeChanged, "sizechanged");                
                _LIT8(KWidgetAmount, "widgetamount");
                _LIT8(KTitleToScroll, "titletoscroll");
                _LIT8(KTitleScrolled, "titlescrolled");
                _LIT8(KTitleIndex, "titleindex");
                _LIT8(KCallState, "callstate");
                _LIT8(KEditMode, "editmode" );                

                namespace swipe
                     {
                     _LIT8(KDirection, "direction");
                     namespace direction
                         {
                         _LIT8(KLeft, "left");
                         _LIT8(KRight, "right");
                         }
                     }
                namespace keyevent
                    {
                    _LIT8(KKeyCode, "keycode");
                    _LIT8(KScanCode, "scancode");
                    _LIT8(KModifiers, "modifiers");
                    _LIT8(KEventType, "eventtype");
                    _LIT8(KRepeats, "repeats");
                    }
                namespace orientation
                    {
                    _LIT8(KReason, "reason");
                    namespace reason
                        {
                        _LIT8(KLandscape, "landscape");
                        _LIT8(KPortrait, "portrait");
                        }
                    }
                namespace callstate
                    {
                    _LIT8(KActive, "active");
                    _LIT8(KInActive, "inactive");
                    }
                namespace editmode
                    {
                    _LIT8(KEnter, "enter");
                    _LIT8(KExit, "exit");
                    }                     
                }
            }
        namespace event
            {
            _LIT8(KSystemPrefix, "system/");
            _LIT8(KName, "name");
            _LIT8(KSetFocus, "system/setfocus");
            _LIT8(KSetInitialFocus, "system/setinitialfocus");
            _LIT8(KSetPassiveFocus, "system/setpassivefocus");
            _LIT8(KSetActiveFocus, "system/setactivefocus");
            _LIT8(KResetPassiveFocus, "system/resetpassivefocus");
            _LIT8(KSetEditMode, "system/seteditmode");            
            _LIT8(KResetEditMode, "system/reseteditmode");            
            _LIT8(KClearPassiveFocuses, "system/clearpassivefocuses");
            _LIT8(KRunAddWidgetQuery, "system/runaddwidgetquery");            
            _LIT8(KRemoveFocusedWidget, "system/removefocusedwidget");            
            _LIT8(KRemoveWidget, "system/removewidget");            
            _LIT8(KAddWidget, "system/addwidget");
            _LIT8(KActivateView, "system/activateview");
            _LIT8(KActivateNextView, "system/activatenextview");
            _LIT8(KActivatePreviousView, "system/activatepreviousview");
            _LIT8(KAddView,"system/addview");
            _LIT8(KRemoveView,"system/removeview");
            _LIT8(KActivate, "system/activate");
            _LIT8(KDeactivate, "system/deactivate");
            _LIT8(KSet, "system/set");
            _LIT8(KTryDisplayingMenu, "system/trydisplayingmenu");
            _LIT8(KTryDisplayingListQueryDialog, "system/trydisplayinglistquerydialog");
            _LIT8(KTryDisplayingStylusPopup, "system/trydisplayingstyluspopup");
            _LIT8(KToggleWidgetsVisibilty, "system/togglewidgetsvisibilty");
            _LIT8(KSetWallpaper, "system/setwallpaper");
            _LIT8(KActivateTextEditor, "system/activatetexteditor");
            _LIT8(KDeactivateTextEditor, "system/deactivatetexteditor");
            _LIT8(KActivateSelectedItem, "system/activateselecteditem");
            _LIT8(KDestination, "destination");
            _LIT8(KBroadcast, "broadcast");
            _LIT8(KReportEnterEditMode, "system/reportentereditmode" );
            _LIT8(KReportExitEditMode, "system/reportexiteditmode" );
            namespace systemset
                {
                _LIT8(KId, "id");
                _LIT8(KPseudoClass, "pseudoclass");
                _LIT8(KName, "name");
                _LIT8(KValue, "value");
                _LIT8(KType, "type");
                _LIT8(KUnit, "unit");                
                _LIT8(KClass, "class");
                namespace type
                    {
                    _LIT8(KNumeric, "numeric");
                    _LIT8(KString, "string");
                    _LIT8(KAttribute, "attribute");
                    _LIT8(KRGB, "rgb");
                    _LIT8(KUrl, "url");                    
                    }
                }
            }
        }
    namespace style
        {
        namespace common
            {
            _LIT8(KDisplay, "display");
            _LIT8(KNavIndex, "nav-index");
            _LIT8(KS60NavLooping, "_s60-nav-looping");
            _LIT8(KFocus, "focus");
            _LIT8(KPressedDown, "presseddown");
            _LIT8(KPassiveFocus, "passivefocus");
            _LIT8(KHold, "hold");
            _LIT8(KActive, "active");            
            _LIT8(KEdit, "edit");            
            namespace edit
                {
                _LIT8(KEditable, "editable");
                }
            namespace display
                {
                _LIT8(KBlock, "block");
                _LIT8(KNone, "none");
                }
            _LIT8(KS60DisplayPriority, "_s60-display-priority");
            _LIT8(KVisibility, "visibility");
            namespace visibility
                    {
                    _LIT8(KVisible, "visible");
                    _LIT8(KHidden, "hidden");
                    _LIT8(KBlank, "blank");
                    }
            _LIT8(KBlockProgression, "block-progression");
            namespace block_progression
                    {
                    _LIT8(KTB, "tb");
                    _LIT8(KBT, "bt");
                    _LIT8(KRL, "rl");
                    _LIT8(KLR, "lr");
                    }
            _LIT8(KDirection, "direction");
            namespace direction
                    {
                    _LIT8(KLTR, "ltr");
                    _LIT8(KRTL, "rtl");
                    }
            _LIT8(KWidth, "width");
            _LIT8(KHeight, "height");
            _LIT8(KMarginTop, "margin-top");
            _LIT8(KMarginRight, "margin-right");
            _LIT8(KMarginBottom, "margin-bottom");
            _LIT8(KMarginLeft, "margin-left");
            _LIT8(KBorderWidth, "border-width");
            namespace border_width
                    {
                    _LIT8(KThin, "thin");
                    _LIT8(KMedium, "medium");
                    _LIT8(KThick, "thick");
                    }
            _LIT8(KBorderTopWidth, "border-top-width");
            _LIT8(KBorderRightWidth, "border-right-width");
            _LIT8(KBorderBottomWidth, "border-bottom-width");
            _LIT8(KBorderLeftWidth, "border-left-width");
            _LIT8(KPaddingTop, "padding-top");
            _LIT8(KPaddingRight, "padding-right");
            _LIT8(KPaddingBottom, "padding-bottom");
            _LIT8(KPaddingLeft, "padding-left");
            _LIT8(KPosition, "position");
            namespace position 
                {
                _LIT8(KAbsolute, "absolute");
                _LIT8(KStatic, "static");
                _LIT8(KRelative, "relative");
                _LIT8(KFloating, "floating");
                }
            _LIT8(KTop, "top");
            _LIT8(KRight, "right");
            _LIT8(KBottom, "bottom");
            _LIT8(KLeft, "left");
            _LIT8(KMinWidth, "min-width");
            _LIT8(KMaxWidth, "max-width");
            _LIT8(KMinHeight, "min-height");
            _LIT8(KMaxHeight, "max-height");
            _LIT8(KOverflow, "overflow");
            _LIT8(KZIndex, "z-index");
            _LIT8(KAuto, "auto");
            _LIT8(KAdaptive, "adaptive");
            _LIT8(KAppearance, "appearance");
            namespace s60_focus_looping
                {
                _LIT8(KStop, "stop");
                }
            }
        }//style
    namespace appearance
        {
        namespace common
            {
            //Color and background properties
            _LIT8(KColor,            "color");
            _LIT8(KEffectsColor,     "font-effects-color");
            _LIT8(KBackGroundColor,  "background-color");
            _LIT8(KFocusBackground,  "focus-background");
            _LIT8(KBackGroundImage,  "background-image");
            _LIT8(KBackGroundSize,   "background-size");
            _LIT8(KBackGroundRepeat, "background-repeat");
            _LIT8(KFallBackImage,    "fallback-image");                                
            namespace backgroundrepeat
                {
                _LIT8(KRepeat, "repeat");
                _LIT8(KRepeatX, "repeat-x");
                _LIT8(KRepeatY, "repeat-y");
                _LIT8(KSpace, "space");
                _LIT8(KNoRepeat, "no-repeat");
                }
            _LIT8(KBackGroundPosition, "background-position");
            namespace backgroundposition
                {
                _LIT8(KLeft, "left");
                _LIT8(KRight, "right");
                _LIT8(KTop, "top");
                _LIT8(KBottom, "bottom");
                }
            // Border properties
            _LIT8(KBorderStyle,     "border-style");
            _LIT8(KBorderTopStyle,   "border-top-style");
            _LIT8(KBorderRightStyle, "border-right-style");
            _LIT8(KBorderBottomStyle,"border-bottom-style");
            _LIT8(KBorderLeftStyle,  "border-left-style");
            namespace borderstyle
                {
                _LIT8(KDotted, "dotted");
                _LIT8(KSolid, "solid");
                _LIT8(KGroove, "groove");
                _LIT8(KRidge, "ridge");
                _LIT8(KInset, "inset");
                _LIT8(KOutset, "outset"); 
                }
            _LIT8(KBorderColor,      "border-color");
            _LIT8(KBorderLeftColor,      "border-left-color");
            _LIT8(KBorderRightColor,      "border-right-color");
            _LIT8(KBorderTopColor,      "border-top-color");
            _LIT8(KBorderBottomColor,      "border-bottom-color");
            _LIT8(KBorderImage,      "border-image");
            namespace borderimage
                {
                _LIT8(KStretch, "stretch");
                _LIT8(KRepeat, "repeat");
                _LIT8(KRound, "round");
                }
            //Font properties
            _LIT8(KFontFamily,     "font-family");
            _LIT8(KFontSize,       "font-size");
            _LIT8(KFontWeight,     "font-weight");
            _LIT8(KFontLineSpace,  "font-line-space");
            namespace fontweight
                {
                _LIT8(KBold, "bold");
                }
            _LIT8(KFontStyle,      "font-style");
            namespace fontstyle
                {
                _LIT8(KItalic, "italic");
                }
            _LIT8(KTextDecoration, "text-decoration");
            namespace textdecoration
                {
                _LIT8(KUnderline, "underline");
                _LIT8(KLinethrough, "line-through");
                }
            _LIT8(KTextAlign, "text-align");
            namespace textalign
                {
                _LIT8( KAlignLeft,"left");
                _LIT8( KAlignRight,"right");
                _LIT8( KAlignCenter,"center");
                _LIT8( KAlignJustify,"justify");
                }
            _LIT8(KS60TextVAlign, "_s60-text-valign");
            namespace textvalign
                {
                _LIT8( KAlignTop, "top");
                _LIT8( KAlignCenter, "center");
                _LIT8( KAlignBottom, "bottom");                               
                }                               
            _LIT8(KTextEffects, "font-effects");
            namespace fonteffects
                {
                _LIT8( KEffectsAlgorithmicbold, "algorithmicbold");
                _LIT8( KEffectsDropshadow, "dropshadow");
                _LIT8( KEffectsOutline, "outline");
                _LIT8( KEffectsEmbossed, "embossed");
                _LIT8( KEffectsEngraved, "engraved");
                _LIT8( KEffectsSoftedge, "softedge");
                }
            _LIT8(KTextOverflowMode, "text-overflow-mode");
            namespace textoverflow
                {
                _LIT8(KClipMode, "clip");
                _LIT8(KWrapMode,"wrap");
                _LIT8(KMaxLineAmount,"max-line-amount");
                }
            _LIT8(KWhiteSpaceCollapse, "white-space-collapse");
            namespace whitespace
                {
                _LIT8(KCollapse, "collapse");
                _LIT8(KPreserveBreaks, "preserve-breaks");
                _LIT8(KDiscard, "discard");
                }
            //View properties missing
            }//common
        }
    }

// Class declaration
/**
 * Xuikon property class implementation.
 *
 * @ingroup group_xnlayoutengine
 * @lib ?library
 * @since Series 60 3.1
 */
class CXnProperty : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aProperty DOM property to embed
     */
    IMPORT_C static CXnProperty* NewL( CXnDomProperty* aProperty );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aStringPool String pool to use
     */
    IMPORT_C static CXnProperty* NewL(
        const TDesC8& aName,
        CXnDomPropertyValue* aPropertyValue,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aValueType Property value type
     * @param aStringPool String pool to use
     */
    IMPORT_C static CXnProperty* NewL(
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue,
        CXnDomPropertyValue::TPrimitiveValueType aValueType,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aValueType Property value type
     * @param aStringPool String pool to use
     */
    IMPORT_C static CXnProperty* NewL(
        const TDesC8& aPropertyName,
        TReal aPropertyValue,
        CXnDomPropertyValue::TPrimitiveValueType aValueType,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor. Does not take ownership of the DOM property
     *
     * @param aProperty DOM property to embed
     */
    static CXnProperty* NewSharedL( CXnDomProperty* aProperty );

    /**
     * Destructor.
     */
    virtual ~CXnProperty();

public:
    // new functions

    /**
     * Compares two propertys
     *
     * @since Series 60 3.2
     * @return ETrue if properties equals, EFalse otherwise
     */
    IMPORT_C TBool EqualsL( CXnProperty& aProperty ) const;

    /**
     * Get the embedded DOM property
     *
     * @since Series 60 3.1
     * @return Embedded DOM property
     */
    IMPORT_C CXnDomProperty* Property();

    /**
     * Get the value of the property as a 16-bit descriptor. Ownership is
     * transferred to caller.
     *
     * @since Series 60 3.1
     * @return String value of the property
     */
    IMPORT_C HBufC* StringValueL();

    /**
     * Get the value of the property as an 8-bit descriptor.
     *
     * @since Series 60 3.1
     * @return String value of the property
     */
    IMPORT_C const TDesC8& StringValue();

    /**
     * Get the value of the property as a number
     *
     * @since Series 60 3.1
     * @return Numeric value of the property
     */
    IMPORT_C TReal FloatValueL();

    /**
     * Clone the property
     *
     * @since Series 60 3.1
     * @return Clone of the property
     */
    IMPORT_C CXnProperty* CloneL();

private:
    /**
     * C++ default constructor.
     */
    CXnProperty();

    /**
     * Two-phased constructor.
     *
     * @param aProperty DOM property to embed
     */
    void ConstructL( CXnDomProperty* aProperty );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aStringPool String pool to use
     */
    void ConstructL(
        const TDesC8& aName,
        CXnDomPropertyValue* aPropertyValue,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aValueType Property value type
     * @param aStringPool String pool to use
     */
    void ConstructL(
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue,
        CXnDomPropertyValue::TPrimitiveValueType aValueType,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor.
     *
     * @param aName Property name
     * @param aPropertyValue Property value
     * @param aValueType Property value type
     * @param aStringPool String pool to use
     */
    void ConstructL(
        const TDesC8& aPropertyName,
        TReal aPropertyValue,
        CXnDomPropertyValue::TPrimitiveValueType aValueType,
        CXnDomStringPool& aStringPool );

    /**
     * Two-phased constructor. Does not take ownership of the DOM property
     *
     * @param aProperty DOM property to embed
     */
    void ConstructSharedL( CXnDomProperty* aProperty );

private:
    // Data

    /**
     * embedded DOM property.
     * Own or Not own, depending on the iIsDomPropertyOwned flag
     */
    CXnDomProperty* iProperty;

    /**
     * flag indicating whether the DOM property is owned by the
     * CXnProperty object
     */
    TBool iIsDomPropertyOwned;
    };

#endif
