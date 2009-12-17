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
* Description:  Literal definitions 
*
*/



#ifndef XN_LITERAL_H
#define XN_LITERAL_H

#include "xndomnode.h"
#include <stringtablesupport.h>
// MODULE LITERAL DEFINITIONS
//Core
_STLIT8(KProperty,    "property");

//Ui
_STLIT8(KXmluiml,     "xmluiml");
_STLIT8(KHeader,      "header");
_STLIT8(KInclude,     "include");
_STLIT8(KViews,       "views");
_STLIT8(KView,        "view");
_STLIT8(KPanes,       "panes");
_STLIT8(KStatusPane,  "statuspane");
_STLIT8(KMainPane,    "mainpane");
_STLIT8(KControlPane, "controlpane");
_STLIT8(KDialogs,     "dialogs");
_STLIT8(KDialog,      "dialog");
_STLIT8(KNote,        "note");

//Header
_STLIT8(KUiDefinition,"uidefinition");
_STLIT8(KApplication, "application");
_STLIT8(KDesc,        "desc");

//Control
_STLIT8(KButton,      "button");
_STLIT8(KGrid,        "grid");
_STLIT8(KListItem,    "listitem");
_STLIT8(KDataGrid,    "datagrid");
_STLIT8(KGridCellTemplate,    "gridcelltemplate");
_STLIT8(KList,        "list");
_STLIT8(KDataList,    "datalist");
_STLIT8(KListRowTemplate, "listrowtemplate");
_STLIT8(KMenuBar,     "menubar");
_STLIT8(KMenu,        "menu");
_STLIT8(KMenuItem,    "menuitem");
_STLIT8(KText,        "text");
_STLIT8(KImage,       "image");
_STLIT8(KEditor,      "editor");
_STLIT8(KMarquee,	"marquee");
_STLIT8(KNewsticker, "newsticker");
_STLIT8(KTooltip,	"tooltip");
//XHTML
_STLIT8(KObject,	"object");
_STLIT8(KParam,		"param");

//Box
_STLIT8(KBox,         "box");

//Interaction
_STLIT8(KAction,      "action");
_STLIT8(KTrigger,     "trigger");
_STLIT8(KEvent,       "event");


//PROPERTY LITERAL DEFINITIONS

//Common properties
_STLIT8(KDisplay,          "display");
_STLIT8(KDisplayPriority,  "_s60-display-priority");
_STLIT8(KVisibility,       "visibility");
_STLIT8(KInitialFocus, 	   "_s60-initial-focus");


_STLIT8(KBlockProgression, "block-progression");
_STLIT8(KDirection,        "direction");

_STLIT8(KWidth,            "width");
_STLIT8(KHeight,           "height");

_STLIT8(KMarginTop,        "margin-top");
_STLIT8(KMarginRight,      "margin-right");
_STLIT8(KMarginBottom,     "margin-bottom");
_STLIT8(KMarginLeft,       "margin-left");
_STLIT8(KBorderWidth,      "border-width");
_STLIT8(KBorderTopWidth,   "border-top-width");
_STLIT8(KBorderRightWidth, "border-right-width");
_STLIT8(KBorderBottomWidth,"border-bottom-width");
_STLIT8(KBorderLeftWidth,  "border-left-width");

_STLIT8(KPaddingTop,       "padding-top");
_STLIT8(KPaddingRight,     "padding-right");
_STLIT8(KPaddingBottom,    "padding-bottom");
_STLIT8(KPaddingLeft,      "padding-left");

_STLIT8(KPosition,         "position");
_STLIT8(KTop,              "top");
_STLIT8(KRight,            "right");
_STLIT8(KBottom,           "bottom");
_STLIT8(KLeft,             "left");
_STLIT8(KMinWidth,         "min-width");
_STLIT8(KMaxWidth,         "max-width");
_STLIT8(KMinHeight,        "min-height");
_STLIT8(KMaxHeight,        "max-height");

_STLIT8(KColor,            "color");
_STLIT8(KBGColor,          "background-color");
_STLIT8(KBGImg,            "background-image");
_STLIT8(KBGSize,           "background-size");
_STLIT8(KBGRepeat,         "background-repeat");
_STLIT8(KBGPos,            "background-position");

_STLIT8(KBorderTopStyle,   "border-top-style");
_STLIT8(KBorderRightStyle, "border-right-style");
_STLIT8(KBorderBottomStyle,"border-bottom-style");
_STLIT8(KBorderLeftStyle,  "border-left-style");
_STLIT8(KBorderColor,      "border-color");
_STLIT8(KBorderImg,        "border-image");


//Element with text properties
_STLIT8(KTextOverflowMode, "text-overflow-mode");

_STLIT8(KFontFamily,       "font-family");
_STLIT8(KFontSize,         "font-size");
_STLIT8(KFontWeight,       "font-weight");
_STLIT8(KFontStyle,        "font-style");
_STLIT8(KTextDecoration,   "text-decoration");

//For text,list,datalist, grid, datagrid elements
_STLIT8(KOverflow,          "overflow");

//List And grid properties
_STLIT8(KVisibleRows,      "_s60-visible-list-rows");
_STLIT8(KInitialSelection, "_s60-initial-list-selection");
_STLIT8(KFocusHorLooping,  "_s60-list-focus-horizontal-looping");
_STLIT8(KFocusVerLooping,  "_s60-list-focus-vertical-looping");

//Datagrid & grid properties
_STLIT8(KGridColumns,       "_s60-grid-columns");
_STLIT8(KS60GridOrientation, "_s60-grid-orientation");
_STLIT8(KS60GridVerDirection, "_s60-grid-ver-direction"); //lr, rl
_STLIT8(KS60GridHorDirection, "_s60-grid-hor-direction"); //tb, bt

