@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

if exist pannablewidgetplugin.sisx del pannablewidgetplugin.sisx

makesis pannablewidgetplugin.pkg
signsis pannablewidgetplugin.sis pannablewidgetplugin.sisx ../../../sis/rd.cer ../../../sis/rd-key.pem

if exist pannablewidgetplugin.sisx (
echo pannablewidgetplugin.sisx creation SUCCEEDED
del pannablewidgetplugin.sis
)

if not exist pannablewidgetplugin.sisx (
echo pannablewidgetplugin.sisx creation FAILED
)