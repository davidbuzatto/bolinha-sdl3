#include <stdlib.h>

#include <SDL3/SDL.h>

#include "Bolinha.h"
#include "MundoJogo.h"
#include "Tipos.h"

MundoJogo *criarMundoJogo( int fps, float gravidade ) {

    MundoJogo *novoMJ = (MundoJogo*) malloc( sizeof( MundoJogo ) );

    *novoMJ = (MundoJogo) {0};

    novoMJ->lj = (LoopJogo) {
        .fps = fps,
        .atrasoQuadro = 1000 / fps,
        .deltaBase = 1.0f / fps,
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
        400, 
        225, 
        50, 
        200, 
        200
    );

    novoMJ->gravidade = gravidade;

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
        atualizarBolinha( mj->bolinhas[i], mj, delta );
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