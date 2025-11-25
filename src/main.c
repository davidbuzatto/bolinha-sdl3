/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Testes SDL 3 - Bolinha
 * 
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Tipos.h"
#include "MundoJogo.h"

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define GRAVIDADE 2000
#define FPS 60

static MundoJogo *mj;

SDL_AppResult executarQuadroJogo( MundoJogo *mj ) {

    Uint64 tempoAtual = SDL_GetTicks();

    mj->lj.delta = (float) ( tempoAtual - mj->lj.tempoAntes ) / 1000.0f;
    mj->lj.tempoAntes = tempoAtual;

    atualizarMundoJogo( mj, mj->lj.delta );
    desenharMundoJogo( mj );

    // controla o FPS
    mj->lj.tempoDepois = SDL_GetTicks();
    mj->lj.tempoQuadro = mj->lj.tempoDepois - mj->lj.tempoAntes;

    if ( mj->lj.atrasoQuadro > mj->lj.tempoQuadro ) {
        SDL_Delay( mj->lj.atrasoQuadro - mj->lj.tempoQuadro );
    }
    
    return SDL_APP_CONTINUE;

}

// inicialização
SDL_AppResult SDL_AppInit( void **appState, int argc, char *argv[] ) {

    mj = criarMundoJogo( FPS, GRAVIDADE );

    // appState é um ponteiro para ponteiros que aponta para o "estado da aplicação"
    *appState = mj;

    if ( !SDL_Init( SDL_INIT_VIDEO ) ) {
        SDL_Log( "Erro ao inicializar a SDL: %s", SDL_GetError() );
        return SDL_APP_FAILURE;
    }

    if ( !SDL_CreateWindowAndRenderer( "Testes SDL 3 - Bolinha", LARGURA_JANELA, ALTURA_JANELA, SDL_WINDOW_RESIZABLE, &mj->janela, &mj->renderer ) ) {
        SDL_Log( "Nao foi possível criar a janela ou o renderizador: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_srand( SDL_GetPerformanceCounter() ); 
    SDL_SetRenderLogicalPresentation( mj->renderer, LARGURA_JANELA, ALTURA_JANELA, SDL_LOGICAL_PRESENTATION_LETTERBOX );

    return SDL_APP_CONTINUE;

}

// tratamento de eventos
// void *appState renomeado para void *mundoJogo (clareza)
SDL_AppResult SDL_AppEvent( void *mundoJogo, SDL_Event *event ) {

    if ( event->type == SDL_EVENT_QUIT ) {
        return SDL_APP_SUCCESS;
    }

    tratarEventosMundoJogo( mundoJogo, event ); 

    return SDL_APP_CONTINUE;

}

// iteração do jogo/simulação
// void *appState renomeado para void *mundoJogo (clareza)
SDL_AppResult SDL_AppIterate( void *mundoJogo ) {
    return executarQuadroJogo( mundoJogo );
}

// finalização
// void *appState renomeado para void *mundoJogo (clareza)
void SDL_AppQuit( void *mundoJogo, SDL_AppResult result ) {
    destruirMundoJogo( mundoJogo );
}
