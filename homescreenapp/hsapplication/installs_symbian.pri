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

deploy.path = z:

exports1.path = /private/20022F35
exports1.sources += data/backup_registration.xml
for(export1, exports1.sources):BLD_INF_RULES.prj_exports += "./$$export1 $$deploy.path$$exports1.path/$$basename(export1)"

exports2.path = /private/20022f35/wallpapers/scene
exports2.sources += resource/scene/default_landscape.png
exports2.sources += resource/scene/default_portrait.png
for(export2, exports2.sources):BLD_INF_RULES.prj_exports += "./$$export2 $$deploy.path$$exports2.path/$$basename(export2)"

exports3.path = /private/20022f35/wallpapers/page
exports3.sources += resource/page/default_landscape.png
exports3.sources += resource/page/default_portrait.png
exports3.sources += resource/page/1_landscape.png
exports3.sources += resource/page/1_portrait.png
exports3.sources += resource/page/2_landscape.png
exports3.sources += resource/page/2_portrait.png
exports3.sources += resource/page/3_landscape.png
exports3.sources += resource/page/3_portrait.png
for(export3, exports3.sources):BLD_INF_RULES.prj_exports += "./$$export3 $$deploy.path$$exports3.path/$$basename(export3)"
