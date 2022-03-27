#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_InputState {
    char windowminimized;
} Enj_InputState;

void Enj_InitInputState(Enj_InputState *state);

int Enj_HandleInput(
    Enj_InputState *state,
    Enj_ButtonList *buttons,
    Enj_KeyboardList *keyboards
);

#ifdef __cplusplus
}
#endif
