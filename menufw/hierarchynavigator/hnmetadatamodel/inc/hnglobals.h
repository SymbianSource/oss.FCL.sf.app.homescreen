/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef C_HNGLOBALS_H
#define C_HNGLOBALS_H

#include <f32file.h>

/**
 * Default size for svg icons.
 */
const TInt KDefaultSvgIconSize = 60;

/**
 * Maximum size for icons loaded from files containing
 * raster graphics, such as png, jpg. This does not
 * apply to MIF and MBM files.
 */
const TInt KMaxIconBitmapSize = 240;

/**
 * Maximun integer character length
 */
const TInt KMaxLength = 12;

/**
 * Single character length.
 */
const TInt KSingleChar( 1 );

/**
 * Default datagranularity.
 */
const TInt KDefaultGranularity( 8 );

/**
 * Max digit number.
 */
const TInt KMaxDigitNumber( 10 );

/**
 * A number of cached suites.
 */
const TInt KXmlModelCacheMaxLength( 10 );

/**
 * Used to check if root suite loaded for display.
 */
const TInt KModelCountForRoot( 1 );

/**
 * KErrNoMemory code from MenuService
 */
const TInt KSErrNoMemory = 1007;
/**
 * Logical relations namespace
 */
namespace HnLogicalRelations
    {
    /**
     * Greater.
     */
    _LIT8( KLogicalGreater8,        ">"  );

    /**
     * Less.
     */
    _LIT8( KLogicalLess8,           "<"  );

    /**
     * Equals.
     */
    _LIT8( KLogicalEqual8,          "="  );

    /**
     * Different.
     */
    _LIT8( KLogicalDifferent8,      "!="  );

    /**
     * And.
     */
    _LIT8( KLogicalAnd8,            "&"  );

    /**
     * Or.
     */
    _LIT8( KLogicalOr8,             "|"  );

    /**
     * Nas.
     */
    _LIT8( KLogicalHas8,            "HAS" );

    /**
     * Has not.
     */
    _LIT8( KLogicalHasNot8,         "HASNOT" );

    /**
     * Uid param for uri.
     */
    _LIT8( KParamUid8,              "&uid=" );
    }

/**
 * Event namespace
 */
namespace HnEvent
    {
    /**
     * Event data.
     */
    _LIT8( KEventArgNamespace8,     "event_data");

    /**
     * Dropped before custom id.
     */
    _LIT8( KDroppedBefore8,         "dropped_before_custom_id" );

    /**
     * Dragged custom id.
     */
    _LIT8( KDragged8,               "dragged_custom_id" );

    /**
     * Highlight offset.
     */
    _LIT8( KHighlightOffset8,       "highlight_offset" );

    }

//paths
_LIT( KFolderSuitePath, "Z:\\private\\101F4CD2\\import\\suites\\foldersuite\\suite.xml" );
_LIT( KFolderItemsPath, "Z:\\private\\101F4CD2\\import\\suites\\foldersuite\\items.xml" );

//XML tags
_LIT8( KMm8,                        "mm" );
_LIT8( KContentElementName8,        "content" );
_LIT8( KSuiteElementName8,          "suite" );
_LIT8( KItemElementName8,           "item" );
_LIT8( KActionElementName8,         "action" );
_LIT8( KEventElementName8,          "event" );
_LIT8( KMenuItemElementName8,       "menuitem" );
_LIT8( KMenuItemSpecificElementName8,       "menuitem_specific" );
_LIT8( KMenuItemActionElementName8,       "menuitem_action" );
_LIT8( KLocalizationElementName8,   "localization" );
_LIT8( KNotifyRequestElementName8,  "notify_request" );
_LIT8( KMenuButton8,				"button" );
_LIT8( KMiddleSoftKey8,             "msk" );
_LIT8( KMenuToolbar8,				"toolbar" );

_LIT8 ( KName8,                     "name" );

