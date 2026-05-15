#include "logger.h"

void *logger(void* argv){
    // Inicia o arquivo de log
    FILE *log_file = fopen("log/RPG.log", "w");
    int sem_value = 0;
    sem_getvalue(&log_queue_sem, &sem_value);

    while(!end_logger || sem_value != 0){
        // Tira valor do semáforo
        sem_wait(&log_queue_sem);
        sem_getvalue(&log_queue_sem, &sem_value);

        // Se a log queue estiver vazia
        if(logQueue->length < 1){
            // E volta ao início do loop
            continue;
        }

        // Pega o mutex da log queue
        pthread_mutex_lock(&log_queue_mutex);
        sem_getvalue(&log_queue_sem, &sem_value);

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

        sem_getvalue(&log_queue_sem, &sem_value);
    }

    fclose(log_file);
}

// Manda um log de movimento para o logger processar
status_t log_move(character_t jogador){
    // Cria o log
    log_t log_msg;

    snprintf(log_msg.text, LOG_LENGTH, "[INFO] Player se moveu para a posição [%d, %d]\n\n", jogador.position.x, jogador.position.y);

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

// Manda um log de coleta de item para o logger processar
status_t log_item_collected(character_t jogador, item_t item){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[INFO] Player coletou o item na posicao: [%d, %d]\nMana atual do player: %d\tValor do item: %d\n\n",
        item.position.x, item.position.y, jogador.mana_points, item.valor);

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

// Manda um log de combate para o logger processar
status_t log_combat(entity_t main_entity, entity_t secundary_entity){
    // Cria o log
    log_t log_msg;
    
    if(main_entity.type == CHARACTER && secundary_entity.type == ENEMY){
        character_t jogador = *(main_entity.data.character);
        enemy_t inimigo = *(secundary_entity.data.enemy);

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "[INFO] Player combateu o inimigo na posicao: [%d, %d]\nVida atual do Player: %d\tVida atual do inimigo: %d\n\n",
            inimigo.position.x, inimigo.position.y, jogador.life_points, inimigo.life_points
        );
    }

    else if(main_entity.type == ENEMY && secundary_entity.type == CHARACTER){
        character_t jogador = *(secundary_entity.data.character);
        enemy_t inimigo = *(main_entity.data.enemy);

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "[INFO] O inimigo combateu o player na posicao: [%d, %d]\nVida atual do Player: %d\tVida atual do inimigo: %d\n\n",
            jogador.position.x, jogador.position.y, jogador.life_points, inimigo.life_points
        );
    }

    else{
        LOG_ERROR("Invalid entity types");
        return ERR_INVALID_IN;
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

// Manda um log de uso de habilidade para o logger processar
status_t log_skill(character_t jogador, skill_t skill){
    // Cria o log
    log_t log_msg;

    char skill_string[64] = {0};
    switch (skill)
    {
    case HEALING_SPELL:
        strcpy(skill_string, "feitico de cura\0");
        break;
    
    case LIGHTNING:
        strcpy(skill_string, "feitico de relampago\0");
        break;
    
    case FIREBALL:
        strcpy(skill_string, "feitico de bola de fogo\0");
        break;
    
    default:
        break;
    }
    
    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[INFO] Player usou a habilidade %s\nVida atual do Player: %d\tMana atual do Player: %d\n\n",
        skill_string, jogador.life_points, jogador.mana_points
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_skill: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de dano para o logger processar
status_t log_damage(entity_t main_entity, int32_t damage){
    // Cria o log
    log_t log_msg;

    if(main_entity.type == CHARACTER){
        character_t jogador = *(main_entity.data.character);

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "[INFO] Player levou %d de dano\nVida atual do Player: %d\tMana atual do Player: %d\n\n",
            damage, jogador.life_points, jogador.mana_points
        );
    }

    else if (main_entity.type == ENEMY){
        enemy_t inimigo = *(main_entity.data.enemy);

        snprintf(
            log_msg.text, LOG_LENGTH, 
            "[INFO] Inimigo levou %d de dano\nVida atual do inimigo: %d\n\n",
            damage, inimigo.life_points
        );
    }

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_damage: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de vitória para o logger processar
status_t log_victory(character_t jogador){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[INFO] Player venceu!!!\nVida atual do Player: %d\tMana atual do Player: %d\n\n",
        jogador.life_points, jogador.mana_points
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_victory: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de derrota para o logger processar
status_t log_defeat(character_t jogador){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[INFO] Player perdeu.\nVida atual do Player: %d\tMana atual do Player: %d\n\n",
        jogador.life_points, jogador.mana_points
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_defeat: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de erro para o logger processar
status_t log_error(const char* file_name, uint32_t line, const char* function_name, const char* error_text){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[ERROR] %s:%d (%s): %s\n\n",
        file_name, line, function_name, error_text
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_error: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de warning para o logger processar
status_t log_warning(const char* file_name, uint32_t line, const char* function_name, const char* warning_text){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[WARNING] %s:%d (%s): %s\n\n",
        file_name, line, function_name, warning_text
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_error: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}

// Manda um log de informações de debug para o logger processar
status_t log_debug(const char* file_name, uint32_t line, const char* function_name, const char* debug_text){
    // Cria o log
    log_t log_msg;

    snprintf(
        log_msg.text, LOG_LENGTH, 
        "[DEBUG] %s:%d (%s): %s\n\n",
        file_name, line, function_name, debug_text
    );

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(IS_ERROR_STATUS(enqueue(logQueue, (node_data_t) log_msg))){
        fputs("ERROR - function log_error: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return SUCCESS;
}