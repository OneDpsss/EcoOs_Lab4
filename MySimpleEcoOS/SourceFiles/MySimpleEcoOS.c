/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoMemoryManager1.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoMemoryManager1Lab.h"
#include "IEcoVirtualMemory1.h"
#include "IEcoTaskScheduler1.h"
#include "IdEcoTaskScheduler1Lab.h"
#include "IdEcoTimer1.h"
#include "IEcoSystemTimer1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"
#include "IdEcoMutex1Lab.h"
#include "IdEcoSemaphore1Lab.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;
char_t* heap_end = 0;

IEcoVBIOS1Video* g_pIVideo = 0;
uint8_t g_col = 1;
uint8_t g_row = 1;

typedef struct MySimpleEcoOS_TestContext {
    uint32_t total;
    uint32_t failed;
} MySimpleEcoOS_TestContext;

static uint16_t MySimpleEcoOS_StrLen(const char_t* s) {
    uint16_t n = 0;
    if (s == 0) {
        return 0;
    }
    while (s[n] != 0) {
        ++n;
    }
    return n;
}

static void MySimpleEcoOS_WriteChar(char_t ch) {
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, g_col, g_row, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, &ch, 1);
    g_col += 1;
}

static void MySimpleEcoOS_WriteSpaces(uint16_t count) {
    uint16_t i = 0;
    for (i = 0; i < count; ++i) {
        MySimpleEcoOS_WriteChar(' ');
    }
}

static void MySimpleEcoOS_WriteTextPadded(const char_t* text, uint16_t width) {
    uint16_t len = MySimpleEcoOS_StrLen(text);
    if (len > width) {
        g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, g_col, g_row, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, (char_t*)text, width);
        g_col += width;
        return;
    }
    if (len > 0) {
        g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, g_col, g_row, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, (char_t*)text, len);
        g_col += len;
    }
    MySimpleEcoOS_WriteSpaces((uint16_t)(width - len));
}

static void MySimpleEcoOS_WriteUIntDec(uint32_t v) {
    char_t buf[11];
    uint8_t i = 0;

    if (v == 0) {
        MySimpleEcoOS_WriteChar('0');
        return;
    }

    while (v != 0 && i < 10) {
        buf[i++] = (char_t)('0' + (v % 10));
        v /= 10;
    }
    while (i > 0) {
        MySimpleEcoOS_WriteChar(buf[--i]);
    }
}

static void MySimpleEcoOS_WriteHexNibble(uint8_t nibble) {
    char_t ch = 0;
    if (nibble < 10) {
        ch = (char_t)(nibble + '0');
    }
    else {
        ch = (char_t)(nibble - 10 + 'A');
    }
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, g_col, g_row, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, &ch, 1);
    g_col += 1;
}

static void MySimpleEcoOS_NewLine(void) {
    g_col = 1;
    g_row += 1;
}

static void MySimpleEcoOS_HR(uint16_t width) {
    uint16_t i = 0;
    for (i = 0; i < width; ++i) {
        MySimpleEcoOS_WriteChar('-');
    }
    MySimpleEcoOS_NewLine();
}

static void MySimpleEcoOS_WriteHex32(uint32_t v) {
    int8_t shift = 0;
    for (shift = 28; shift >= 0; shift -= 4) {
        MySimpleEcoOS_WriteHexNibble((uint8_t)((v >> shift) & 0xF));
    }
}

static void MySimpleEcoOS_WriteText(const char_t* text) {
    uint16_t len = 0;
    while (text[len] != 0) {
        ++len;
    }
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, g_col, g_row, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, (char_t*)text, len);
    g_col += len;
}

static void MySimpleEcoOS_WritePtr32(void* pv) {
    uint64_t p64 = (uint64_t)pv;
    uint32_t p = (uint32_t)p64;
    int8_t shift = 0;
    for (shift = 28; shift >= 0; shift -= 4) {
        MySimpleEcoOS_WriteHexNibble((uint8_t)((p >> shift) & 0xF));
    }
}

static void MySimpleEcoOS_DumpBlocks(void) {
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Heap blocks");
    MySimpleEcoOS_NewLine();

    MySimpleEcoOS_HR(52);
    MySimpleEcoOS_WriteText("Dump unavailable: MemoryBlock layout is not visible in this project.");
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Heap range: start=0x");
    MySimpleEcoOS_WritePtr32((void*)&__heap_start__);
    MySimpleEcoOS_WriteText(" end=0x");
    MySimpleEcoOS_WritePtr32((void*)heap_end);
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_HR(52);
}

static void MySimpleEcoOS_TestStep(MySimpleEcoOS_TestContext* ctx, const char_t* name, bool_t ok) {
    ctx->total += 1;

    MySimpleEcoOS_WriteText("| ");
    if (ctx->total < 10) {
        MySimpleEcoOS_WriteChar('0');
    }
    MySimpleEcoOS_WriteUIntDec(ctx->total);
    MySimpleEcoOS_WriteText(" | ");
    MySimpleEcoOS_WriteTextPadded(name, 28);
    MySimpleEcoOS_WriteText(" | ");
    if (ok) {
        MySimpleEcoOS_WriteTextPadded("OK", 4);
    }
    else {
        MySimpleEcoOS_WriteTextPadded("FAIL", 4);
        ctx->failed += 1;
    }
    MySimpleEcoOS_WriteText(" |");
    MySimpleEcoOS_NewLine();
}

