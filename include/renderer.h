#ifndef RENDERER_H
#define RENDERER_H

#ifndef JOGO
#define JOGO
#endif
#include "data_structures.h"

#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <windows.h>

// Variáveis globais
extern char** board;
extern int boardSize;
extern character_t* player;

// Sinais para terminar as threads
extern volatile sig_atomic_t end_renderer;

// Sinais para sincronizar as threads
extern volatile sig_atomic_t display_updated;

// Mutexes
extern pthread_mutex_t board_mutex;

// Semáforos
extern sem_t render_sem;

// Funções de renderização
void *renderer();
void render_board();
void mostrar_tabuleiro();

#endif