_LIT8( KQueryElementItem8,          "query" );
_LIT8( KConstructorElementItem8 ,   "constructor" );
_LIT8( KCommandElementItem8 ,       "command" );
_LIT8( KOutputElementItem8 ,        "output" );
_LIT8( KImageElementItem8,          "image" );
_LIT8( KTextElementItem8,           "text" );
_LIT8( KIdAttrName8,                "id" );
_LIT8( KFolderAttrName8,            "folder" );


_LIT8( KCommandModeAsync8,          "async" );
_LIT8( KCommandNameAttr8,           "name" );
_LIT8( KCommandModeAttr8,           "mode" );

_LIT8( KContentTypeElementItem8,    "contenttype" );
_LIT8( KFilterElementItem8,         "filter" );
_LIT8( KKeyElementItem8,            "key" );

_LIT8( 	KKeyAttrName8,              "key" );
_LIT8( 	KTypeAttrName8,             "type" );
_LIT8( 	KValueAttrName8,            "value" );
_LIT8( 	KEventAttrName8,            "event" );
_LIT8( 	KPositionAttrName8,         "position" );
_LIT8( 	KMenuConditionAttrName8,    "condition" );
_LIT8(  KIndexAttrName8, 			"index" );
_LIT8( 	KMenuDimmAttrName8,    	    "dimm" );
_LIT8( 	KGenreAttrName8,            "genre" );
_LIT8( 	KTitleAttrName8,            "title" );
_LIT8(  KEmptyTextAttrName8,        "emptytext" );
_LIT8( 	KNameAttrName8,             "name" );
_LIT8(  KWidgetAttrName8,           "widget" );
_LIT8( 	KCountAttrName8,            "count" );
_LIT8( 	KTemplateAttrName8,         "template" );
_LIT8(  KTemplateEmpty8,            "empty" );
_LIT8( 	KWidgetTypeAttrName8,       "type" );
_LIT8( 	KItemIdAttrName8,           "id" );
_LIT8( 	KNameSpaceAttrName8,        "namespace" );
_LIT8( 	KServiceAttrName8,          "service" );
_LIT8( 	KAliasAttrName8,            "alias" );
_LIT8(  KParams8,                   "params");

_LIT8( KEditModeItem8,              "edit_mode");
_LIT8( KInterfaceAttrName8,         "interface" );

_LIT8 ( KKeyTypeString,             "string" );
_LIT8 ( KKeyTypeString8,            "string8" );
_LIT8 ( KKeyTypeString16,           "string16" );
_LIT8 ( KKeyTypeInteger,            "integer" );
_LIT8 ( KKeyTypeInteger64,          "integer64" );
_LIT8 ( KKeyTypeUInteger,           "uinteger" );
_LIT8 ( KKeyTypeUid    ,            "uid" );
_LIT8 ( KKeyTypeBoolean,            "boolean" );
_LIT8 ( KKeyTypeMap,                "map" );
_LIT8 ( KKeyTypeList,               "list" );
_LIT8 ( KIdsList,                   "item_ids" );

_LIT8 ( KAction,                    "action");
_LIT8 ( KActionRemove,              "remove");
_LIT8 ( KIdParam,	  				"id" );
_LIT8 ( KMenuApplication8,          "menu:application" );
_LIT  ( KMenuApplication,           "menu:application" );
_LIT8 ( KMenuParentFolder8,         "custom:parent_folder" );
_LIT  ( KMenuParentFolder,          "custom:parent_folder" );
_LIT8 ( KMenuFolder8,               "menu:folder" );
_LIT  ( KMenuFolder,                "menu:folder" );
_LIT8 ( KMenuSuite8,                "menu:suite" );
_LIT  ( KMenuSuite,                 "menu:suite" );
_LIT8 ( KId8,                       "id" );
_LIT8 ( KUid8,                      "uid" );
_LIT8 ( KMcsTitleName8,             "title_name" );
_LIT8 ( KMcsAppGroupName8,          "applicationgroup_name" );
_LIT8 ( KMcsShortName8,             "short_name" );
_LIT8 ( KMcsLongName8,              "long_name" );
_LIT8 ( KMatrixMenuData,  			"matrixmenudata" );
_LIT8 ( KServiceContentName,  		"ContentName" );
_LIT8 ( KType8,  		            "type" );
_LIT8( KIdPath8, 					"ReturnValue:[$index]/id" );
_LIT8( KTypePath8, 					"ReturnValue:[$index]/type" );
_LIT8( KTitleNamePath8, 			"ReturnValue:[$index]/title_name" );
_LIT8( KAppGroupNamePath8,          "ReturnValue:[$index]/applicationgroup_name" );
_LIT8( KLongNamePath8, 		    	"ReturnValue:[$index]/long_name" );
_LIT8( KShortNamePath8, 			"ReturnValue:[$index]/short_name" );
_LIT8( KDeleteLockedPath8, 			"ReturnValue:[$index]/delete_locked" );
_LIT8( KParentIdPath8,                  "ReturnValue:[$index]/parent_id" );
_LIT8( KChildrenCountPath8, 			"ReturnValue/[$index]/children_count" );
_LIT8( KMcsTemplateTypePath8, 			"ReturnValue/[$index]/template" );
_LIT8( KMcsWidgetTypePath8, 			"ReturnValue/[$index]/widget_type" );

