#ifndef PLAYER_PROCESSING_H
#define PLAYER_PROCESSING_H

// Inclusão de bibliotecas
#ifndef JOGO
#define JOGO
#endif
#include "utils/data_structures.h"
#include "utils/status.h"
#include "board_processing.h"
#include "logger.h"
#include "renderer.h"
#include <pthread.h>
#include <windows.h>

// Constantes
#define INITIAL_HP 100
#define HEALING_SPELL_COST 40
#define HEALING_SPELL_HEALING 20
#define LIGHTNING_SPELL_COST 100
#define LIGHTNING_SPELL_DAMAGE 50
#define FIREBALL_SPELL_COST 150
#define FIREBALL_SPELL_DAMAGE 50

// Declaração de funções expostas
// Funções de inicialização
character_t *criar_personagem(position_t position);

// Funções de ações do personagem
status_t mover_personagem(character_t *personagem, char direcao);
status_t usar_habilidade(character_t *personagem, skill_t skill, char direcao);

#endif