#include "enemies_processing.h"

// Variáveis globais alocadas em main
extern char **board;
extern int boardSize;
extern character_t *player;
extern pthread_mutex_t board_mutex;

// Declaração das funções internas
static char movimento_aleatorio();
static char movimento_perseguir(enemy_t i);
static char movimento_simples_vertical(enemy_t i, int dx, int dy);
static char movimento_simples_horizontal(enemy_t i, int dx, int dy);

// Definição de funções
// Funções de inicialização
// Gera inimigos em posições aleatórias na lista de inimigos
status_t gerar_inimigos(linked_list_t *enemy_list, int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t enemy_position = {0};

        do{
            enemy_position.x = rand() % (boardSize);
            enemy_position.y = rand() % (boardSize);

        }while(board[enemy_position.y][enemy_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_inimigo(enemy_list, enemy_position))){
            LOG_ERROR("Couldn't add enemy to the list");
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(enemyList, stdout);
    #endif

    return SUCCESS;
}

// Adiciona um inimigo com vida aleatória na posição especificada na lista de inimigos
status_t adicionar_inimigo(linked_list_t *lista_inimigo, position_t position){
    if(lista_inimigo == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        LOG_ERROR("Invalid position");
        return ERR_INVALID_IN;
    }

    enemy_t new_enemy = {
        .life_points = (rand() + 5) % 51,
        .position = position
    };

    if(IS_ERROR_STATUS(insert_linked_list(lista_inimigo, (node_data_t) new_enemy, -1))){
        LOG_ERROR("Couldn't add enemy to the list");
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'E';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Funções de processamento
status_t mover_inimigos(linked_list_t *enemy_list){
    if(enemy_list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(enemy_list->length == 0) return ACTION_SKIPPED;

    for(int i = 0; i < enemy_list->length; i++){
        status_t resultado_movimento = mover_inimigo(enemy_list, i);

        if(IS_ERROR_STATUS(resultado_movimento)){
            LOG_ERROR("Couldn't move enemy");
            return ERR_INTERNAL;
        }
    }

    return SUCCESS;
}

status_t mover_inimigo(linked_list_t *enemy_list, int index_inimigo){
    if(enemy_list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    linked_node_t *no_inimigo = search_linked_list(enemy_list, index_inimigo);

    if(no_inimigo == NULL){
        LOG_ERROR("Invalid argument");
        return ERR_INVALID_IN;
    }

    enemy_t inimigo = no_inimigo->data.inimigo;

    position_t next_position = inimigo.position;

    // Lógica de decisão de movimento
    char direcao;

    // Distância entre o player e o inimigo
    int dx = player->position.x - inimigo.position.x;
    int dy = player->position.y - inimigo.position.y;
    double dis = sqrt(dx*dx + dy*dy);

    // Se o player está dentro do raio de visão do inimigo, o inimigo o persegue
    if(dis <= ENEMY_VIEW_RADIUS){
        direcao = movimento_perseguir(inimigo);
    }
    // Se não estiver ele se move aleatoriamente
    else{
        direcao = movimento_aleatorio();
    }

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

    // Impede que o inimigo saia dos limites do tabuleiro
    if(is_position_valid(next_position) == false) return ACTION_SKIPPED;

    // Detecção de colisão
    switch (board[next_position.y][next_position.x]){
    case '-':
        no_inimigo->data.inimigo.position = next_position;

        if(IS_ERROR_STATUS(atualizar_posicoes())){
            LOG_ERROR("Couldn't update positions");
            return ERR_INTERNAL;
        }

        return SUCCESS;

        break;
    
    case 'P':
    {
        status_t resultado_combate = combate(player, inimigo.position, ENEMY);
        if(IS_ERROR_STATUS(resultado_combate)){
            LOG_ERROR("Couldn't process combat");
            return ERR_INTERNAL;
        }

        // Em caso de combate, o inimigo não se move, seja porque o player morrer, porque o player ocupa a posição que ele quer ir ou porque o inimigo morreu 
        return ACTION_SKIPPED;

        break;
    }

    case 'E':
    case 'I':
    case 'X':
    default:
        return ACTION_SKIPPED;
        break;
    }
}

static char movimento_aleatorio(){
    const char *movimentos_possiveis = "WASD";

    int quantidade_movimentos = strlen(movimentos_possiveis);
    int indice = rand() % quantidade_movimentos;

    return movimentos_possiveis[indice];
}

static char movimento_perseguir(enemy_t i){
    int dx = player->position.x - i.position.x;
    int dy = player->position.y - i.position.y;

    // Se direção for 1, vai tentar caminhar na direção vertical, se for 0 vai tentar caminhar na direção horizontal
    int direcao = (abs(dy) > abs(dx))?1:0;

    // Tenta ir na vertical
    if(direcao == 1){
        // Player acima
        if(dy > 0){
            // Vê se consegue se mover para cima
            if(movimento_simples_vertical(i, dx, dy) == 'W'){
                return 'W';
            }

            // Vê se consegue se mover em alguma direção horizontal
            char movimento_horizontal = movimento_simples_horizontal(i, dx, dy);
            if(movimento_horizontal != '\0'){
                return movimento_horizontal;
            }

            // Se não conseguiu se mover para nenhum lugar, vai para baixo
            return 'S';
        }

        // Player abaixo
        else if(dy <= 0){
            // Vê se consegue se mover para baixo
            if(movimento_simples_vertical(i, dx, dy) == 'S'){
                return 'S';
            }

            // Vê se consegue se mover em alguma direção horizontal
            char movimento_horizontal = movimento_simples_horizontal(i, dx, dy);
            if(movimento_horizontal != '\0'){
                return movimento_horizontal;
            }

            // Se não conseguiu se mover para nenhum lugar, vai para cima
            return 'W';
        }
    }
    
    // Tenta ir na horizontal
    else if(direcao == 0){
        // Player à direita
        if(dx > 0){
            // Vê se consegue se mover para direita
            if(movimento_simples_horizontal(i, dx, dy) == 'D'){
                return 'D';
            }

            // Vê se consegue se mover em alguma direção vertical
            char movimento_vertical = movimento_simples_vertical(i, dx, dy);
            if(movimento_vertical != '\0'){
                return movimento_vertical;
            }

            // Se não conseguiu se mover para nenhum lugar, vai para esquerda
            return 'A';
        }

        // Player à esquerda
        else if(dx <= 0){
            // Vê se consegue se mover para esquerda
            if(movimento_simples_horizontal(i, dx, dy) == 'A'){
                return 'A';
            }

            // Vê se consegue se mover em alguma direção vertical
            char movimento_vertical = movimento_simples_vertical(i, dx, dy);
            if(movimento_vertical != '\0'){
                return movimento_vertical;
            }

            // Se não conseguiu se mover para nenhum lugar, vai para direita
            return 'D';
        }
    }
}

static char movimento_simples_vertical(enemy_t i, int dx, int dy){
    position_t next_position = {0};

    // O player está acima
    if(dy > 0){
        // Move pra cima se puder
        next_position.x = i.position.x;
        next_position.y = i.position.y + 1;
        
        if(is_tile_enemy_walkable(next_position)){
            return 'W';
        }

        // Se não, tenta se mover para baixo
        next_position.x = i.position.x;
        next_position.y = i.position.y - 1;

        if(is_tile_enemy_walkable(next_position)){
            return 'S';
        }
    }

    // O player está abaixo
    else if(dy <= 0){
        // Move pra baixo se puder
        next_position.x = i.position.x;
        next_position.y = i.position.y - 1;

        if(is_tile_enemy_walkable(next_position)){
            return 'S';
        }

        // Se não, tenta se mover para cima
        next_position.x = i.position.x;
        next_position.y = i.position.y + 1;

        if(is_tile_enemy_walkable(next_position)){
            return 'W';
        }
    }

    // Se não conseguiu se mover pra lugar nenhum, retorna \0
    return '\0';
}

static char movimento_simples_horizontal(enemy_t i, int dx, int dy){
    position_t next_position = {0};

    // O player está à direita
    if(dx > 0){
        // Move pra direita se puder
        next_position.x = i.position.x + 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position)){
            return 'D';
        }

        // Se não, tenta se mover para esquerda
        next_position.x = i.position.x - 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position)){
            return 'A';
        }
    }

    // O player está à esquerda
    else if(dx <= 0){
        // Move pra esquerda se puder
        next_position.x = i.position.x - 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position)){
            return 'A';
        }

        // Se não, tenta se mover para direita
        next_position.x = i.position.x + 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position)){
            return 'D';
        }
    }

    // Se não conseguiu se mover pra lugar nenhum, retorna \0
    return '\0';
}