_LIT8( KWidgetTypePath8,            "params:widget_type" );
_LIT8( KTemplatePath8,              "params:template" );
_LIT( KTemplatePath,                "params:template" );
_LIT( KWidgetTypePath,              "params:widget_type" );
_LIT8( KReturnValue8, 				"ReturnValue" );
_LIT8( KErrorCode8, 				"ErrorCode" );
_LIT8( KParentFolderId8,            "parent_folder_id" );
_LIT8( KTmpParentFolderId8,         "tmp_parent_folder_id" );

_LIT( KCommandDeleteDialog,         "MMDeleteDialogs" );

_LIT8 ( KSourceAttrName8,           "source" );

_LIT(	KColon,                     ":" );
_LIT8(  KColon8,                    ":" );
_LIT(   KBslash,                    "\\" );
_LIT(   KSlash,                     "/" );
_LIT8(  KSlash8,                    "/" );
_LIT(   KOpenBracket,               "(" );
_LIT(   KCloseBracket,              ")" );
_LIT8(  KOpenSquareBracket,         "[" );
_LIT8(  KCloseSquareBracket,        "]" );
_LIT8(  KHexPrefix8,                "0x" );
_LIT(   KHexPrefix16,               "0x" );
_LIT(   KPipe,                      "|" );
_LIT(	KBlank,						"");

_LIT(	KResourceSeparator,         " = " );
_LIT(	KComma,                     "," );
_LIT8(	KComma8,                     "," );

_LIT(	KRsg,                       ".rsg");
_LIT(	KMbg,                       ".mbg");
_LIT(	KRscPath,                   ":\\resource\\apps\\");

_LIT(   KPrefMm,                    "mm://" );
_LIT8(  KRootWithPref,              "mm://root" );
_LIT8(  KSetFocusWithPref,              "mm://!setfocus" );
_LIT8(   KCRepTimeFormat, "%S,%04d%02d%02d:%02d%02d%02d.%06d" );

// images
_LIT8(  KSkinIdMajor8,              "skinid_major" );
_LIT8(  KSkinIdMinor8,              "skinid_minor" );
_LIT8(  KBitmapId8,                 "bitmap_id" );
_LIT8(  KMaskId8,                   "mask_id" );
_LIT8(  KAppUid8,                   "application_uid" );
_LIT8(  KBitmap8,                   "bitmap" );
_LIT8(  KMask8,                     "mask" );
_LIT8(  KMifFile8,                  "mif_file" );

_LIT(  KSvgFileExtension,         ".svg" );

// texts
_LIT8( KResourceAttrName8,          "resource");
_LIT8( KFile8,                      "file" );

// services

