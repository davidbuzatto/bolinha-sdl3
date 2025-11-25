#pragma once

#include <SDL3/SDL.h>
#include "Tipos.h"

Bolinha* criarBolinha( float x, float y, float raio, float vx, float vy );
void destruirBolinha( Bolinha *bolinha );
void tratarEventosBolinha( Bolinha *bolinha, MundoJogo *mj, SDL_Event *event );
void atualizarBolinha( Bolinha *bolinha, MundoJogo *mj, float delta );
void desenharBolinha( Bolinha *bolinha, SDL_Renderer *renderer );
