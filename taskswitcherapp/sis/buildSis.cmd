@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off
setlocal EnableDelayedExpansion
set openpar=(
set closepar=)

if exist *.sis* ( del *.sis* )
if exist tmp.pkg ( del tmp.pkg )

copy /Y taskswitcher.pkg tmp.pkg > NUL

echo Creating sis for each component:
for /R .. %%i IN (*_template.pkg) DO set tmpname=%%~ni && set modulename=!tmpname:~0,-10!&& call createpackage %%i release-armv5 cert.cer key.pem > NUL && set sisname=%%~dpi!modulename!_release-armv5.sis && if exist !sisname! ( echo [SUCCEEDED] !modulename! [SUCCEEDED] && move !sisname! . && FOR /F "eol=( tokens=4 delims=({}" %%j IN (%%i) DO FOR /F "tokens=1 delims=)" %%k IN ("%%j") DO echo @"!modulename!_release-armv5.sis",!openpar!%%k!closepar! >> tmp.pkg ) else ( echo [FAILED] !modulename! [FAILED] )

REM echo.

echo Embedding all sis files into single sis
makesis tmp.pkg taskswitcher.sis >NUL
signsis taskswitcher.sis taskswitcher.sisx cert.cer key.pem >NUL

echo Cleaning up
del tmp.pkg
del taskswitcher.sis
del *_release-armv5.sis*
pause