_LIT8( 	KServiceMultimediaMenu,     "MultimediaMenu");
_LIT8( 	KServiceOpenSuite,          "OpenSuite");
_LIT8( 	KServiceStartEditMode,      "StartEditMode");
_LIT8(  KServiceStopEditMode,       "StopEditMode");
_LIT8(  KServiceSwitchWidget,       "SwitchWidget");
_LIT8( 	KServiceBack,               "Back");
_LIT8( 	KSetFocus,                  "SetFocus");
_LIT8(  KMoveFocusBeforeDelete,     "MoveFocusBeforeDelete");
_LIT8( 	KRefreshIcons,              "RefreshIcons" );
_LIT8(  KEvaluateMdModel,           "EvaluateMdModel" );
_LIT8(  KDisableActionsForItem,     "DisableActionsForItem" );

_LIT( 	KSuiteName,                 "suite_name" );
_LIT8(  KSuiteName8, 				"suite_name" );

//
_LIT8 ( KHnRequest, "KHnRequest" );
// widget types
_LIT8 ( KSuite8,                    "suite" );
_LIT  ( KFolderSuite,               "foldersuite" );
_LIT8 ( KFolderSuite8,              "foldersuite" );
_LIT8 ( KWidgetType8,               "type" );
_LIT8 ( KAllowedTypes8,             "allowed_types" );
_LIT8 ( KWidgetTypeList8,           "list" );
_LIT8 ( KWidgetTypeCoverFlow8,      "coverflow" );
_LIT8 ( KWidgetTypeGrid8,           "grid" );

_LIT( KWidgetTypeList,              "list" );
_LIT( KWidgetTypeCoverFlow,         "coverflow" );
_LIT( KWidgetTypeGrid,              "grid" );

_LIT(  KStringFalse,                "FALSE" );
_LIT(  KStringTrue,                 "TRUE" );

_LIT8(  KStringFalse8,              "FALSE" );
_LIT8(  KStringTrue8,               "TRUE" );

_LIT(	KSuitesDir,					"import\\suites\\" );
_LIT(	KXmlExt, 					".xml" );
_LIT(  	Kbackslash, 				"\\" );

_LIT(	KSuiteDefFileName, 			"suite.xml" );
_LIT( 	KTitle, 					"mul_title" );
_LIT8( 	KTitle8, 					"mul_title" );
_LIT8(  KTemplate8,                 "mul_template" );

_LIT8(	KTitleName8, 				"suite" );
_LIT8(	KItemName8, 				"item" );
_LIT8(	KGenreName8, 				"genre" );
_LIT8(	KLocalizationName8, 		"localization" );

_LIT8(	KEventName8, 				"event" );
_LIT8(	KMenuItemName8, 			"menuitem" );
_LIT(	KMenuItemName,	 			"menuitem" );
_LIT8(	KMenuItemSpecificName8, 			"menuitem_specific" );
_LIT(	KMenuItemSpecificName,	 			"menuitem_specific" );

_LIT( 	KRoot, 						"root" );
_LIT8(   KRoot8,                    "root" );
_LIT( 	KExtension, 				".r" ); // rsc, r01, r02, etc.
_LIT( 	KExtensionRsc, 				".rsc" );


_LIT8(	KHexStart8, 				"0x");
_LIT( 	KDrive ,					"C:");
_LIT( 	KEntriesSuffix, 			"suites\\");

_LIT8(  KMoveLocked8,               "move_locked" );
_LIT8(  KRemoveLocked8,             "remove_locked" );
_LIT8(  KRunning8,                  "running" );
_LIT8(  KDrmProtection8,            "drm_protection" );
_LIT8(  KItemType8,                 "type" );
_LIT8(  KCustomId8,                 "custom_id" );
_LIT8(  KItemCustomId8,             "item_custom_id" );
_LIT8(  KIndexShift8,               "index_shift" );
_LIT8(  KSuiteCustomId8,            "suite_custom_id" );
_LIT8(  KItemTypeUnknown8,          "unknown" );
_LIT8(  KItemTypeFolder8,           "folder" );
_LIT8(  KItemUid8,                  "uid" );

_LIT8(  KMenuDrmUnknown8,           "unknown" );
_LIT(   KMenuDrmRightsExpired,      "expired" );