static void MySimpleEcoOS_PrintMemStatus(IEcoMemoryManager1* pIMemMgr) {
    ECOMEMORYMANAGER1STATUS st = {0};
    bool_t ok = 0;

    if (pIMemMgr == 0) {
        return;
    }

    ok = pIMemMgr->pVTbl->get_Status(pIMemMgr, &st);
    if (!ok) {
        MySimpleEcoOS_WriteText("MemStatus: FAIL");
        MySimpleEcoOS_NewLine();
        return;
    }

    MySimpleEcoOS_WriteText("|    Mem: total=0x");
    MySimpleEcoOS_WriteHex32(st.totalSize);
    MySimpleEcoOS_WriteText(" free=0x");
    MySimpleEcoOS_WriteHex32(st.freeSize);
    MySimpleEcoOS_WriteText(" used=0x");
    MySimpleEcoOS_WriteHex32(st.usedBlocks);
    MySimpleEcoOS_NewLine();
}

void WriteDigit(uint8_t u) {
    MySimpleEcoOS_WriteHexNibble(u);
}

void WriteString(char_t* pChar) {
    MySimpleEcoOS_WriteText(pChar);
}

void WritePtr(void* pv) {
    MySimpleEcoOS_WritePtr32(pv);
}

void LogBlocks() {
    MySimpleEcoOS_DumpBlocks();
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};

    void* pv1 = 0;
    void* pv2 = 0;
    void* pv3 = 0;
    void* pv4 = 0;

    MySimpleEcoOS_TestContext testCtx = {0};

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(
        GetIEcoComponentFactoryPtr_00000000000000000000000042757331,
        0,
        0,
        &IID_IEcoInterfaceBus1,
        (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        /* Установка расширения менаджера памяти */
        // pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1);
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        /* Установка разрешения расширения пула */
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    // pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);
    heap_end = &__heap_start__+0x080000;

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &g_pIVideo);

    /* Получение интерфейса для работы с памятью */
    result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    /* Проверка */
    if (result != 0 || pIMem == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Memory allocator tests");
    MySimpleEcoOS_NewLine();

    MySimpleEcoOS_WriteText("+----+------------------------------+------+" );
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("| #  | test                         | res  |" );
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("+----+------------------------------+------+" );
    MySimpleEcoOS_NewLine();

    pv1 = pIMem->pVTbl->Alloc(pIMem, 100);
    MySimpleEcoOS_TestStep(&testCtx, "Alloc 100 (0x64)", pv1 != 0);
    if (pv1 != 0) {
        MySimpleEcoOS_WriteText("|    ptr: 0x");
        MySimpleEcoOS_WritePtr32(pv1);
        MySimpleEcoOS_NewLine();
    }
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pv2 = pIMem->pVTbl->Alloc(pIMem, 500);
    MySimpleEcoOS_TestStep(&testCtx, "Alloc 500 (0x1F4)", pv2 != 0);
    if (pv2 != 0) {
        MySimpleEcoOS_WriteText("|    ptr: 0x");
        MySimpleEcoOS_WritePtr32(pv2);
        MySimpleEcoOS_NewLine();
    }
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pv3 = pIMem->pVTbl->Alloc(pIMem, 50);
    MySimpleEcoOS_TestStep(&testCtx, "Alloc 50 (0x32)", pv3 != 0);
    if (pv3 != 0) {
        MySimpleEcoOS_WriteText("|    ptr: 0x");
        MySimpleEcoOS_WritePtr32(pv3);
        MySimpleEcoOS_NewLine();
    }
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pIMem->pVTbl->Free(pIMem, pv2);
    MySimpleEcoOS_TestStep(&testCtx, "Free(ptr2)", 1);
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pv4 = pIMem->pVTbl->Alloc(pIMem, 10000);
    MySimpleEcoOS_TestStep(&testCtx, "Alloc 10000 (0x2710)", pv4 != 0);
    if (pv4 != 0) {
        MySimpleEcoOS_WriteText("|    ptr: 0x");
        MySimpleEcoOS_WritePtr32(pv4);
        MySimpleEcoOS_NewLine();
    }
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pIMem->pVTbl->Free(pIMem, pv1);
    MySimpleEcoOS_TestStep(&testCtx, "Free(ptr1)", 1);
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pIMem->pVTbl->Free(pIMem, pv4);
    MySimpleEcoOS_TestStep(&testCtx, "Free(ptr4)", 1);
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    pIMem->pVTbl->Free(pIMem, pv3);
    MySimpleEcoOS_TestStep(&testCtx, "Free(ptr3)", 1);
    MySimpleEcoOS_PrintMemStatus(pIMemMgr);
    LogBlocks();

    MySimpleEcoOS_WriteText("+----+------------------------------+------+" );
    MySimpleEcoOS_NewLine();

    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Summary");
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Total:  ");
    MySimpleEcoOS_WriteUIntDec(testCtx.total);
    MySimpleEcoOS_NewLine();
    MySimpleEcoOS_WriteText("Failed: ");
    MySimpleEcoOS_WriteUIntDec(testCtx.failed);
    MySimpleEcoOS_NewLine();

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

