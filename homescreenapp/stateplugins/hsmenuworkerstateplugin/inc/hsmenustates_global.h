/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#ifndef HSMENUSTATES_GLOBAL_H
#define HSMENUSTATES_GLOBAL_H

#include <QDebug>
#include <QtGlobal>

#ifndef MENUSTATES_UNITTEST
#define HS_STATES_TEST_CLASS(className)
#else
#define HS_STATES_TEST_CLASS(className) class className;
#endif

#ifndef MENUSTATES_UNITTEST
#define HS_STATES_TEST_FRIEND_CLASS(className)
#else
#define HS_STATES_TEST_FRIEND_CLASS(className) friend class className;
#endif

static const char *const HS_APP_CHECK_LIST_LAYOUT =
    ":/xml/hsappchecklist.docml";
static const char *const HS_WIDGET_PREVIEW_DIALOG_LAYOUT =
    ":/xml/hswidgetpreviewdialog.docml";
static const char *const HS_WIDGET_PREVIEW_DIALOG_NAME="preview_dialog";
static const char *const HS_WIDGET_PREVIEW_LABEL_NAME="label";
static const char *const HS_WIDGET_PREVIEW_ICON_BOX_NAME="icon_box";
static const char *const HS_PREVIEW_ATTRIBUTE_NAME="preview_image_name";

static const char *const HS_ARRANGE_DIALOG_LAYOUT =
    ":/xml/hsarrangedialog.docml";
static const char *const HS_ARRANGE_DIALOG_NAME="arrange_dialog";
static const char *const HS_ARRANGE_LIST_NAME="arrange_list";

static const char *const HS_DETAILS_DIALOG_LAYOUT =
    ":/xml/hsdetailsdialog.docml";
static const char *const HS_DETAILS_DIALOG_NAME="details_dialog";

static const char *const HS_UNINSTALL_DIALOG_LAYOUT =
    ":/xml/hsuninstalldialog.docml";
static const char *const HS_UNINSTALL_DIALOG_NAME ="uninstall_dialog";
static const char *const HS_UNINSTALL_DIALOG_DELETE_LABEL ="delete_message_label";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_LABEL ="confirmation_dialog_label";
static const char *const HS_UNINSTALL_DIALOG_LIST_VIEW_LABEL ="list_view_label";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_ACTION ="qtl_dialog_softkey_2_left";
static const char *const HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_ICON ="icon";


/*!
 To enable logging of function entry and exit use the following flag for qmake:
 -config nft
 To include in logs extra information about RAM and heap usage, define an additional constant e.g. in ProductVariant.hrh:
 #define NFT_RAM
 */

#ifdef NFT
#ifdef Q_OS_SYMBIAN
#include <hal.h>
#include <e32std.h>
#endif
#endif

#ifdef NFT

#define HSMENUTEST(aText) qDebug() << QString(aText)

#ifdef Q_OS_SYMBIAN
#ifdef NFT_RAM
#define HSMENUTEST_FREERAM_ENTRY(function) \
    TInt HSMENUTEST_ENTRY_RAM(0); \
    TInt HSMENUTEST_ENTRY_HEAP(0); \
    TInt HSMENUTEST_ENTRY_HEAP_SIZE(0); \
    { \
        TInt allRAM(0); \
        HAL::Get(HAL::EMemoryRAM, allRAM); \
        HAL::Get(HAL::EMemoryRAMFree, HSMENUTEST_ENTRY_RAM); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        HSMENUTEST_ENTRY_HEAP = heap.Available(biggestBlock); \
        HSMENUTEST_ENTRY_HEAP_SIZE = heap.Size(); \
        qDebug("(nft) " function " - Memory (kB) - Free RAM: %d, Heap size: %d, Free heap: %d", \
               HSMENUTEST_ENTRY_RAM >> 10, HSMENUTEST_ENTRY_HEAP_SIZE >> 10, \
               HSMENUTEST_ENTRY_HEAP >> 10); \
    }

#define HSMENUTEST_FREERAM_EXIT(function) \
    { \
        TInt allRAM(0); \
        TInt freeRAM(0); \
        HAL::Get(HAL::EMemoryRAM, allRAM); \
        HAL::Get(HAL::EMemoryRAMFree, freeRAM); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        TInt heapFree = heap.Available(biggestBlock); \
        TInt heapSize = heap.Size(); \
        qDebug("(nft) " function " - Memory (kB) - Free RAM: %d, Heap size: %d, Free heap: %d", \
               freeRAM >> 10, heapSize >> 10, heapFree >> 10); \
        qDebug("(nft) " function " - Memory (kB) - RAM diff: %d, Heap size diff: %d, Free heap diff: %d", \
               (freeRAM-HSMENUTEST_ENTRY_RAM) >> 10, (heapSize-HSMENUTEST_ENTRY_HEAP_SIZE) >> 10, \
               (heapFree-HSMENUTEST_ENTRY_HEAP) >> 10); \
    }

#else

#define HSMENUTEST_FREERAM_ENTRY(function)
#define HSMENUTEST_FREERAM_EXIT(function)

#endif

#define HSMENUTESTTIME_ENTRY(function) \
    TInt64 HSMENUTEST_ENTRY_TIME(0); \
    { \
        TTime t; \
        t.UniversalTime(); \
        qDebug("\n" function "      entry:%20lld", t.Int64()); \
        HSMENUTEST_ENTRY_TIME = t.Int64(); \
    }

#define HSMENUTESTTIME_EXIT(function) { \
        TTime t; \
        t.UniversalTime(); \
        qDebug("\n" function "      entry:%20lld\n" function " difference:%20lld", t.Int64(), \
               t.Int64()-HSMENUTEST_ENTRY_TIME); \
    }

#define HSMENUTEST_FUNC_ENTRY(function)   HSMENUTEST_FREERAM_ENTRY(function) \
    HSMENUTESTTIME_ENTRY(function)
#define HSMENUTEST_FUNC_EXIT(function)    HSMENUTESTTIME_EXIT(function) \
    HSMENUTEST_FREERAM_EXIT(function)

#else //Q_OS_SYMBIAN
#define HSMENUTEST_FUNC_ENTRY(function)   qDebug()<< function << " entry";
#define HSMENUTEST_FUNC_EXIT(function)    qDebug()<< function << " exit";
#endif //Q_OS_SYMBIAN
#else //NFT
#define HSMENUTEST_FUNC_ENTRY(function)
#define HSMENUTEST_FUNC_EXIT(function)
#endif //NFT

#endif //HSMENUSTATES_GLOBAL_H
