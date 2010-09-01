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
# Description:  scalable icon makefile for project
#

FINAL :
	echo VALIDATE
	ant -q -Dxmllocation="\epoc32\release\winscw\udeb\Z\private\101F4CD2\import\suites\"
MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE :
