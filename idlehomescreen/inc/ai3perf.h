/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines macros to log performance related details
*
*/


#ifndef AI3PERF_H
#define AI3PERF_H
#include "ai3.hrh"
#ifdef AI3_DEBUG_PERFORMANCE
#include "ai3performance.h"
  #define AI3_PERF_START(p1,p2) CAi3Perf::TPerfData* p1 = CAi3Perf::StartTestCase(_L(p2));
  #define AI3_PERF_STOP(p1,p2) CAi3Perf::StopTestCase(p1,_L(p2));
#else
  #define AI3_PERF_START(p1,p2)
  #define AI3_PERF_STOP(p1,p2)
#endif

#endif //AI3PERF_H