#include "board_processing.h"

// Variáveis globais alocadas em main
extern char **board;
extern int boardSize;
extern character_t *player;
extern d_linked_list_t *enemyList, *itemList, *obstacleList;
extern pthread_mutex_t board_mutex;

// Declaração de funções internas
static status_t atualizar_posicoes_de_lista(d_linked_list_t *lista);

// Definição de funções
// Funções de inicialização
char** inicializar_tabuleiro(int N){
    char** tabuleiro = (char**) calloc(N, sizeof(char*));
    if(tabuleiro == NULL){
        LOG_ERROR("Couldn't allocate memory");
        return NULL;
    }

    for(int i = 0; i < N; i++){
        tabuleiro[i] = (char*) calloc(N, sizeof(char));
        if(tabuleiro[i] == NULL){
            LOG_ERROR("Couldn't allocate memory");
            return NULL;
        }
        for(int j = 0; j < N; j++){
            tabuleiro[i][j] = '-';
        }
    }

    return tabuleiro;
}

// Gera itens em posições aleatórias na lista de itens
status_t gerar_itens(d_linked_list_t *item_list, int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t item_position = {0};

        int valor = (rand() + 5) % 101;

        // Gera novas posições, até achar uma desocupada
        do{
            item_position.x = rand() % (boardSize);
            item_position.y = rand() % (boardSize);
        }while(board[item_position.y][item_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_item(item_list, item_position, valor))){
            LOG_ERROR("Couldn't add item to the list");
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif

    return SUCCESS;
}

// Gera obstáculos em posições aleatórias na lista de obstáculos
status_t gerar_obstaculos(d_linked_list_t *lista_obstaculo, int quantidade){
    position_t obstacle_position = {0};

    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
            obstacle_position.x = rand() % (boardSize);
            obstacle_position.y = rand() % (boardSize);
        }while(board[obstacle_position.y][obstacle_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_obstaculo(lista_obstaculo, obstacle_position))){
            LOG_ERROR("Couldn't add obstacle to the list");
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(obstacleList, stdout);
    #endif

    return SUCCESS;
}

// Adiciona um item com valor especificado na posição especificada na lista de itens
status_t adicionar_item(d_linked_list_t *lista_item, position_t position, int valor){
    if(lista_item == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    item_t new_item = {
        .valor = valor,
        .position = position
    };

    if(IS_ERROR_STATUS(insert_d_linked_list(lista_item, (node_data_t) new_item, NULL))){
        LOG_ERROR("Couldn't add item to the list");
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'I';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Adiciona um obstaculo posição especificada na lista de obstaculos
status_t adicionar_obstaculo(d_linked_list_t *lista_obstaculo, position_t position){
    if(lista_obstaculo == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    obstacle_t new_obstacle = {
        .position = position
    };

    if(IS_ERROR_STATUS(insert_d_linked_list(lista_obstaculo, (node_data_t) new_obstacle, NULL))){
        LOG_ERROR("Couldn't add obstacle to the list");
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'X';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Funções de processamento

// Atualiza o tabuleiro (matriz de caracteres) de acordo com as posições das entidades nas listas
status_t atualizar_posicoes(){
    pthread_mutex_lock(&board_mutex);

    // Reseta o tabuleiro
    for(int i = 0; i < boardSize; i++){
        for(int j = 0; j < boardSize; j++){
            board[i][j] = '-';
        }
    }

    // Põe o player no tabuleiro
    board[player->position.y][player->position.x] = 'P';
    
    // Põe os inimigos no tabuleiro
    if(IS_ERROR_STATUS(atualizar_posicoes_de_lista(enemyList))){
        LOG_ERROR("Couldn't update enemies' position");
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    // Põe os itens no tabuleiro
    if(IS_ERROR_STATUS(atualizar_posicoes_de_lista(itemList))){
        LOG_ERROR("Couldn't update items' position");
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    // Põe os obstáculos no tabuleiro
    if(IS_ERROR_STATUS(atualizar_posicoes_de_lista(obstacleList))){
        LOG_ERROR("Couldn't update obstacles' position");
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Atualiza o tabuleiro (matriz de caracteres) de acordo com as posições das entidades em uma lista específica
static status_t atualizar_posicoes_de_lista(d_linked_list_t *lista){
    if(lista == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(lista->length < 1){
        return SUCCESS;
    }

    // Começa a percorrer a lista pela cabeça da lista
    d_linked_node_t* current_node = lista->head;
    node_data_t current_data;

    for(int i = 0; i < lista->length; i++){
        if(current_node == NULL){
            LOG_ERROR("Broken link in the list");
            return ERR_DATA;
        }

        // Define o currentData como o dado no nó atual
        current_data = current_node->data;

        position_t current_position;

        switch (lista->list_type){
        case CHARACTER_TYPE:
            current_position = current_data.personagem.position;

            // Caso o tipo da lista for de personagem, coloca 'P' na posição do personagem atual
            board[current_position.y][current_position.x] = 'P';
            break;
        
        case ENEMY_TYPE:
            current_position = current_data.personagem.position;

            // Caso o tipo da lista for de inimigo, coloca 'E' na posição do inimigo atual
            board[current_position.y][current_position.x] = 'E';
            break;
        
        case ITEM_TYPE:
            current_position = current_data.personagem.position;

            // Caso o tipo da lista for de item, coloca 'I' na posição do item atual
            board[current_position.y][current_position.x] = 'I';
            break;
        
        case OBSTACLE_TYPE:
            current_position = current_data.personagem.position;

            // Caso o tipo da lista for de obstaculo, coloca 'X' na posição do obstaculo atual
            board[current_position.y][current_position.x] = 'X';
            break;
        
        default:
            LOG_ERROR("Invalid list type");
            return ERR_INVALID_IN;
            break;
        }

        // Passa pro próximo nó
        current_node = current_node->next;
    }

    return SUCCESS;
}

// Dá o dano especificado na entidade na posição especificada
status_t dano_na_entidade(int dano, position_t position){
    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    char tile = board[position.y][position.x];

    switch (tile)
    {
    case 'P':
        if(player == NULL){
            LOG_ERROR("Player's memory address not found");
            return ERR_MEMORY;
        }

        player->life_points -= dano;

        entity_t jogador = {
            .type = CHARACTER,
            .data = (entity_data_t) player
        };

        log_damage(jogador, dano);

        return SUCCESS;
        break;
    
    case 'E':
    {
        d_linked_node_t* no_inimigo = search_position_d_linked_list(enemyList, position);
        if(no_inimigo == NULL){
            LOG_ERROR("Enemy not found on enemy list");
            return ERR_DATA;
        }

        enemy_t* inimigo = &no_inimigo->data.inimigo;
        if(inimigo == NULL){
            LOG_ERROR("Enemy's memory address not found");
            return ERR_MEMORY;
        }

        inimigo->life_points -= dano;

        entity_t i = {
            .type = ENEMY,
            .data = (entity_data_t) inimigo
        };

        log_damage(i, dano);
        
        if(inimigo->life_points < 1){
            if(IS_ERROR_STATUS(destruir_entidade(position))){
                LOG_ERROR("Couldn't destroy the entity");
                return ERR_INTERNAL;
            }
        }

        return SUCCESS;
        break;
    }
    default:
        break;
    }

    return SUCCESS;
}

// Processa o combate entre o player e um inimigo
status_t combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type){
    if(is_position_valid(enemy_position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    if(board[enemy_position.y][enemy_position.x] != 'E'){
        LOG_ERROR("No enemy on the assigned position");
        return ERR_INVALID_DST;
    }

    if(personagem == NULL){
        LOG_ERROR("Invalid character");
        return ERR_INVALID_IN;
    }

    // Pega os dados do inimigo e salva localmente, para caso ele seja eliminado da lista
    d_linked_node_t *no_inimigo = search_position_d_linked_list(enemyList, enemy_position);

    if(no_inimigo == NULL){
        LOG_ERROR("Invalid node");
        return ERR_DATA;
    }
    enemy_t inimigo = no_inimigo->data.inimigo;

    // Executa o dano no inimigo na lista
    if(IS_ERROR_STATUS(dano_na_entidade(20, enemy_position))){
        LOG_ERROR("Couldn't process damage on the enemy");
        return ERR_INTERNAL;
    }

    // Dá dano no personagem
    if(IS_ERROR_STATUS(dano_na_entidade(10, personagem->position))){
        LOG_ERROR("Couldn't process damage on the player");
        return ERR_INTERNAL;
    }

    // Faz no_inimigo apontar para NULL só para ter certeza que não vai tentar acessar esse nó depois do dano
    no_inimigo = NULL;

    // Dá dano no dado local de inimigo
    inimigo.life_points -= 20;

    entity_t p = {
        .type = CHARACTER,
        .data = personagem
    };
    
    entity_t i = {
        .type = ENEMY,
        .data = (entity_data_t) &inimigo
    };

    // Caso o atacante seja o player, ele será a main entity do log
    if(attacker_type == CHARACTER) log_combat(p, i);

    // Caso o atacante seja o inimigo, ele será a main entity do log
    else if(attacker_type == ENEMY) log_combat(i, p);
    

    // Caso os dois fiquem vivos
    if(personagem->life_points > 0 && inimigo.life_points > 0){
        return PARTIAL_SUCCESS;
    }
    
    // Caso o player morra
    else if(personagem->life_points <= 0){
        return ACTION_SKIPPED;
    }

    // Caso só o inimigo morra
    else{
        return SUCCESS;
    }

    #ifdef DEBUG
    print_list(enemyList, stdout);
    #endif

    return SUCCESS;
}

// Destroi a entidade na posição especificada
status_t destruir_entidade(position_t position){
    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    d_linked_list_t* list = NULL;

    switch (board[position.y][position.x])
    {
    case 'E':
        list = enemyList;
        break;
    
    case 'I':
        list = itemList;
        break;
    
    case 'X':
        list = obstacleList;
        break;
    
    default:
        return ACTION_SKIPPED;
        break;
    }

    d_linked_node_t* no_entidade = search_position_d_linked_list(list, position);
    if(IS_ERROR_STATUS(remove_d_linked_list(list, no_entidade))){
        LOG_ERROR("Couldn't remove entity from list");
        return ERR_DATA;
    }

    return SUCCESS;
}

// Funções auxiliares
// Verifica se uma casa do tabuleiro é andável pelo player
// Em caso de ser andável retorna 1, caso contrário retorna 0
bool is_tile_player_walkable(position_t position){
    // Verifica se está dentro do tabuleiro
    if(is_position_valid(position) == false){
        return false;
    }

    // Verifica se há um obstáculo
    char tile = board[position.y][position.x];
    if(tile == 'X'){
        return false;
    }

    return true;
}

// Verifica se uma casa do tabuleiro é andável pelos inimigos
// Em caso de ser andável retorna 1, caso contrário retorna 0
bool is_tile_enemy_walkable(position_t position){
    // Verifica se está dentro do tabuleiro
    if(is_position_valid(position) == false){
        return false;
    }

    // Verifica se há um obstáculo
    char tile = board[position.y][position.x];
    switch (tile)
    {
    case 'X':
    case 'I':
    case 'E':
        return false;
        break;
    
    default:
        return true;
        break;
    }
}

// Verifica se é uma posição válida no tabuleiro
// Em caso de ser retorna 1, caso contrário retorna 0
bool is_position_valid(position_t position){
    // Verifica se a posição está dentro dos limites do tabuleiro
    return (
        position.x < 0 || 
        position.x > boardSize - 1 ||
        position.y < 0 ||
        position.y > boardSize - 1
    ) ? false : true;
}
