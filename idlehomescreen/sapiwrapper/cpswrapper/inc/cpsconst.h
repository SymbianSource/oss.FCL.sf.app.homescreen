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
 * Description:  Defines the CPS constants
 *
 */

#ifndef CPSCONST_H
#define CPSCONST_H

namespace cpswrapper
    {
    
	// Local constants
	_LIT8( KCPSConfigurationIf, "IContentPublishing" );
	_LIT8( KCPS, "Service.ContentPublishing" );
	// CPS constants
	_LIT( KHSTemplate, "hstemplate" );
	_LIT( KPublisher16, "publisher" );
	_LIT8( KPublisher, "publisher" );
	_LIT8( KContentType, "content_type" );
	_LIT8( KContentId, "content_id" );
	_LIT8( KFilter, "filter" );
	_LIT( KAll, "all");
	_LIT8( KOperation, "operation" );
	_LIT8( KChangeInfo, "change_info" );
	_LIT8( KDelete, "delete" );
	_LIT( KDelete16, "delete" );
	_LIT8( KAdd, "add" );
	_LIT( KAdd16, "add" );
	_LIT8( KRequestNotification, "RequestNotification" );
	_LIT8( KType, "type" );
	_LIT8( KGetList, "GetList" );
	_LIT8( KResults, "results" );
	_LIT8( KTemplateType, "template_type");
	_LIT8( KWidgetName, "widget_name");
	_LIT8( KDataMap, "data_map" );
	_LIT8( KMaxWidgets, "widget_inst");
    _LIT8( KWidgetDesc,   "widget_desc");
    _LIT8( KWidgetIcon, "widget_icon");
	
	_LIT( KWRTTemplate, "ai3templatedwidget");
	_LIT( KWRTPublisher, "wrt_publisher");
	_LIT8( KWidgetInfo,   "widget_info");
	
    }
#endif /* CPSCONST_H */
