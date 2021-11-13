#pragma once
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_Button Enj_Button;
typedef struct Enj_Button{
    SDL_Rect rect;
    int hovering;
    int pressing;

    void (*onpress)(int, int, void *);
    void (*onunpress)(int, int, void *);
    void (*onhover)(int, int, void *);
    void (*onunhover)(int, int, void *);

    void *data;

    Enj_Button *prev;
    Enj_Button *next;
}Enj_Button;


typedef struct Enj_ButtonList{
    Enj_Button *head;
    Enj_Button *tail;
    Enj_Allocator *alloc;

    size_t size;
} Enj_ButtonList;

void Enj_InitButtonList(Enj_ButtonList *sl, Enj_Allocator *a);
void Enj_FreeButtonList(Enj_ButtonList *sl);
Enj_Button * Enj_CreateButton(Enj_ButtonList *sl);
void Enj_DestroyButton(Enj_ButtonList *sl, Enj_Button *s);
Enj_Button * Enj_GetButtonListHead(Enj_ButtonList *sl);
Enj_Button * Enj_GetButtonListTail(Enj_ButtonList *sl);
size_t Enj_GetButtonListSize(Enj_ButtonList *sl);

#ifdef __cplusplus
}
#endif
