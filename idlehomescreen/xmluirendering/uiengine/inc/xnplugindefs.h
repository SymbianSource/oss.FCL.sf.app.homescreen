/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plugin constant definitions
*
*/


#ifndef _XNPLUGINDEFS_H_
#define _XNPLUGINDEFS_H_

// System includes
#include <e32std.h> 

// Constants
const TInt KContentIdMaxLength ( 255 );
const TInt KResultMaxLength ( 255 );
const TInt KSAPIContentNameMaxLength ( 255 ); 
const TUint KSepratorChar ( '/' );

_LIT8( KPluginInterface, "2001f48a" );
_LIT8( KPlugin, "plugin" );
_LIT8( KKeyWidget, "widget" );
_LIT8( KKeyTemplate, "template");
_LIT8( KType, "type" );
_LIT8( KConfigurationId, "configurationid" );
_LIT8( KConfigurationUid, "configurationuid" );
_LIT8( KPluginId, "pluginId" );
_LIT8( KPluginName, "pluginname" );
_LIT16( KContentSource16, "ContentSource" );
_LIT8( KContentSource, "ContentSource" );
_LIT8( KContentData, "contentdata" );
_LIT8( KView, "view" );
_LIT8( KApplication, "application" );
_LIT( KSeperator, "/" );
_LIT( KOpen, "(" );
_LIT( KClose, ")" );
_LIT( KHSOnLine_OffLine, "online_offline" );
_LIT( KHSOnLine, "online" );
_LIT( KHSOffLine, "offline" );
		
_LIT8( KNameSeperator, ":" );
_LIT8( KOpenBrace, "[" );
_LIT8( KCloseBrace, "]" );

_LIT8( KSettingsConfiguration, "settingsconfiguration" );
_LIT8( KConfigurationModel, "configuration" );
_LIT8( KContentModel, "contentmodel") ;
_LIT8( KContentSourceNode, "contentsource");

_LIT8( KSource, "source" );
_LIT8( KLaunch, "launch" );
_LIT8( KName, "name" );
_LIT8( KStatus, "status" );

_LIT( KDummy16, "empty" );
_LIT8( KDummy8, "empty" );

// CPS constants
_LIT( KTemplatedWidget, "ai3templatedwidget" );
_LIT(  KPublisher16, "publisher" );
_LIT8( KPublisher, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT8( KFilter, "filter" );
_LIT8( KGetList, "GetList" );
_LIT8( KResults, "results" );
_LIT8( KTemplateType, "template_type" );
_LIT8( KWidgetName, "widget_name" );
_LIT8( KWidgetInfo, "widget_info" );

_LIT8( KTrigger, "trigger" );
_LIT8( KPubTrigger, "pubtrigger" );
_LIT8( KDataMap, "data_map" );
_LIT8( KWidgetTriggerName, "(selected)" );
_LIT8( KItem, "item" );
_LIT( KAll, "all" );
_LIT8( KOperation, "operation" );
_LIT8( KChangeInfo, "change_info" );
_LIT8( KDelete, "delete" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KOpen8, "(" );

_LIT8( KWallpaper, "wallpaper" );
_LIT8( KPath, "path" );
_LIT8( KWRTTemplate, "ai3templatedwidget");

#endif /* _XNPLUGINDEFS_H_ */
