#pragma once

#include <SDL3/SDL.h>
#include "Tipos.h"

MundoJogo *criarMundoJogo( int fps, float gravidade );
void destruirMundoJogo( MundoJogo *mj );
void tratarEventosMundoJogo( MundoJogo *mj, SDL_Event *event );
void atualizarMundoJogo( MundoJogo *mj, float delta );
void desenharMundoJogo( MundoJogo *mj );
void adicionarBolinhaMundoJogo( MundoJogo *mj, float x, float y, float raio, float vx, float vy );
