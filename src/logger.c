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
        node_data_t data_log = dequeue(logQueue);

        // Devolve o mutex da log queue
        pthread_mutex_unlock(&log_queue_mutex);

        // Verifica se o dado está vazio
        if(is_data_empty(data_log) == 1) continue;

        // Se o dado não está vazio, extrai o evento
        log_t log_msg = data_log.log;

        // Pega o horário atual e transforma em string
        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        char timeString[80];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", local);

        // Escreve o horário no log
        if(fprintf(log_file, "%s ", timeString) == -1){
            fputs("ERROR - thread logger: Couldn't write log\n", stderr);
            return NULL;
        }

        // E printa o evento
        if(IS_ERROR_STATUS(print_log(log_msg, log_file))){
            fputs("ERROR - thread logger: Couldn't write log\n", stderr);
            return NULL;
        }
    }

    fclose(log_file);
}

// Loga um movimento num arquivo de log especificado
status_t log_move(action_t m){
    // Cria o log
    log_t log_msg;

    snprintf(log_msg.text, LOG_LENGTH, "Player se moveu para a posição [%d, %d]\n\n", player->position.x, player->position.y);

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_move: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Loga um item coletado num arquivo de log especificado
status_t log_item_collected(item_t item){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "Player coletou o item na posicao: [%d, %d]\nMana atual do player: %d\tValor do item: %d\n\n",
        item.position.x, item.position.y, player->mana_points, item.valor);

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
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
    // Cria o log
    log_t log_msg;
    
    if(main_entity_type == CHARACTER){
        character_t jogador = main_entity.character;
        enemy_t inimigo = secundary_entity.enemy;

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "Player combateu o inimigo na posicao: [%d, %d]\nVida atual do Player: %d\tVida atual do inimigo: %d\n\n",
            inimigo.position.x, inimigo.position.y, jogador.life_points, inimigo.life_points
        );
    }

    else if(main_entity_type == ENEMY){
        character_t jogador = main_entity.character;
        enemy_t inimigo = secundary_entity.enemy;

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "O inimigo combateu o player na posicao: [%d, %d]\nVida atual do Player: %d\tVida atual do inimigo: %d\n\n",
            jogador.position.x, jogador.position.y, jogador.life_points, inimigo.life_points
        );
    }

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
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
    // Cria o log
    log_t log_msg;

    if(main_entity_type == CHARACTER){
        character_t jogador = main_entity.character;

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "Player levou %d de dano\nVida atual do Player: %d\tMana atual do Player: %d\n\n",
            damage, jogador.life_points, jogador.mana_points
        );
    }

    else if (main_entity_type == ENEMY){
        enemy_t inimigo = main_entity.enemy;

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "Inimigo levou %d de dano\nVida atual do inimigo: %d\n\n",
            damage, inimigo.life_points
        );
    }

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
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