//From hnmodel.h
// model events
_LIT( 	KNewSuiteLoadedMdEvent, 	"new_suite_loaded");
_LIT( 	KOpenSuiteMdEvent,      	"open_suite" );
_LIT(   KReevaluateMdEvent,         "reevaluate_model" );
_LIT(   KSwitchWidgetMdEvent,       "switch_widget" );
_LIT(   KStartEditModeMdEvent,      "start_edit_mode" );
_LIT(   KStopEditModeMdEvent,       "stop_edit_mode" );
_LIT(   KAppGainForeground,         "gain_foreground" );
_LIT(   KAppGainBackground,         "gain_background" );
_LIT(   KSetFocusEvent,             "set_focus" );
_LIT(   KRefreshUiMdEvent,          "refresh_ui" );
_LIT(   KRefreshToolbarMdEvent,     "refresh_toolbar" );
_LIT( 	KBackMdEvent,           	"back" );
_LIT(   KResetToRoot,               "reset_to_root");

// model event-related params

_LIT8( KParentId8,                  "parent_id" );
_LIT8( KDefaultParentId8,           "1" );
_LIT( KDefaultParentId,             "1" );
_LIT8( KDefaultCount8,              "1" );

_LIT(  KMatrixPanic, "Matrix Error" );

// debug strings
#ifdef _DEBUG

_LIT( 	KEventMapLogFile, 			"c:\\logs\\menu\\event_ids.txt" );
_LIT( 	KSuiteLoadingLogFile, 		"c:\\logs\\menu" );

#endif// _DEBUG

// info notes for notifying lack of configuration files
_LIT( KDebugNoteDirNotFound,
  "Directory containing suite configuration not found!" );
_LIT( KDebugNoteFileNotFound,
  "File containing suite configuration not found!" );
_LIT( KDebugNoteOtherError,
  "Loading suite configuration returned error: %d" );

_LIT8( KStar8, "*" );


_LIT( 	KKeyEmpty,                  "key:empty" );
_LIT( 	KKeySelect,                 "key:select" );
_LIT( 	KKeySelect2,                "key:select2" );
_LIT( 	KKeySelect3,                "key:select3" );
_LIT( 	KKeyCall,                   "key:call" );
_LIT( 	KKeyClear,                  "key:clear" );
_LIT(   KKeyMove,                   "key:move" );
_LIT(   KKeyMoveInto,               "key:move_into" );
_LIT( 	KOnSuiteLoad,               "suite:load" );
_LIT(   KOnSuiteUnLoad,             "suite:unload" );
_LIT( 	KOnFocusGain,               "item:focus" );
_LIT(   KOnFocusLost,               "item:unfocus" );

_LIT8(  KIndexWithBrackets8,        "[$index]");
_LIT8(  KIndex8,                    "$index" );
_LIT(   KIndex,                     "$index" );
_LIT8(  KCount,                     "/[$count]" );


/** EActiveSpaceActionKeyEmpty */
const TInt KKeyIdEmpty   		=    0;

/** EActiveSpaceActionKeySelect */
const TInt KKeyIdSelect  		=    1;

/** EActiveSpaceActionKeySelect2 */
const TInt KKeyIdSelect2 		=    2;

/** EActiveSpaceActionKeySelect3 */
const TInt KKeyIdSelect3 		=    3;

/** EActiveSpaceActionKeyCall */
const TInt KKeyIdCall    		=    4;

const TInt KKeyIdClear          =    6;

const TInt KIdSuiteLoad         =    7;

const TInt KIdSuiteUnLoad       =    8;

const TInt KIdFocusGain         =    9;

const TInt KIdFocusLost         =   10;

const TInt KKeyIdMove           =   11;

const TInt KKeyIdMoveInto       =   12;

const TInt KKeyIdMoveHighlight  =   13;


const TInt KCustomKeyOffset     =   7000;

// for XML model provider

const TInt KHnDefaultGranularity = 5;

/** Central Repository Uid for Matrix Menu */
static const TUid KMatrixRepositoryUid = { 0x20012474 };

