#include <stdlib.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "SDL3_gfxPrimitives.h"

#include "Bolinha.h"
#include "Tipos.h"

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
        .atrito = 0.99f,
        .elasticidade = 0.9f,
        .arrastando = false,
        .xa = 0.0f,
        .ya = 0.0f
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

            bolinha->x = event->motion.x - bolinha->xOffset;
            bolinha->y = event->motion.y - bolinha->yOffset;

            bolinha->vx = ( bolinha->x - bolinha->xa ) / mj->lj.delta;
            bolinha->vy = ( bolinha->y - bolinha->ya ) / mj->lj.delta;

            bolinha->xa = bolinha->x;
            bolinha->ya = bolinha->y;

        }

    }

}

void atualizarBolinha( Bolinha *bolinha, MundoJogo *mj, float delta ) {
    
    int larguraJanela;
    int alturaJanela;
    SDL_GetWindowSize( mj->janela, &larguraJanela, &alturaJanela );

    if ( !bolinha->arrastando ) {

        bolinha->x += bolinha->vx * delta;
        bolinha->y += bolinha->vy * delta;

        if ( bolinha->x + bolinha->raio > larguraJanela ) {
            bolinha->x = larguraJanela - bolinha->raio;
            bolinha->vx = -bolinha->vx * bolinha->elasticidade;
        } else if ( bolinha->x - bolinha->raio < 0 ) {
            bolinha->x = bolinha->raio;
            bolinha->vx = -bolinha->vx * bolinha->elasticidade;
        }

        if ( bolinha->y + bolinha->raio > alturaJanela ) {
            bolinha->y = alturaJanela - bolinha->raio;
            bolinha->vy = -bolinha->vy * bolinha->elasticidade;
        } else if ( bolinha->y - bolinha->raio < 0 ) {
            bolinha->y = bolinha->raio;
            bolinha->vy = -bolinha->vy * bolinha->elasticidade;
        }

        bolinha->vx = bolinha->vx * bolinha->atrito;
        bolinha->vy = bolinha->vy * bolinha->atrito + mj->gravidade * delta;

    }
    
}

void desenharBolinha( Bolinha *bolinha, SDL_Renderer *renderer ) {
    filledCircleRGBA( 
        renderer, 
        bolinha->x, 
        bolinha->y, 
        bolinha->raio, 
        bolinha->cor.r, 
        bolinha->cor.g, 
        bolinha->cor.b, 
        bolinha->cor.a
    );
}