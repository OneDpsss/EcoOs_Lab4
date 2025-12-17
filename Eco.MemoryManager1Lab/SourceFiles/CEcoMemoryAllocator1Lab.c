/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoMemoryAllocator1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов IEcoMemoryAllocator1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "CEcoMemoryManager1Lab.h"


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_QueryInterface(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoMemoryManager1) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoMemoryAllocator1) ) {
        *ppv = &pCMe->m_pVTblIMem;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoVirtualMemory1) ) {
        *ppv = &pCMe->m_pVTblIVirtual;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_AddRef(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Release(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoMemoryManager1Lab_623E1838((IEcoMemoryManager1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Alloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Alloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    return pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, size);

}

/*
 *
 * <сводка>
 *   Функция Free
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Free(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pv);
}

/*
 *
 * <сводка>
 *   Функция Realloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Realloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Copy
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Copy(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ voidptr_t pvSrc, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Fill
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Fill(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ char_t Fill, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Compare
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Compare(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv1, /* in */ voidptr_t pv2, /* in */ uint32_t size) {
    /*CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));*/

    return 0;
}