/**
 * Following characters are not allowed in the names
 */
const TUint KNotAllowedChars[] = {'<', '>', '\\', '/', '"', '|', ':', '*', '?'};

/** Central Repository Uid for Appshell (needed to detect folder change) */
static const TUid KCRUidMenu = {0x101f8847};

const TInt KRootId = 1 ;

const TUint32 KMenuShowFolder = 0x0000000c;

const TInt KTimeStampBufferLength = 25;

const TInt KUidStringLength = 10;

const TInt KTimeStampCutOff = 5;

/**
 * Order in which drives are searched for suites.
 */
const TInt KDriveSearchOrder[] = { EDriveY, EDriveX, EDriveW, EDriveV, EDriveU,
    EDriveT, EDriveS, EDriveR, EDriveQ, EDriveP, EDriveO, EDriveN, EDriveM,
    EDriveL, EDriveK, EDriveJ, EDriveI, EDriveH, EDriveG, EDriveF, EDriveE,
    EDriveD, EDriveC, EDriveB, EDriveA,	EDriveZ	};

/**
 * Type of the widget which displays Suite data
 *
 * @since S60 v5.0
 */
enum THnSuiteWidgetType
    {
    EUnspecified = 0x00,
    EGridWidget = 0x01,
    EListWidget = 0x02,
    ECoverFlowWidget = 0x04,
    EChangeWidget = 0x05
    };

/**
 * Service Mode
 */
enum TServiceMode
    {
    EServiceModeUndefined = 0,
    EServiceModeSynchronous,
    EServiceModeAsynchronous
    };

/**
 * Image source
 */
enum TImageSource
    {
    EImageSourceUndefined = 0,
    EImageSourceFile,
    EImageSourceResource,
    EImageSourceApplicationUid,
    EImageSourceLiw
    };

/**
 * Custom item ids.
 */
enum TCustomId
  {
  ECustomIdFirstItem = -2,
  ECustomIdLastItem = -3
  };

/**
 * Suite's exit mode
 */
enum TExitMode
    {
    EExitModeNormal = 0,
    EExitModeHide
    };

/**
 * Type of change in items number
 */
enum TItemsChangeType {
  EItemsAdded = 0,
  EItemsRemoved
};

_LIT8( KExitModeParams,  "params:exit" );
_LIT8( KActionParams,  "params:action" );
_LIT(  KExitModeHide,    "hide" );
_LIT(  KActionExit,    "exit" );

/*
 * MCS
 */
_LIT8(KMCSDataSourceInterface8,   "IDataSource");
_LIT8(KMCSMenuContentInterface8,  "IMenuContent");
_LIT8(KMCSService8,               "Service.MenuContent");
// Search and Result Types
_LIT8(KRecursiveSearch8,          "recursive_search");
_LIT8(KFlatResult8,               "flat_result");
_LIT8(KParentOnly8,               "parent_only");
_LIT8(KIncludeParent8,            "include_parent");
// GetList Parameters
_LIT8(KInData8,                   "InData");
_LIT8(KFilter8,                   "Filter");
// Command Names
_LIT8(KCmdAdd8,                   "Add");
_LIT8(KCmdDelete8,                "Delete");
_LIT8(KCmdExecuteAction8,         "ExecuteAction");
_LIT8(KCmdGetList8,               "GetList");
_LIT8(KCmdOrganize8,              "Organize");
// Flags
_LIT8(KDeleteLocked8,             "delete_locked");
_LIT8(KHidden8,                   "hidden");
_LIT8(KMissing8,                  "missing");
// Organize Parameters
_LIT8(KFolderId8,                 "folder_id");
_LIT8(KBeforeItemId8,             "before_item_id");

_LIT8( KUriHighlight,             "uri_highlight" );
_LIT8( KParamsUriHighlight,       "params:uri_highlight" );

_LIT8( KUriHighlightSuite,        "uri_highlight_suite" );
_LIT8( KParamsUriHighlightSuite,  "params:uri_highlight_suite" );

#endif // C_HNGLOBALS_H
