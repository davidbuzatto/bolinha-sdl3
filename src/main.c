/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Testes SDL - Bolinha
 * 
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>

#include "SDL3_gfxPrimitives.h"

#define PI 3.14159265358979323846f
#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define GRAVIDADE 2000
#define FPS 60

typedef struct {

    Uint64 fps;
    Uint64 atrasoQuadro;
    float deltaBase;
    float delta;

    Uint64 tempoAntes;
    Uint64 tempoDepois;
    Uint64 tempoQuadro;

} LoopJogo;

typedef struct {

    float x;
    float y;
    float raio;
    SDL_Color cor;

    float vx;
    float vy;
    float atrito;
    float elasticidade;
    bool arrastando;

    float xa;
    float ya;
    float xOffset;
    float yOffset;

} Bolinha;

typedef struct {

    // controle do loop do jogo
    LoopJogo lj;

    // entidades
    Bolinha **bolinhas;
    int quantidadeBolinhas;
    int quantidadeMaximaBolinhas;

    Bolinha *bolinhaEmArraste;

    // "globais"
    float gravidade;

    SDL_Window *janela;
    SDL_Renderer *renderer;

} MundoJogo;

static MundoJogo *mj;

SDL_AppResult executarQuadroJogo( MundoJogo *mj );

MundoJogo *criarMundoJogo( void );
void destruirMundoJogo( MundoJogo *mj );
void tratarEventosMundoJogo( MundoJogo *mj, SDL_Event *event );
void atualizarMundoJogo( MundoJogo *mj, float delta );
void desenharMundoJogo( MundoJogo *mj );
void adicionarBolinhaMundoJogo( MundoJogo *mj, float x, float y, float raio, float vx, float vy );

Bolinha* criarBolinha( float x, float y, float raio, float vx, float vy );
void destruirBolinha( Bolinha *bolinha );
void tratarEventosBolinha( Bolinha *bolinha, MundoJogo *mj, SDL_Event *event );
void atualizarBolinha( Bolinha *bolinha, float gravidade, float delta );
void desenharBolinha( Bolinha *bolinha, SDL_Renderer *renderer );

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

/*SDL_AppResult executarQuadroJogo( MundoJogo *mj ) {

    mj->lj.tempoQuadro = mj->lj.tempoDepois - mj->lj.tempoAntes;
    mj->lj.delta = 0;

    if ( mj->lj.atrasoQuadro > mj->lj.tempoQuadro ) {
        SDL_Delay( mj->lj.atrasoQuadro - mj->lj.tempoQuadro );
        mj->lj.delta = mj->lj.deltaBase;
    }

    mj->lj.tempoAntes = SDL_GetTicks();

    atualizarMundoJogo( mj, mj->lj.delta );
    desenharMundoJogo( mj );

    mj->lj.tempoDepois = SDL_GetTicks();
    
    return SDL_APP_CONTINUE;

}*/

MundoJogo *criarMundoJogo( void ) {

    MundoJogo *novoMJ = (MundoJogo*) malloc( sizeof( MundoJogo ) );

    *novoMJ = (MundoJogo) {0};

    novoMJ->lj = (LoopJogo) {
        .fps = FPS,
        .atrasoQuadro = 1000 / FPS,
        .deltaBase = 1.0f / FPS,
        .tempoAntes = 0,
        .tempoDepois = 0,
        .tempoQuadro = 0,
        .delta = 0,
    };

    novoMJ->quantidadeMaximaBolinhas = 100;
    novoMJ->quantidadeBolinhas = 0;
    novoMJ->bolinhas = (Bolinha**) malloc( sizeof( Bolinha* ) * novoMJ->quantidadeMaximaBolinhas );

    adicionarBolinhaMundoJogo( 
        novoMJ, 
        LARGURA_JANELA / 2 - 50, 
        ALTURA_JANELA / 2 - 50, 
        50, 
        200, 
        200
    );

    novoMJ->gravidade = GRAVIDADE;

    return novoMJ;

}

void destruirMundoJogo( MundoJogo *mj ) {
    for ( int i = 0; i < mj->quantidadeBolinhas; i++ ) {
        destruirBolinha( mj->bolinhas[i] );
    }
    free( mj->bolinhas );
    free( mj );
}

void tratarEventosMundoJogo( MundoJogo *mj, SDL_Event *event ) {

    if ( event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_RIGHT ) {
        adicionarBolinhaMundoJogo( 
            mj, 
            event->button.x, 
            event->button.y, 
            20 + SDL_rand( 30 ), 
            ( 100 + SDL_rand( 100 ) ) * ( SDL_rand( 2 ) == 0 ? -1 : 1 ),
            ( 100 + SDL_rand( 100 ) ) * ( SDL_rand( 2 ) == 0 ? -1 : 1 )
        );
    }

    if ( event->type == SDL_EVENT_MOUSE_WHEEL ) {
        mj->gravidade += 100 * event->wheel.y;
    }

    for ( int i = mj->quantidadeBolinhas - 1; i >= 0; i-- ) {
        tratarEventosBolinha( mj->bolinhas[i], mj, event );
    }

}

void atualizarMundoJogo( MundoJogo *mj, float delta ) {

    for ( int i = 0; i < mj->quantidadeBolinhas; i++ ) {
        atualizarBolinha( mj->bolinhas[i], mj->gravidade, delta );
    }
    
}

