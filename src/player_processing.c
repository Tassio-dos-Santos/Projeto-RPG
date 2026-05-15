#include "player_processing.h"

// Variáveis globais alocadas em main
extern char **board;
extern d_linked_list_t *enemyList, *itemList, *obstacleList;
extern pthread_mutex_t board_mutex;

// Declaração de funções internas
static status_t coletar_item(character_t *personagem, d_linked_list_t *item_list, position_t item_position);
static status_t bola_de_fogo(character_t *personagem, char direcao);
static status_t relampago(character_t *personagem, char direcao);
static status_t feitico_cura(character_t *personagem);

// Definição de funções
// Funções de inicialização
// Cria um personagem na posição especificada
// Retorna o endereço para o personagem em caso de sucesso e NULL em caso de fracasso
character_t *criar_personagem(position_t position){
    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return NULL;
    }

    character_t *personagem_novo = (character_t *) malloc(sizeof(character_t));

    if(personagem_novo == NULL){
        LOG_ERROR("Couldn't allocate memory");
        return NULL;
    }

    personagem_novo->position = position;
    personagem_novo->mana_points = 0;
    personagem_novo->life_points = INITIAL_HP;

    pthread_mutex_lock(&board_mutex);
    // Põe o player no tabuleiro
    board[position.y][position.x] = 'P';
    pthread_mutex_unlock(&board_mutex);

    return personagem_novo;
}

// Funções de ações do personagem
// Processa o movimento do player
status_t mover_personagem(character_t *personagem, char direcao){
    if(!(direcao == 'W' || direcao == 'A' || direcao == 'S' || direcao == 'D')){
        LOG_ERROR("Invalid input");
        return ERR_INVALID_IN;
    }

    position_t next_position = personagem->position;
    switch (direcao)
    {
    case 'W':
        next_position.y++;
        break;
    
    case 'A':
        next_position.x--;
        break;
    
    case 'S':
        next_position.y--;
        break;
    
    case 'D':
        next_position.x++;
        break;
    
    default:
        break;
    }

    // Impede que o player saia dos limites do tabuleiro
    if(is_position_valid(next_position) == false) return ACTION_SKIPPED;

    // Detecção de colisão
    switch (board[next_position.y][next_position.x]){
    case '-':
        break;
    
    case 'E':
    {
        status_t resultado_combate = combate(personagem, next_position, CHARACTER);

        // Player morreu
        if(resultado_combate == ACTION_SKIPPED){
            return ACTION_SKIPPED;
        }

        // Inimigo não morreu
        else if(resultado_combate == PARTIAL_SUCCESS){
            return PARTIAL_SUCCESS;
        }
        
        // Caso só inimigo morreu, continua
        else if(resultado_combate == SUCCESS){
            break;
        }

        else if(IS_ERROR_STATUS(resultado_combate)){
            LOG_ERROR("Couldn't process combat");
            return ERR_INTERNAL;
        }
    }
    case 'I':
    {
        if(IS_ERROR_STATUS(coletar_item(personagem, itemList, next_position))){
            LOG_ERROR("Couldn't process item colection");
            return ERR_INTERNAL;
        }

        break;
    }
    case 'X':
    {
        #ifdef DEBUG
        d_linked_node_t *no_obstaculo = search_position_d_linked_list(obstacleList, next_position);
        if(no_obstaculo == NULL){
            LOG_ERROR("Couldn't get obstacle in obstacle list");
            return ERR_DATA;
        }
        obstacle_t obstaculo = no_obstaculo->data.obstaculo;
        print_obstacle(obstaculo, stdout);
        #endif
    }
    case 'P':
    default:
        return ACTION_SKIPPED;
        break;
    }

    // Se chegou até essa parte do código (não passou por return), o movimento é executado

    personagem->position = next_position;

    if(IS_ERROR_STATUS(atualizar_posicoes())){
        LOG_ERROR("Couldn't update positions");
        return ERR_INTERNAL;
    }

    log_move(*player);
    return SUCCESS;
}

