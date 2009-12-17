@ECHO OFF
SETLOCAL

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
rem Description: Validates ActiveIdle3 plugin configurations
rem

SET PATH_LINT=C:\Apps\libxml2-2.6.23.win32\bin\xmllint.exe
SET PATH_MANIFEST_XSD=hsps_manifest.xsd
SET PATH_DEFINITION_XSD=hsps_definition.xsd

IF NOT EXIST %PATH_MANIFEST_XSD% GOTO ERR_XSD
IF NOT EXIST %PATH_DEFINITION_XSD% GOTO ERR_XSD

SET ARGS_MANIFEST=--schema %PATH_MANIFEST_XSD% --noout
SET ARGS_DEFINITION=--schema %PATH_DEFINITION_XSD% --noout

echo.
echo Validating plugin configurations against the HSPS schema files...
echo.

FOR /F %%j in ('dir ..\manifest.dat /s/b^|find "\hsps\00\" ') do %PATH_LINT% %%j %ARGS_MANIFEST% 2>&1|find "error"

FOR /F %%j in ('dir ..\*.xml /s/b^|find "\hsps\00\" ') do %PATH_LINT% %%j %ARGS_DEFINITION% 2>&1|find "error"

GOTO END

:ERR_LINT
ECHO XMLLint was not found! Personlize the PATH_LINT variable in %0 and download the tool.
GOTO END

:ERR_XSD
ECHO Schema files were not found (%PATH_MANIFEST_XSD% and %PATH_DEFINITION_XSD%)!
GOTO END

:END

ENDLOCAL