void desenharMundoJogo( MundoJogo *mj ) {

    // "buffer" de desenho
    SDL_SetRenderDrawColor( mj->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    SDL_RenderClear( mj->renderer );

    for ( int i = 0; i < mj->quantidadeBolinhas; i++ ) {
        desenharBolinha( mj->bolinhas[i], mj->renderer );
    }
    
    SDL_SetRenderDrawColor( mj->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderDebugTextFormat( mj->renderer, 10, 10, "FPS: %.3f", 1.0f / mj->lj.delta );
    SDL_RenderDebugTextFormat( mj->renderer, 10, 30, "Delta: %.3f", mj->lj.delta );
    SDL_RenderDebugTextFormat( mj->renderer, 10, 50, "Gravidade: %.2f", mj->gravidade );

    // desenha
    SDL_RenderPresent( mj->renderer );

}

void adicionarBolinhaMundoJogo( MundoJogo *mj, float x, float y, float raio, float vx, float vy ) {
    if ( mj->quantidadeBolinhas < mj->quantidadeMaximaBolinhas ) {
        mj->bolinhas[mj->quantidadeBolinhas] = criarBolinha( x, y, raio, vx, vy );
        (mj->quantidadeBolinhas)++;
    }
}

Bolinha* criarBolinha( float x, float y, float raio, float vx, float vy ) {

    Bolinha *novaBolinha = (Bolinha*) malloc( sizeof( Bolinha ) );

    *novaBolinha = (Bolinha) {
        .x = x,
        .y = y,
        .raio = raio,
        .cor = {
            .r = SDL_rand( 256 ),
            .g = SDL_rand( 256 ),
            .b = SDL_rand( 256 ),
            .a = SDL_ALPHA_OPAQUE
        },
        .vx = vx,
        .vy = vy,
        .atrito = 0.99,
        .elasticidade = 0.9,
        .arrastando = false,
        .xa = 0,
        .ya = 0
    };

    return novaBolinha;

}

void destruirBolinha( Bolinha *bolinha ) {
    free( bolinha );
}

void tratarEventosBolinha( Bolinha *bolinha, MundoJogo *mj, SDL_Event *event ) {

    if ( mj->bolinhaEmArraste == NULL ) {

        if ( event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT ) {

            float x = event->button.x;
            float y = event->button.y;

            float c1 = x - bolinha->x;
            float c2 = y - bolinha->y;

            if ( c1 * c1 + c2 * c2 <= bolinha->raio * bolinha->raio ) {
                bolinha->arrastando = true;
                bolinha->xOffset = c1;
                bolinha->yOffset = c2;
                mj->bolinhaEmArraste = bolinha;
            }

        }

    }

    if ( event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT ) {
        bolinha->arrastando = false;
        mj->bolinhaEmArraste = NULL;
    }

    if ( event->type == SDL_EVENT_MOUSE_MOTION ) {

        if ( bolinha->arrastando ) {

            bolinha->x = event->button.x - bolinha->xOffset;
            bolinha->y = event->button.y - bolinha->yOffset;

            bolinha->vx = ( bolinha->x - bolinha->xa ) / mj->lj.delta;
            bolinha->vy = ( bolinha->y - bolinha->ya ) / mj->lj.delta;

            bolinha->xa = bolinha->x;
            bolinha->ya = bolinha->y;

        }

    }

}

void atualizarBolinha( Bolinha *bolinha, float gravidade, float delta ) {
    
    if ( !bolinha->arrastando ) {

        bolinha->x += bolinha->vx * delta;
        bolinha->y += bolinha->vy * delta;

        if ( bolinha->x + bolinha->raio > LARGURA_JANELA ) {
            bolinha->x = LARGURA_JANELA - bolinha->raio;
            bolinha->vx = -bolinha->vx * bolinha->elasticidade;
        } else if ( bolinha->x - bolinha->raio < 0 ) {
            bolinha->x = bolinha->raio;
            bolinha->vx = -bolinha->vx * bolinha->elasticidade;
        }

        if ( bolinha->y + bolinha->raio > ALTURA_JANELA ) {
            bolinha->y = ALTURA_JANELA - bolinha->raio;
            bolinha->vy = -bolinha->vy * bolinha->elasticidade;
        } else if ( bolinha->y - bolinha->raio < 0 ) {
            bolinha->y = bolinha->raio;
            bolinha->vy = -bolinha->vy * bolinha->elasticidade;
        }

        bolinha->vx = bolinha->vx * bolinha->atrito;
        bolinha->vy = bolinha->vy * bolinha->atrito + gravidade * delta;

    }
    
}

void desenharBolinha( Bolinha *bolinha, SDL_Renderer *renderer ) {

    filledCircleRGBA( renderer, bolinha->x, bolinha->y, bolinha->raio, bolinha->cor.r, bolinha->cor.g, bolinha->cor.b, bolinha->cor.a );

    /*SDL_SetRenderDrawColor( renderer, bolinha->cor.r, bolinha->cor.g, bolinha->cor.b, bolinha->cor.a );

    // má performance, apenas para teste
    for ( int i = 0; i < 360; i++ ) {
        SDL_RenderLine( 
            renderer, 
            bolinha->x, 
            bolinha->y,
            bolinha->x + bolinha->raio * cos( DEG2RAD * i ),
            bolinha->y + bolinha->raio * sin( DEG2RAD * i )
        );
    }*/

}

// inicialização
SDL_AppResult SDL_AppInit( void **appState, int argc, char *argv[] ) {

    mj = criarMundoJogo();
    *appState = mj;

    if ( !SDL_Init( SDL_INIT_VIDEO ) ) {
        SDL_Log( "Erro ao inicializar a SDL: %s", SDL_GetError() );
        return SDL_APP_FAILURE;
    }

    if ( !SDL_CreateWindowAndRenderer( "Testes SDL - Bolinha", LARGURA_JANELA, ALTURA_JANELA, SDL_WINDOW_RESIZABLE, &mj->janela, &mj->renderer ) ) {
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
