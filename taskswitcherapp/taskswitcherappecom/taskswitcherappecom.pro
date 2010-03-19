#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TEMPLATE = subdirs
BLD_INF_RULES.prj_mmpfiles = "./group/tsappecom.mmp"

load(data_caging_paths) 

ecomplugindll.sources = tsappecom.dll
ecomplugindll.path = $$SHARED_LIB_DIR

ecompluginresource.sources = /epoc32/data/z/resource/plugins/tsappecom.rsc
ecompluginresource.path = $$ECOM_RESOURCE_DIR

DEPLOYMENT += ecomplugindll ecompluginresource