//Views and view properties
_STLIT8(KViewDecoration,       "_s60-view-decoration");
_STLIT8(KTabStyle,             "_s60-tab-style");
_STLIT8(KTabColor,             "_s60-tab-color");
_STLIT8(KTabImg,               "_s60-tab-image");
_STLIT8(KViewTransitionEffect, "_s60-view-transition-effect");

//View
_STLIT8(KTabLabel,             "_s60-tab-label");

//Function pointer
typedef void (*FuncPtr) (TAny*);
       
//Struct to hold type and function which operates it
typedef struct
    {
    const void* iName;
    FuncPtr   iFuncPtr;
    TContentType iContentType;
    }TXnElementVisitor;
    
//Struct to follow up if property has capability to inherit  
typedef struct
    {
    const void* iName;
    TBool       iInherit;
    }TXnPropertyProxy;


// Common properties intermediate table 
const TXnPropertyProxy KXnCommonPropertyTable[] =
	{
	    {(const void*)&KDisplay,             EFalse},
	    {(const void*)&KDisplayPriority,     EFalse},
	    {(const void*)&KVisibility,          ETrue},
		{(const void*)&KInitialFocus,        EFalse},
	    
	    {(const void*)&KBlockProgression,    ETrue},
	    {(const void*)&KDirection,           ETrue},
	    
	    {(const void*)&KWidth,               EFalse},
	    {(const void*)&KHeight,              EFalse},
	    
	    {(const void*)&KMarginTop,           EFalse},
	    {(const void*)&KMarginRight,         EFalse},
	    {(const void*)&KMarginBottom,        EFalse},
	    {(const void*)&KMarginLeft,          EFalse},
	    {(const void*)&KBorderWidth,         EFalse},
	    {(const void*)&KBorderTopWidth,      EFalse},
	    {(const void*)&KBorderRightWidth,    EFalse},
	    {(const void*)&KBorderBottomWidth,   EFalse},
	    {(const void*)&KBorderLeftWidth,     EFalse},
	    
	    {(const void*)&KPaddingTop,          EFalse},
	    {(const void*)&KPaddingRight,        EFalse},
	    {(const void*)&KPaddingBottom,       EFalse},
	    {(const void*)&KPaddingLeft,         EFalse},
	    
	    {(const void*)&KPosition,            EFalse},
	    {(const void*)&KTop,                 EFalse},
	    {(const void*)&KRight,               EFalse},
	    {(const void*)&KBottom,              EFalse},
	    {(const void*)&KLeft,                EFalse},
	    
	    {(const void*)&KMinWidth,            EFalse},
	    {(const void*)&KMaxWidth,            EFalse},
	    {(const void*)&KMinHeight,           EFalse},
	    {(const void*)&KMaxHeight,           EFalse},
	    
	    //Appearance
	    {(const void*)&KColor,               ETrue},
	    {(const void*)&KBGColor,             EFalse},
	    {(const void*)&KBGImg,               EFalse},
	    {(const void*)&KBGSize,              EFalse},
	    {(const void*)&KBGRepeat,            EFalse},
	    {(const void*)&KBGPos,               EFalse},
	    
	    {(const void*)&KBorderTopStyle,      EFalse},
	    {(const void*)&KBorderRightStyle,    EFalse},
	    {(const void*)&KBorderBottomStyle,   EFalse},
	    {(const void*)&KBorderLeftStyle,     EFalse},
	    {(const void*)&KBorderColor,         EFalse},
	    {(const void*)&KBorderImg,           EFalse}
	    
	};


const TXnPropertyProxy KXnElementWithTextPropertyTable[] =
	{
	    {(const void*)&KTextOverflowMode,   EFalse},
	    {(const void*)&KFontFamily,         ETrue},
	    {(const void*)&KFontSize,           ETrue},
	    {(const void*)&KFontWeight,         ETrue},
	    {(const void*)&KFontStyle,          ETrue},
	    {(const void*)&KTextDecoration,     EFalse}
	};

const TXnPropertyProxy KXnTextElementPropertyTable[] =
	{
	    {(const void*)&KOverflow,       EFalse}
	};
	
const TXnPropertyProxy KXnGridAndListPropertyTable[] =
	{
	    {(const void*)&KVisibleRows,          EFalse},
	    {(const void*)&KInitialSelection,     EFalse},
	    {(const void*)&KFocusHorLooping,      EFalse},
	    {(const void*)&KFocusVerLooping,      EFalse},
	    {(const void*)&KOverflow,             EFalse}
	};

const TXnPropertyProxy KXnDataGridAndGridPropertyTable[] =
	{
	    {(const void*)&KGridColumns,            EFalse},
	    {(const void*)&KS60GridOrientation,     EFalse},
	    {(const void*)&KS60GridVerDirection,    EFalse},
	    {(const void*)&KS60GridHorDirection,    EFalse}
	};
	
const TXnPropertyProxy KXnViewsAndViewPropertyTable[] =
	{
	    {(const void*)&KViewDecoration,          EFalse},
	    {(const void*)&KTabStyle,                ETrue},
	    {(const void*)&KTabColor,                ETrue},
	    {(const void*)&KTabImg,                  EFalse},
	    {(const void*)&KViewTransitionEffect,    EFalse}
	};

const TXnPropertyProxy KXnViewPropertyTable[] =
	{
	    
	    {(const void*)&KTabLabel,   EFalse}
	   
	};				    
// CLASS DECLARATION


#endif      // MXN_DOM_VISITOR_H  
            
// End of File
