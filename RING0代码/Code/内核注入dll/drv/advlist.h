/*
    双向链表 哈希链表 无脑宏
    支持内核/用户态模式

    mengxp
    2013/09/06
*/

#ifndef _ADVANCED_LIST_H
#define _ADVANCED_LIST_H

#ifndef _WINNT_
#ifndef _NTDEF_

typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;

#endif
#endif

#ifndef _NTDEF_

FORCEINLINE
    VOID
    InitializeListHead(
    __out PLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

__checkReturn
    BOOLEAN
    FORCEINLINE
    IsListEmpty(
    __in const LIST_ENTRY * ListHead
    )
{
    return (BOOLEAN)(ListHead->Flink == ListHead);
}

FORCEINLINE
    BOOLEAN
    RemoveEntryList(
    __in PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}

FORCEINLINE
    PLIST_ENTRY
    RemoveHeadList(
    __inout PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}



FORCEINLINE
    PLIST_ENTRY
    RemoveTailList(
    __inout PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}


FORCEINLINE
    VOID
    InsertTailList(
    __inout PLIST_ENTRY ListHead,
    __inout __drv_aliasesMem PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


FORCEINLINE
    VOID
    InsertHeadList(
    __inout PLIST_ENTRY ListHead,
    __inout __drv_aliasesMem PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

FORCEINLINE
    VOID
    AppendTailList(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY ListToAppend
    )
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
}

#endif

#define LIST_FOR_EACH(pList, pListHead) \
    for(pList = (pListHead)->Flink; \
        pList != (pListHead); \
        pList = pList->Flink)

#define LIST_FOR_EACH_ENTRY(pUser, pList, pListHead, Type, Field) \
    for(pList = (pListHead)->Flink; \
    pList != (pListHead) && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
    pList = pList->Flink)

#define LIST_FOR_EACH_ENTRY_CONTINUE(pUser, pList, pListStart, pListHead, Type, Field) \
    for(pList = (pListStart)->Flink; \
    pList != (pListHead) && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
    pList = pList->Flink)

#define LIST_FOR_EACH_SAFE(pList, pListNext, pListHead) \
    for(pList = (pListHead)->Flink, pListNext = pList->Flink; \
        pList != (pListHead); \
        pList = pListNext, pListNext = pList->Flink)

#define LIST_FOR_EACH_ENTRY_SAFE(pUser, pList, pListNext, pListHead, Type, Field) \
    for(pList = (pListHead)->Flink, pListNext = pList->Flink; \
        pList != (pListHead) && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
        pList = pListNext, pListNext = pList->Flink)


typedef struct _HLIST_ENTRY {
    struct _HLIST_ENTRY *Flink, **pBlink;
}HLIST_ENTRY,*PHLIST_ENTRY;

typedef struct _HLIST_FIRST {
    struct _HLIST_ENTRY *FirstLink;
}HLIST_FIRST,*PHLIST_FIRST;

FORCEINLINE VOID InitializeHList(PHLIST_ENTRY h)
{
    h->Flink = NULL;
    h->pBlink = NULL;
}

FORCEINLINE VOID RemoveEntryHList(PHLIST_ENTRY n)
{
    PHLIST_ENTRY Flink = n->Flink;
    PHLIST_ENTRY *pBlink = n->pBlink;

    if(pBlink)
        *pBlink = Flink;
    if(Flink)
        Flink->pBlink = pBlink;
    InitializeHList(n);
}

FORCEINLINE VOID InsertHListHead(PHLIST_FIRST h, PHLIST_ENTRY n)
{
    PHLIST_ENTRY FirstLink = h->FirstLink;

    n->Flink = FirstLink;
    if (FirstLink)
        FirstLink->pBlink = &n->Flink;
    h->FirstLink = n;
    n->pBlink = &h->FirstLink;
}


#define HLIST_FOR_EACH(pList, pListHead) \
    for(pList = (pListHead)->FirstLink; \
        pList ; \
        pList = pList->Flink)

#define HLIST_FOR_EACH_ENTRY(pUser, pList, pListHead, Type, Field) \
    for(pList = (pListHead)->FirstLink; \
        pList && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
        pList = pList->Flink)

#define HLIST_FOR_EACH_ENTRY_CONTINUE(pUser, pList, pListStart, Type, Field) \
    for(pList = (pListStart)->Flink; \
        pList && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
        pList = pList->Flink)

//for里面无法保存pListNext，必须手动添加pListNext = pList->Flink;
#define HLIST_FOR_EACH_SAFE(pList, pListNext, pListHead) \
    for(pList = (pListHead)->FirstLink; \
        pList ;\
        pList = pListNext)

//for里面无法保存pListNext，必须手动添加pListNext = pList->Flink;
#define HLIST_FOR_EACH_ENTRY_SAFE(pUser, pList, pListNext, pListHead, Type, Field) \
    for(pList = (pListHead)->FirstLink; \
        pList && (pUser = CONTAINING_RECORD(pList,Type,Field)); \
        pList = pListNext)

#endif