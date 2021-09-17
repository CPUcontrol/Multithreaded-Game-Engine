#include "act.h"

void Enj_ActEnter(Enj_Act *act, void *state){
    (*act->onenter)(state, act->proc);
}
void Enj_ActExit(Enj_Act *act, void *state){
    (*act->onexit)(state, act->proc);
}
int Enj_ActUpdate(Enj_Act *act, void *state){
    return (*act->onupdate)(state, act->proc);
}
