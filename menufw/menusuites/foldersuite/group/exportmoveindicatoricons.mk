#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  resource makefile for folder suite.
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/Z
else
ZDIR=$(EPOCROOT)epoc32/data/z
endif

FINAL :
	echo Exporting mbg file...
	perl -S $(EPOCROOT)epoc32/tools/ecopyfile.pl "$(EPOCROOT)epoc32/include/move_indicator_icons.mbg" "$(ZDIR)/resource/apps/move_indicator_icons.mbg"
  
MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE :
