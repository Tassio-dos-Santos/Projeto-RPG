#include "logger.h"

void *logger(void* argv){
    // Inicia o arquivo de log
    FILE *log_file = fopen("log/RPG.log", "w");

    while(!end_logger){
        // Tira valor do semáforo
        sem_wait(&log_queue_sem);
        if(end_logger) break;

        // Se a log queue estiver vazia
        if(logQueue->length < 1){
            // Devolve o mutex
            pthread_mutex_unlock(&log_queue_mutex);

            // E volta ao início do loop
            continue;
        }

        // Pega o mutex da log queue
        pthread_mutex_lock(&log_queue_mutex);
        if(end_logger) break;

        // Pega o dado da frente da fila
        node_data_t dataEvento = dequeue(logQueue);

        // Devolve o mutex da log queue
        pthread_mutex_unlock(&log_queue_mutex);

        // Verifica se o dado está vazio
        if(is_data_empty(dataEvento) == 1) continue;

        // Se o dado não está vazio, extrai o evento
        event_t evento = dataEvento.evento;

        // Pega o horário atual e transforma em string
        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        char timeString[80];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", local);

        // Escreve o horário no log
        if(fprintf(log_file, "%s ", timeString) == -1){
            fputs("ERROR - function log_move: Couldn't write log\n", stderr);
            fflush(stderr);
            return NULL;
        }

        // E printa o evento
        if(IS_ERROR_STATUS(print_event(evento, log_file))){
            fputs("ERROR - thread logger: Couldn't write log\n", stderr);
            fflush(stderr);
            return NULL;
        }
    }

    fclose(log_file);
}

// Loga um movimento num arquivo de log especificado
status_t log_move(action_t m){
    // Cria o evento
    event_t movimento = {
        .event_type = MOVE,
        .main_entity = (entity_t) *player,
        .main_entity_type = CHARACTER
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) movimento))){
        fputs("ERROR - function log_move: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return 1;
}

// Loga um item coletado num arquivo de log especificado
status_t log_item_collected(item_t item){
    event_t item_coletado = {
        .event_type = ITEM_COLLECTED,
        .main_entity = (entity_t) *player,
        .main_entity_type = CHARACTER,
        .secundary_entity = (entity_t) item,
        .secundary_entity_type = ITEM,
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) item_coletado))){
        fputs("ERROR - function log_item_collected: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Loga um combate num arquivo de log especificado
status_t log_combat(entity_t main_entity, entity_t secundary_entity, entity_type_t main_entity_type){
    event_t combate = {
        .event_type = COMBAT,
        .main_entity = main_entity,
        .main_entity_type = main_entity_type,
        .secundary_entity = secundary_entity
    };

    // Define o tipo de entidade da entidade secundária a partir da principal
    if(main_entity_type == CHARACTER) combate.secundary_entity_type = ENEMY;
    else if(main_entity_type == ENEMY) combate.secundary_entity_type = CHARACTER;

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) combate))){
        fputs("ERROR - function log_combat: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Loga um combate num arquivo de log especificado
status_t log_damage(entity_t main_entity, entity_type_t main_entity_type, int32_t damage){
    event_t dano = {
        .event_type = COMBAT,
        .main_entity = main_entity,
        .main_entity_type = main_entity_type,
        .auxiliar_data.life_points = damage
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) dano))){
        fputs("ERROR - function log_combat: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}