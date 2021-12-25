#pragma once
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_Keyboard Enj_Keyboard;
typedef struct Enj_Keyboard{
    int textmode;

    int active;

    void (*onpress)(const char *, void *);
    void (*onunpress)(const char *, void *);
    void (*oninput)(const char *, void *);

    void *data;

    Enj_Keyboard *prev;
    Enj_Keyboard *next;
}Enj_Keyboard;


typedef struct Enj_KeyboardList{
    Enj_Keyboard *head;
    Enj_Keyboard *tail;
    Enj_Allocator *alloc;

    size_t size;
} Enj_KeyboardList;

void Enj_InitKeyboardList(Enj_KeyboardList *sl, Enj_Allocator *a);
void Enj_FreeKeyboardList(Enj_KeyboardList *sl);
Enj_Keyboard * Enj_CreateKeyboard(Enj_KeyboardList *sl);
void Enj_DestroyKeyboard(Enj_KeyboardList *sl, Enj_Keyboard *s);
Enj_Keyboard * Enj_GetKeyboardListHead(Enj_KeyboardList *sl);
Enj_Keyboard * Enj_GetKeyboardListTail(Enj_KeyboardList *sl);
size_t Enj_GetKeyboardListSize(Enj_KeyboardList *sl);

#ifdef __cplusplus
}
#endif
