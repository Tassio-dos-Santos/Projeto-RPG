#ifndef BOARD_PROCESSING_H
#define BOARD_PROCESSING_H

// Inclusão de bibliotecas
#ifndef JOGO
#define JOGO
#endif
#include "utils/data_structures.h"
#include "utils/status.h"
#include "logger.h"
#include <stdbool.h>
#include <pthread.h>

// Constantes
#define MAX_BOARD_SIZE 30
#define MIN_BOARD_SIZE 4

// Declaração de funções expostas
// Funções de inicialização
char** inicializar_tabuleiro(int N);
status_t gerar_itens(d_linked_list_t *item_list, int quantidade);
status_t gerar_obstaculos(d_linked_list_t *lista_obstaculo, int quantidade);
status_t adicionar_item(d_linked_list_t *lista_item, position_t position, int valor);
status_t adicionar_obstaculo(d_linked_list_t *lista_obstaculo, position_t position);

// Funções de processamento
status_t atualizar_posicoes();
status_t dano_na_entidade(int dano, position_t position);
status_t combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type);
status_t destruir_entidade(position_t position);

// Funções auxiliares
bool is_tile_player_walkable(position_t position);
bool is_tile_enemy_walkable(position_t position);
bool is_position_valid(position_t position);

#endif