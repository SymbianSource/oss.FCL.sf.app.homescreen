rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

rem Change dll versions for IAD update
call elftran.exe -version 1.2 \epoc32\release\armv5\urel\widgetmanager.dll
call elftran.exe -version 1.2 \epoc32\release\armv5\udeb\widgetmanager.dll

rem Create wm sis&sisx files
makesis widgetmanager_udeb.pkg
signsis widgetmanager_udeb.SIS widgetmanager_udeb.SISX Nokia_RnDCert_02.der Nokia_RnDCert_02.key
makesis widgetmanager_urel.pkg
signsis widgetmanager_urel.SIS widgetmanager_urel.SISX Nokia_RnDCert_02.der Nokia_RnDCert_02.key

rem Create wm stubsis file
makesis -s widgetmanager_stub.pkg