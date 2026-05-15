#ifndef LOGGER_H
#define LOGGER_H

#ifndef JOGO
#define JOGO
#endif
#include "utils\data_structures.h"
#include "utils\status.h"

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>

 // Variáveis globais
extern character_t* player;

// A fila de logs
extern queue_t *logQueue;

// O sinal de encerramento da thread
extern volatile sig_atomic_t end_logger;

// Mutex para garantir que não haverá race condition na fila de logs
extern pthread_mutex_t log_queue_mutex;

// Semáforo para coordenar a relação produtor-consumidor de logs
extern sem_t log_queue_sem;

// Funções do logger
// Thread do logger
void *logger(void* argv);

// Funções de log para eventos
/*
    MOVE,
    COMBAT,
    ITEM_COLLECTED,
    SKILL,
    DAMAGE,
    VICTORY,
    DEFEAT
*/
status_t log_move(character_t jogador);
status_t log_combat(entity_t main_entity, entity_t secundary_entity);
status_t log_item_collected(character_t jogador, item_t i);
status_t log_skill(character_t jogador, skill_t skill);
status_t log_damage(entity_t main_entity, int32_t damage);
status_t log_victory(character_t jogador);
status_t log_defeat(character_t jogador);

// Funções de log para erros e avisos
status_t log_error(const char* file_name, uint32_t line, const char* function_name, const char* error_text);
status_t log_warning(const char* file_name, uint32_t line, const char* function_name, const char* warning_text);

// Funções de log para debug
status_t log_debug(const char* file_name, uint32_t line, const char* function_name, const char* debug_text);

// Macros para erros, avisos e debug
#define LOG_ERROR(error_text) log_error(__FILE__, __LINE__, __FUNCTION__, error_text)
#define LOG_WARNING(warning_text) log_warning(__FILE__, __LINE__, __FUNCTION__, warning_text)
#define LOG_DEBUG(debug_text) log_debug(__FILE__, __LINE__, __FUNCTION__, debug_text)

#endif