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
	echo Exporting .rsg files...
	perl -S $(EPOCROOT)epoc32/tools/ecopyfile.pl "$(EPOCROOT)epoc32/include/foldersuite.rsg" "$(ZDIR)/resource/apps/foldersuite.rsg"
	echo Exporting .mbg files...
	perl -S $(EPOCROOT)epoc32/tools/ecopyfile.pl "$(EPOCROOT)epoc32/include/gridroot.mbg" "$(ZDIR)/resource/apps/gridroot.mbg"
	perl -S $(EPOCROOT)epoc32/tools/ecopyfile.pl "$(EPOCROOT)epoc32/include/matrixmenudata.mbg" "$(ZDIR)/resource/apps/matrixmenudata.mbg"
  
MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE :
