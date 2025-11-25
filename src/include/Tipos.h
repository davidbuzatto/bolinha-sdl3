#pragma once

#include <SDL3/SDL.h>

typedef struct LoopJogo {

    Uint64 fps;
    Uint64 atrasoQuadro;
    float deltaBase;
    float delta;

    Uint64 tempoAntes;
    Uint64 tempoDepois;
    Uint64 tempoQuadro;

} LoopJogo;

typedef struct Bolinha {

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

typedef struct MundoJogo {

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