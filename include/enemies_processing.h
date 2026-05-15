#ifndef ENEMIES_PROCESSING_H
#define ENEMIES_PROCESSING_H

// Inclusão de bibliotecas
#ifndef JOGO
#define JOGO
#endif
#include "utils\data_structures.h"
#include "utils\status.h"
#include "logger.h"
#include "board_processing.h"
#include <math.h>

// Constantes
#define ENEMY_VIEW_RADIUS 3.5

// Declaração de funções expostas
// Funções de inicialização
status_t gerar_inimigos(d_linked_list_t *enemy_list, int quantidade);
status_t adicionar_inimigo(d_linked_list_t *lista_inimigo, position_t position);

// Funções de processamento
status_t mover_inimigos(d_linked_list_t *enemy_list);
status_t mover_inimigo(d_linked_list_t *enemy_list, int index_inimigo);

#endif