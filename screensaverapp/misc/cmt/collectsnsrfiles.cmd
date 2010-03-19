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
rem dir ..\..\screensaverapp\*.cpp /s /b >snsrfiles.txt
dir ..\..\screensaverproviders\snsrbigclockscreensaverprovider\src\*.cpp /s /b >snsrfiles.txt
dir ..\..\runtimeproviders\snsrdefaultruntimeprovider\src\*.cpp /s /b >>snsrfiles.txt
dir ..\..\snsrapplication\src\*.cpp /s /b >>snsrfiles.txt
dir ..\..\stateproviders\snsrdefaultstateprovider\src\*.cpp /s /b >>snsrfiles.txt
dir ..\..\..\..\homescreensrv\bagetmodel\src\*.cpp /s /b >>snsrfiles.txt
dir ..\..\..\..\homescreensrv\screensavermodel\src\*.cpp /s /b >>snsrfiles.txt