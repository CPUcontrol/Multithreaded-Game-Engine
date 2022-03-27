#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "../core/button.h"
#include "../core/keyboard.h"

#include "input.h"

void Enj_InitInputState(Enj_InputState *state){
    state->windowminimized = 0;
}

int Enj_HandleInput(
    Enj_InputState *state,
    Enj_ButtonList *buttons,
    Enj_KeyboardList *keyboards
)
{
    if(Enj_GetKeyboardListSize(keyboards)){
        for(
            Enj_Keyboard *kb = Enj_GetKeyboardListTail(keyboards);
            kb;
            kb = kb->prev
        )
        {
            if(!kb->active) continue;

            if(kb->textmode && !SDL_IsTextInputActive()){
                SDL_StartTextInput();
            }
            else if(!kb->textmode && SDL_IsTextInputActive()){
                SDL_StopTextInput();
            }

            break;
        }
    }
    else if(SDL_IsTextInputActive()){
        SDL_StopTextInput();
    }

    SDL_Event ev;
    while(SDL_PollEvent(&ev)){
        switch(ev.type){
        case SDL_QUIT:
            return 1;
        case SDL_WINDOWEVENT:
            switch (ev.window.event){
            case SDL_WINDOWEVENT_MINIMIZED:
                state->windowminimized = 1;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                state->windowminimized = 0;
                break;
            }
            break;
        case SDL_KEYDOWN:{
                for(
                    Enj_Keyboard *itb = Enj_GetKeyboardListTail(keyboards);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    if(itb->onpress){
                        (*itb->onpress)(
                            SDL_GetKeyName(ev.key.keysym.sym),
                            itb->data);
                    }

                    break;
                }
            }
            break;
        case SDL_KEYUP:{
                for(
                    Enj_Keyboard *itb = Enj_GetKeyboardListTail(keyboards);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    if(itb->onunpress){
                        (*itb->onunpress)(
                            SDL_GetKeyName(ev.key.keysym.sym),
                            itb->data);
                    }

                    break;
                }
            }
            break;
        case SDL_TEXTINPUT:{
                for(
                    Enj_Keyboard *itb = Enj_GetKeyboardListTail(keyboards);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    if(itb->oninput){
                        (*itb->oninput)(
                            ev.text.text,
                            itb->data);
                    }

                    break;
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(ev.button.button == SDL_BUTTON_LEFT){
                for(
                    Enj_Button *itb = Enj_GetButtonListTail(buttons);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    int xrel = ev.button.x - itb->rect.x;
                    int yrel = ev.button.y - itb->rect.y;
                    if(!( (xrel >= 0)
                        & (xrel < itb->rect.w)
                        & (yrel >= 0)
                        & (yrel < itb->rect.h)))
                        continue;

                    if(itb->pressing) continue;

                    if(itb->onpress){
                        (*itb->onpress)(
                            ev.button.x,
                            ev.button.y,
                            itb->data);

                    }
                    itb->pressing = 1;

                    break;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            //Different from mouse button down in that it does not
            //consider bounding boxes
            if(ev.button.button == SDL_BUTTON_LEFT){
                for(
                    Enj_Button *itb = Enj_GetButtonListTail(buttons);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    if(!itb->pressing) continue;

                    if(itb->onunpress){
                        (*itb->onunpress)(
                            ev.button.x,
                            ev.button.y,
                            itb->data);
                    }
                    itb->pressing = 0;
                }
            }
            break;
        case SDL_MOUSEMOTION:{

            char hoverfound = 0;

            for(
                Enj_Button *itb = Enj_GetButtonListTail(buttons);
                itb;
                itb = itb->prev
            )
            {
                if(!itb->active) continue;

                int xrel = ev.button.x - itb->rect.x;
                int yrel = ev.button.y - itb->rect.y;
                if( !hoverfound
                    & (xrel >= 0)
                    & (xrel < itb->rect.w)
                    & (yrel >= 0)
                    & (yrel < itb->rect.h)){

                    if(!itb->hovering){
                        itb->hovering = 1;
                        if(itb->onhover){
                            (*itb->onhover)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);

                        }
                    }

                    hoverfound = 1;
                }
                else{
                    if(itb->hovering){
                        itb->hovering = 0;
                        if(itb->onunhover){
                            (*itb->onunhover)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);

                        }
                    }
                }
            }
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