// Processa a coleta de um item
static status_t coletar_item(character_t *personagem, d_linked_list_t *item_list, position_t item_position){
    if(is_position_valid(item_position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    if(board[item_position.y][item_position.x] != 'I'){
        LOG_ERROR("No item on the assigned position");
        return ERR_INVALID_DST;
    }

    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    d_linked_node_t* no_item = search_position_d_linked_list(item_list, item_position);
    if(no_item == NULL){
        LOG_ERROR("Couldn't find item in the list");
        return ERR_DATA;
    }
    item_t item = no_item->data.item;
    if(IS_ERROR_STATUS(remove_d_linked_list(item_list, no_item))){
        LOG_ERROR("Couldn't remove item from list");
        return ERR_DATA;
    }

    personagem->mana_points += item.valor;
    log_item_collected(*player, item);

    #ifdef DEBUG
    print_list(item_list, stdout);
    #endif

    return SUCCESS;
}

// Processa o uso de uma habilidade
status_t usar_habilidade(character_t *personagem, skill_t skill, char direcao){
    status_t result;
    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    switch (skill)
    {
    case HEALING_SPELL:
        if(personagem->mana_points >= HEALING_SPELL_COST){
            personagem->mana_points -= HEALING_SPELL_COST;

            if(IS_ERROR_STATUS(feitico_cura(personagem))) return ERR_INTERNAL;

            return SUCCESS;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", HEALING_SPELL_COST);
            result = ACTION_SKIPPED;
        }
        break;
    
    case LIGHTNING:
        if(personagem->mana_points >= LIGHTNING_SPELL_COST){
            personagem->mana_points -= LIGHTNING_SPELL_COST;

            if(IS_ERROR_STATUS(relampago(personagem, direcao))) return ERR_INTERNAL;

            result = SUCCESS;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", LIGHTNING_SPELL_COST);
            result = ACTION_SKIPPED;
        }
        break;
    
    case FIREBALL:
        if(personagem->mana_points >= FIREBALL_SPELL_COST){
            personagem->mana_points -= FIREBALL_SPELL_COST;
            if(IS_ERROR_STATUS(bola_de_fogo(personagem, direcao))) return ERR_INTERNAL;
            result = SUCCESS;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", FIREBALL_SPELL_COST);
            result = ACTION_SKIPPED;
        }
        break;
    
    default:
        LOG_ERROR("Invalid skill");
        return ERR_INVALID_IN;
        break;
    }

    Sleep(1000);
    return result;
}

// Processa o feitiço de bola de fogo
static status_t bola_de_fogo(character_t *personagem, char direcao){
    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    position_t player_position = personagem->position;
    position_t current_position = {0};

    for(int j = 1; j <= 2; j++){
    for(int i = -1; i <= 1; i++){
        switch (direcao)
        {
        case 'W':
            current_position.x = player_position.x + i;
            current_position.y = player_position.y + j;
            break;
        
        case 'A':
            current_position.x = player_position.x - j;
            current_position.y = player_position.y + i;
            break;
        
        case 'S':
            current_position.x = player_position.x + i;
            current_position.y = player_position.y - j;
            break;
        
        case 'D':
            current_position.x = player_position.x + j;
            current_position.y = player_position.y + i;
            break;
        
        default:
            return ACTION_SKIPPED;
            break;
        }
        
        if(is_position_valid(current_position) == false){
            break;
        }

        switch (board[current_position.y][current_position.x])
        {
        case '-':
            
            break;
        
        case 'E':
        {
            if(IS_ERROR_STATUS(dano_na_entidade(FIREBALL_SPELL_DAMAGE, current_position))){
                LOG_ERROR("Couldn't process the damage");
                return ERR_INTERNAL;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(IS_ERROR_STATUS(destruir_entidade(current_position))){
                LOG_ERROR("Couldn't destroy the entity");
                return ERR_INTERNAL;
            }

            break;
        }
        default:
            break;
        }

        pthread_mutex_lock(&board_mutex);
        board[current_position.y][current_position.x] = '@';
        pthread_mutex_unlock(&board_mutex);
    }
    }

    log_skill(*player, FIREBALL);

    mostrar_tabuleiro();

    return SUCCESS;
}

// Processa o feitiço de relâmpago
static status_t relampago(character_t *personagem, char direcao){
    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    position_t player_position = personagem->position;
    position_t current_position = {0};
    for(int i = 1; i <= 3; i++){
        switch (direcao)
        {
        case 'W':
            current_position.x = player_position.x;
            current_position.y = player_position.y + i;
            break;
        
        case 'A':
            current_position.x = player_position.x - i;
            current_position.y = player_position.y;
            break;
        
        case 'S':
            current_position.x = player_position.x;
            current_position.y = player_position.y - i;
            break;
        
        case 'D':
            current_position.x = player_position.x + i;
            current_position.y = player_position.y;
            break;
        
        default:
            return ACTION_SKIPPED;
            break;
        }
        
        if(is_position_valid(current_position) == false){
            break;
        }

        switch (board[current_position.y][current_position.x])
        {
        case '-':
            
            break;
        
        case 'E':
        {
            if(IS_ERROR_STATUS(dano_na_entidade(LIGHTNING_SPELL_DAMAGE, current_position))){
                LOG_ERROR("Couldn't process the damage");
                return ERR_INTERNAL;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(IS_ERROR_STATUS(destruir_entidade(current_position))){
                LOG_ERROR("Couldn't destroy the entity");
                return ERR_INTERNAL;
            }

            break;
        }
        default:
            break;
        }

        pthread_mutex_lock(&board_mutex);
        board[current_position.y][current_position.x] = '~';
        pthread_mutex_unlock(&board_mutex);
    }

    log_skill(*player, LIGHTNING);

    mostrar_tabuleiro();

    return SUCCESS;
}

// Processa o feitiço de cura
static status_t feitico_cura(character_t *personagem){
    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    personagem->life_points += HEALING_SPELL_HEALING;

    log_skill(*player, HEALING_SPELL);

    return SUCCESS;
}
