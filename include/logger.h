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
status_t log_move(action_t m);
status_t log_item_collected(item_t i);
status_t log_combat(entity_t main_entity, entity_t secundary_entity, entity_type_t main_entity_type);
status_t log_damage(entity_t main_entity, entity_type_t main_entity_type, int32_t damage);

// Funções de log para erros e avisos


// Funções de log para debug


// Macros para erros, avisos e debug

#endif