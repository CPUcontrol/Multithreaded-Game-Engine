#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Act{
    //Called when activated, allocate memory, resources
    void (*onenter)(void *, void *);
    //Called when activated, clean memory, free resources
    void (*onexit)(void *, void *);
    int (*onupdate)(void *, void *);

    void *proc;
}Enj_Act;

void Enj_ActEnter(Enj_Act *act, void *state);
void Enj_ActExit(Enj_Act *act, void *state);
int Enj_ActUpdate(Enj_Act *act, void *state);

#ifdef __cplusplus
}
#endif
