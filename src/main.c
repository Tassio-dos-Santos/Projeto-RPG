#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "utils/status.h"

#include "logger.h"

#include "renderer.h"

#define JOGO
#include "utils\data_structures.h"

// #define DEBUG

#ifdef DEBUG

#define ENEMY_QUANTITY 10
#define ITEM_QUANTITY 10
#define OBSTACLE_QUANTITY 10

#endif

// Constantes
#define INITIAL_HP 100
#define MAX_BOARD_SIZE 30
#define MIN_BOARD_SIZE 4
#define ENEMY_VIEW_RADIUS 3.5
#define HEALING_SPELL_COST 40
#define HEALING_SPELL_HEALING 20
#define LIGHTNING_SPELL_COST 100
#define LIGHTNING_SPELL_DAMAGE 50
#define FIREBALL_SPELL_COST 150
#define FIREBALL_SPELL_DAMAGE 50

// Variáveis globais
char** board;
int boardSize;
//int atualizarTabuleiro = 1;
character_t* player;
linked_list_t *enemyList, *itemList, *obstacleList;
queue_t *moveQueue, *logQueue;

// Threads
pthread_t logger_thr;
pthread_t renderer_thr;

// Sinais para terminar as threads
volatile sig_atomic_t end_logger = 0;
volatile sig_atomic_t end_renderer = 0;

// Sinais para sincronizar as threads
volatile sig_atomic_t display_updated = 0;

// Mutexes
pthread_mutex_t board_mutex;
pthread_mutex_t log_queue_mutex;

// Semáforos
sem_t log_queue_sem;
sem_t render_sem;

// Função principal de loop
status_t loop();

char ler_input();

// Função de inicialização
char** inicializar_tabuleiro(int N);
status_t gerar_inimigos(int quantidade);
status_t gerar_itens(int quantidade);
status_t gerar_obstaculos(int quantidade);
character_t *criar_personagem(position_t position);
status_t adicionar_inimigo(linked_list_t *lista_inimigo, position_t position);
status_t adicionar_item(linked_list_t *lista_item, position_t position, int valor);
status_t adicionar_obstaculo(linked_list_t *listaObstaculo, position_t position);

// Funções de processamento
status_t atualizar_posicoes();
status_t atualizar_posicoes_de_lista(linked_list_t *lista);
status_t adicionar_comando_fila(queue_t *fila, char acao, char direcao);
status_t processar_comando_fila(queue_t *fila);
status_t dano_no_inimigo(int dano, position_t position);
status_t destruir_entidade(position_t position);

// Funções de ações do personagem
status_t mover_personagem(character_t *personagem, char direcao);
status_t combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type);
status_t coletar_item(character_t *personagem, position_t item_position);
status_t usar_habilidade(character_t *personagem, skill_t skill, char direcao);
status_t bola_de_fogo(character_t *personagem, char direcao);
status_t relampago(character_t *personagem, char direcao);
status_t feitico_cura(character_t *personagem);

// Funções de ação dos inimigos
status_t mover_inimigos();
status_t mover_inimigo(int indexInimigo);
char movimento_aleatorio();
char movimento_perseguir(enemy_t i);
char movimento_simples_vertical(enemy_t i, int dx, int dy);
char movimento_simples_horizontal(enemy_t i, int dx, int dy);

// Funções auxiliares
bool is_tile_player_walkable(position_t position);
bool is_tile_enemy_walkable(position_t position);
bool is_position_valid(position_t position);

// Função de encerramento
status_t liberar_memoria(char **tabuleiro, character_t *p, linked_list_t *listaInimigo, linked_list_t *listaItem, linked_list_t *listaObstaculo, queue_t *filaMovimento);

int main(){
    // Inicializa as estruturas de dados
    enemyList = create_linked_list(ENEMY_TYPE);
    itemList = create_linked_list(ITEM_TYPE);
    obstacleList = create_linked_list(OBSTACLE_TYPE);
    moveQueue = create_queue(MOVE_TYPE);
    logQueue = create_queue(EVENT_TYPE);

    // Inicializa o logger
    pthread_create(&logger_thr, NULL, &logger, NULL);
    pthread_create(&renderer_thr, NULL, &renderer, NULL);

    // Inicializa todos os mutexes
    pthread_mutex_init(&log_queue_mutex, NULL);
    pthread_mutex_init(&board_mutex, NULL);

    // Inicializa todos os semáforos
    sem_init(&log_queue_sem, 0, 0);
    sem_init(&render_sem, 0, 0);

    // Inicializa os números aleatórios
    srand(time(NULL));

    system("cls");

    #ifndef DEBUG
    printf("Digite o numeros de linhas e colunas do tabuleiro: ");
    fflush(stdout);
    scanf("%d", &boardSize);

    if(boardSize > MAX_BOARD_SIZE) boardSize = MAX_BOARD_SIZE;
    if(boardSize < MIN_BOARD_SIZE) boardSize = MIN_BOARD_SIZE;

    int tiles = boardSize*boardSize;
    const int quantidade_inimigos = tiles/18; 
    const int quantidade_itens = tiles/15; 
    const int quantidade_obstaculos = tiles/8; 

    board = inicializar_tabuleiro(boardSize);

    int posicao[2] = {0};

    printf("Digite a posicao x do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[0]));
    // Limita a posição inicial do personagem para sempre ficar dentro do tabuleiro
    if(posicao[0] > boardSize - 1) posicao[0] = boardSize - 1;
    if(posicao[0] < 0) posicao[0] = 0;

    printf("Digite a posicao y do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[1]));
    // Limita a posição inicial do personagem para sempre ficar dentro do tabuleiro
    if(posicao[1] > boardSize - 1) posicao[1] = boardSize - 1;
    if(posicao[1] < 0) posicao[1] = 0;

    position_t player_initial_position = {
        .x = posicao[0],
        .y = posicao[1]
    };

    player = criar_personagem(player_initial_position);

    gerar_inimigos(quantidade_inimigos);

    gerar_itens(quantidade_itens);

    gerar_obstaculos(quantidade_obstaculos);

    while(loop() == SUCCESS);

    #endif

    #ifdef DEBUG
    #define BOARDSZ 10
    #define PLAYERPOSX 0
    #define PLAYERPOSY 0

    boardSize = BOARDSZ;

    board = inicializar_tabuleiro(boardSize);

    position_t player_position = {
        .x = PLAYERPOSX,
        .y = PLAYERPOSY
    };

    player = criar_personagem(player_position);

    gerar_inimigos(ENEMY_QUANTITY);

    // for(int i = 9; i > 7; i--){
    //     position_t enemy_position = {
    //         .x = 9,
    //         .y = i
    //     };

    //     adicionar_inimigo(enemyList, enemy_position);
    // }

    gerar_obstaculos(OBSTACLE_QUANTITY);

    // for(int i = 9; i > 2; i--){
    //     position_t obstacle_position = {
    //         .x = 5,
    //         .y = i
    //     };

    //     adicionar_obstaculo(obstacleList, obstacle_position);
    // }

    gerar_itens(ITEM_QUANTITY);

    while(loop() == SUCCESS);

    #endif

    // Finaliza a thread logger
    end_logger = 1;
    sem_post(&log_queue_sem);
    pthread_mutex_unlock(&log_queue_mutex);
    pthread_join(logger_thr, NULL);

    // Finaliza a thread renderer
    end_renderer = 1;
    sem_post(&render_sem);
    pthread_mutex_unlock(&board_mutex);
    pthread_join(renderer_thr, NULL);

    status_t liberacao = liberar_memoria(board, player, enemyList, itemList, obstacleList, moveQueue);

    if(IS_ERROR_STATUS(liberacao)){
        return 1;
    }
    else{
        return 0;
    } 
}

status_t loop(){
    char input;

    if(IS_ERROR_STATUS(atualizar_posicoes())){
        fputs("ERROR - function loop: Couldn't update positions\n", stderr);
        fflush(stderr);
        return ERR_INTERNAL;
    }

    mostrar_tabuleiro(board);

    while(display_updated == 0);

    // Caso todos itens forem coletados ou todos os inimigos derrotados
    if(itemList->length < 1 || enemyList->length < 1){
        printf("Voce venceu!\n");

        return ACTION_SKIPPED;
    }
    // Caso o player esteja morto, o jogo acaba
    if(player->life_points < 1){
        printf("Voce perdeu.\n");

        return ACTION_SKIPPED;
    }
    
    #ifndef DEBUG
    printf("Acao ('WASD' para movimento, 'C' para feitico de cura, 'R' para relampago, 'B' para bola de fogo, 'Q' para sair do programa): \n");
    fflush(stdout);

    input = ler_input();
    #endif

    #ifdef DEBUG

    fflush(stdout);
    Sleep(500);
    input = 'S';

    #endif

    switch (input)
    {
    case 'Q':
        return ACTION_SKIPPED;
        break;
    
    case 'W':
    case 'A':
    case 'S':
    case 'D':{
            if(IS_ERROR_STATUS(adicionar_comando_fila(moveQueue, 'M', input))){
                fputs("ERROR - function loop: Couldn't add move into queue\n", stderr);
                fflush(stderr);
                return ERR_INTERNAL;
            }

            status_t movimento_player = processar_comando_fila(moveQueue);

            // Caso o movimento não tenha sido realizado por conta de um erro, o loop acaba
            if(IS_ERROR_STATUS(movimento_player)){
                fputs("ERROR - function loop: Couldn't move character\n", stderr);
                fflush(stderr);
                return ERR_INTERNAL;
            }
            
            else if(movimento_player == ACTION_SKIPPED){
                // Caso o player morra, o loop é reiniciado novamente, para processar a morte do player
                if(player->life_points < 1){
                    return SUCCESS;
                }
            }

            status_t movimento_inimigos = mover_inimigos();

            if(IS_ERROR_STATUS(movimento_inimigos)){
                fputs("ERROR - function loop: Couldn't move enemies\n", stderr);
                fflush(stderr);
                return ERR_INTERNAL;
            }
            
            return SUCCESS;
        }
    break;

    case 'C':
        usar_habilidade(player, HEALING_SPELL, 'W');
        break;
    
    case 'R':
        printf("Direcao: 'WASD'\n");

        input = ler_input();

        usar_habilidade(player, LIGHTNING, input);
        break;
    
    case 'B':
        printf("Direcao: 'WASD'\n");

        input = ler_input();

        usar_habilidade(player, FIREBALL, input);
        break;
    
    default:
        //atualizarTabuleiro = 0;
        return SUCCESS;
        break;
    }

    if(IS_ERROR_STATUS(mover_inimigos())){
        fputs("ERROR - function loop: Couldn't move enemies\n", stderr);
        fflush(stderr);
        return ERR_INTERNAL;
    }

    return SUCCESS;
}

char ler_input(){
    char input;

    while(!_kbhit());
    input = _getch();
    input = toupper(input);

    return input;
}

char** inicializar_tabuleiro(int N){
    char** tabuleiro = (char**) calloc(N, sizeof(char*));
    if(tabuleiro == NULL){
        fputs("ERROR - function inicializar_tabuleiro: Couldn't allocate memory\n", stderr);
        fflush(stderr);
        return NULL;
    }

    for(int i = 0; i < N; i++){
        tabuleiro[i] = (char*) calloc(N, sizeof(char));
        if(tabuleiro[i] == NULL){
            fputs("ERROR - function inicializar_tabuleiro: Couldn't allocate memory\n", stderr);
            fflush(stderr);
            return NULL;
        }
        for(int j = 0; j < N; j++){
            tabuleiro[i][j] = '-';
        }
    }

    return tabuleiro;
}

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
        fputs("ERROR - function atualizar_posicoes: Couldn't update enemies' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    // Põe os itens no tabuleiro
    if(IS_ERROR_STATUS(atualizar_posicoes_de_lista(itemList))){
        fputs("ERROR - function atualizar_posicoes: Couldn't update items' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    // Põe os obstáculos no tabuleiro
    if(IS_ERROR_STATUS(atualizar_posicoes_de_lista(obstacleList))){
        fputs("ERROR - function atualizar_posicoes: Couldn't update obstacles' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return ERR_DATA;
    }

    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Atualiza o tabuleiro (matriz de caracteres) de acordo com as posições das entidades em uma lista específica
status_t atualizar_posicoes_de_lista(linked_list_t *lista){
    if(lista == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: Invalid list\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(lista->length < 1){
        return SUCCESS;
    }

    // Começa a percorrer a lista pela cabeça da lista
    linked_node_t* current_node = lista->head;
    if(current_node == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: List head points to NULL\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }
    node_data_t current_data;

    for(int i = 0; i < lista->length; i++){
        // Define o currentData como o dado no nó atual
        current_data = current_node->data;

        position_t current_position;

        switch (lista->listType){
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
            fputs("ERROR - function atualizar_posicoes_de_lista: Invalid list type\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
            break;
        }

        // Passa pro próximo nó
        current_node = current_node->next;
    }

    return SUCCESS;
}

// Gera inimigos em posições aleatórias na lista de inimigos
status_t gerar_inimigos(int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t enemy_position = {0};

        do{
            enemy_position.x = rand() % (boardSize);
            enemy_position.y = rand() % (boardSize);

        }while(board[enemy_position.y][enemy_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_inimigo(enemyList, enemy_position))){
            fputs("ERROR - function gerar_inimigos: Couldn't add enemy to the list\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(enemyList, stdout);
    #endif

    return SUCCESS;
}

// Gera itens em posições aleatórias na lista de itens
status_t gerar_itens(int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t item_position = {0};

        int valor = (rand() + 5) % 101;

        // Gera novas posições, até achar uma desocupada
        do{
            item_position.x = rand() % (boardSize);
            item_position.y = rand() % (boardSize);
        }while(board[item_position.y][item_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_item(itemList, item_position, valor))){
            fputs("ERROR - function gerar_itens: Couldn't add item to the list\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif

    return SUCCESS;
}

// Gera obstáculos em posições aleatórias na lista de obstáculos
status_t gerar_obstaculos(int quantidade){
    position_t obstacle_position = {0};

    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
            obstacle_position.x = rand() % (boardSize);
            obstacle_position.y = rand() % (boardSize);
        }while(board[obstacle_position.y][obstacle_position.x] != '-');

        if(IS_ERROR_STATUS(adicionar_obstaculo(obstacleList, obstacle_position))){
            fputs("ERROR - function gerar_obstaculos: Couldn't add obstacle to the list\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }
    }

    #ifdef DEBUG
    print_list(obstacleList, stdout);
    #endif

    return SUCCESS;
}

// Cria um personagem na posição especificada
// Retorna o endereço para o personagem em caso de sucesso e NULL em caso de fracasso
character_t *criar_personagem(position_t position){
    if(is_position_valid(position) == false){
        fputs("ERROR - function criar_personagem: Invalid position\n", stderr);
        fflush(stderr);
        return NULL;
    }

    character_t *personagem_novo = (character_t *) malloc(sizeof(character_t));

    if(personagem_novo == NULL){
        fputs("ERROR - function criar_personagem: Couldn't allocate memory\n", stderr);
        fflush(stderr);
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

// Adiciona um inimigo com vida aleatória na posição especificada na lista de inimigos
status_t adicionar_inimigo(linked_list_t *lista_inimigo, position_t position){
    if(lista_inimigo == NULL){
        fputs("ERROR - function adicionar_inimigo: Invalid list\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        fputs("ERROR - function adicionar_inimigo: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    enemy_t new_enemy = {
        .life_points = (rand() + 5) % 51,
        .position = position
    };

    if(IS_ERROR_STATUS(insert_linked_list(lista_inimigo, (node_data_t) new_enemy, -1))){
        fputs("ERROR - function adicionar_inimigo: Couldn't add enemy to the list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'E';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Adiciona um item com valor especificado na posição especificada na lista de itens
status_t adicionar_item(linked_list_t *lista_item, position_t position, int valor){
    if(lista_item == NULL){
        fputs("ERROR - function adicionar_item: Invalid list\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        fputs("ERROR - function adicionar_item: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    item_t new_item = {
        .valor = valor,
        .position = position
    };

    if(IS_ERROR_STATUS(insert_linked_list(lista_item, (node_data_t) new_item, -1))){
        fputs("ERROR - function adicionar_item: Couldn't add item to the list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'I';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Adiciona um obstaculo posição especificada na lista de obstaculos
status_t adicionar_obstaculo(linked_list_t *lista_obstaculo, position_t position){
    if(lista_obstaculo == NULL){
        fputs("ERROR - function adicionar_obstaculo: Invalid list\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(is_position_valid(position) == false){
        fputs("ERROR - function adicionar_obstaculo: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    obstacle_t new_obstacle = {
        .position = position
    };

    if(IS_ERROR_STATUS(insert_linked_list(lista_obstaculo, (node_data_t) new_obstacle, -1))){
        fputs("ERROR - function adicionar_obstaculo: Couldn't add obstacle to the list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'X';
    pthread_mutex_unlock(&board_mutex);

    return SUCCESS;
}

// Processa o movimento do player
status_t mover_personagem(character_t *personagem, char direcao){
    if(!(direcao == 'W' || direcao == 'A' || direcao == 'S' || direcao == 'D')){
        fputs("ERROR - function mover_personagem: Invalid input\n", stderr);
        fflush(stderr);
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
            fputs("ERROR - function mover_personagem: Couldn't process combat\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }
    }
    case 'I':
    {
        if(IS_ERROR_STATUS(coletar_item(personagem, next_position))){
            fputs("ERROR - function mover_personagem: Couldn't process item colection\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }

        break;
    }
    case 'X':
    {
        #ifdef DEBUG
        int index = search_position_linked_list(obstacleList, next_position);
        linked_node_t *no_obstaculo = search_linked_list(obstacleList, index);
        if(no_obstaculo == NULL){
            fputs("ERROR - function mover_personagem: Couldn't get obstacle in obstacle list\n", stderr);
            fflush(stderr);
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
        fputs("ERROR - function mover_personagem: Couldn't update positions\n", stderr);
        fflush(stderr);
        return ERR_INTERNAL;
    }

    return SUCCESS;
}

// Processa o combate entre o player e um inimigo
status_t combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type){
    if(is_position_valid(enemy_position) == false){
        fputs("ERROR - function combate: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(board[enemy_position.y][enemy_position.x] != 'E'){
        fputs("ERROR - function combate: No enemy on the assigned position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_DST;
    }

    if(personagem == NULL){
        fputs("ERROR - function combate: Invalid character\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    // Pega os dados do inimigo e salva localmente, para caso ele seja eliminado da lista
    int index_inimigo = search_position_linked_list(enemyList, enemy_position);

    linked_node_t *no_inimigo = search_linked_list(enemyList, index_inimigo);
    if(no_inimigo == NULL){
        fputs("ERROR - function combate: Invalid node\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }
    enemy_t inimigo = no_inimigo->data.inimigo;

    // Executa o dano no inimigo na lista
    if(IS_ERROR_STATUS(dano_no_inimigo(20, enemy_position))){
        fputs("ERROR - function combate: Couldn't process damage on the enemy\n", stderr);
        fflush(stderr);
        return ERR_INTERNAL;
    }

    // Dá dano no personagem
    personagem->life_points -= 10;

    // Faz no_inimigo apontar para NULL só para ter certeza que não vai tentar acessar esse nó depois do dano
    no_inimigo = NULL;

    // Dá dano no dado local de inimigo
    inimigo.life_points -= 20;

    // Caso o atacante seja o player, ele será a main entity do log
    if(attacker_type == CHARACTER) log_combat((entity_t) *personagem, (entity_t) inimigo, attacker_type);

    // Caso o atacante seja o inimigo, ele será a main entity do log
    else if(attacker_type == ENEMY) log_combat((entity_t) inimigo, (entity_t) *personagem, attacker_type);
    

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

// Processa a coleta de um item
status_t coletar_item(character_t *personagem, position_t item_position){
    if(is_position_valid(item_position) == false){
        fputs("ERROR - function coletar_item: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    if(board[item_position.y][item_position.x] != 'I'){
        fputs("ERROR - function coletar_item: No item on the assigned position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_DST;
    }

    if(personagem == NULL){
        fputs("ERROR - function coletar_item: Invalid character\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    int index_item = search_position_linked_list(itemList, item_position);
    linked_node_t* no_item = search_linked_list(itemList, index_item);
    if(no_item == NULL){
        fputs("ERROR - function coletar_item: Couldn't find item in the list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }
    item_t item = no_item->data.item;
    if(IS_ERROR_STATUS(remove_linked_list(itemList, index_item))){
        fputs("ERROR - function coletar_item: Couldn't remove item from list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    personagem->mana_points += item.valor;
    log_item_collected(item);

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif

    return SUCCESS;
}

// Processa o uso de uma habilidade
status_t usar_habilidade(character_t *personagem, skill_t skill, char direcao){
    status_t result;
    if(personagem == NULL){
        fputs("ERROR - function usar_habilidade: Invalid character\n", stderr);
        fflush(stderr);
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
        fputs("ERROR - function usar_habilidade: Invalid skill\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
        break;
    }

    Sleep(1000);
    return result;
}

// Processa o feitiço de bola de fogo
status_t bola_de_fogo(character_t *personagem, char direcao){
    if(personagem == NULL){
        fputs("ERROR - function bola_de_fogo: Invalid character\n", stderr);
        fflush(stderr);
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
            if(IS_ERROR_STATUS(dano_no_inimigo(FIREBALL_SPELL_DAMAGE, current_position))){
                fputs("ERROR - function bola_de_fogo: Couldn't process the damage\n", stderr);
                fflush(stderr);
                return ERR_INTERNAL;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(IS_ERROR_STATUS(destruir_entidade(current_position))){
                fputs("ERROR - function bola_de_fogo: Couldn't destroy the entity\n", stderr);
                fflush(stderr);
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

    mostrar_tabuleiro();

    return SUCCESS;
}

// Processa o feitiço de relâmpago
status_t relampago(character_t *personagem, char direcao){
    if(personagem == NULL){
        fputs("ERROR - function relampago: Invalid character\n", stderr);
        fflush(stderr);
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
            if(IS_ERROR_STATUS(dano_no_inimigo(LIGHTNING_SPELL_DAMAGE, current_position))){
                fputs("ERROR - function relampago: Couldn't process the damage\n", stderr);
                fflush(stderr);
                return ERR_INTERNAL;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(IS_ERROR_STATUS(destruir_entidade(current_position))){
                fputs("ERROR - function relampago: Couldn't destroy the entity\n", stderr);
                fflush(stderr);
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

    mostrar_tabuleiro();

    return SUCCESS;
}

// Dá o dano especificado no inimigo na posição especificada
status_t dano_no_inimigo(int dano, position_t position){
    if(is_position_valid(position) == false){
        fputs("ERROR - function dano_no_inimigo: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    int index_inimigo = search_position_linked_list(enemyList, position);
    linked_node_t* no_inimigo = search_linked_list(enemyList, index_inimigo);
    if(no_inimigo == NULL){
        fputs("ERROR - function dano_no_inimigo: Enemy not found on enemy list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    enemy_t* inimigo = &no_inimigo->data.inimigo;
    if(inimigo == NULL){
        fputs("ERROR - function dano_no_inimigo: Enemy's memory address not found\n", stderr);
        fflush(stderr);
        return ERR_MEMORY;
    }

    inimigo->life_points -= dano;

    log_damage((entity_t) *inimigo, ENEMY, dano);
    
    if(inimigo->life_points < 1){
        if(IS_ERROR_STATUS(destruir_entidade(position))){
            fputs("ERROR - function dano_no_inimigo: Couldn't destroy the entity\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }
    }

    return SUCCESS;
}

// Destroi a entidade na posição especificada
status_t destruir_entidade(position_t position){
    if(is_position_valid(position) == false){
        fputs("ERROR - function destruir_entidade: Invalid position\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    linked_list_t* list = NULL;

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

    int index = search_position_linked_list(list, position);
    if(IS_ERROR_STATUS(remove_linked_list(list, index))){
        fputs("ERROR - function destruir_entidade: Couldn't remove entity from list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    return SUCCESS;
}

// Processa o feitiço de cura
status_t feitico_cura(character_t *personagem){
    if(personagem == NULL){
        fputs("ERROR - function feitico_cura: Invalid character\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    personagem->life_points += HEALING_SPELL_HEALING;

    return SUCCESS;
}

// Processa o movimento dos inimigos
status_t mover_inimigos(){
    if(enemyList->length == 0) return ACTION_SKIPPED;

    for(int i = 0; i < enemyList->length; i++){
        status_t resultado_movimento = mover_inimigo(i);

        if(IS_ERROR_STATUS(resultado_movimento)){
            fputs("ERROR - function mover_inimigos: Couldn't move enemy\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }

        else if(player->life_points < 1){
            return PARTIAL_SUCCESS;
        }
    }

    return SUCCESS;
}

// Processa o movimento de um inimigo específico
status_t mover_inimigo(int index_inimigo){
    linked_node_t *no_inimigo = search_linked_list(enemyList, index_inimigo);

    if(no_inimigo == NULL){
        fputs("ERROR - function mover_inimigo: Invalid argument\n", stderr);
        fflush(stderr);
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
            fputs("ERROR - function mover_inimigo: Couldn't update positions\n", stderr);
            fflush(stderr);
            return ERR_INTERNAL;
        }

        return SUCCESS;

        break;
    
    case 'P':
    {
        status_t resultado_combate = combate(player, inimigo.position, ENEMY);
        if(IS_ERROR_STATUS(resultado_combate)){
            fputs("ERROR - function mover_inimigo: Couldn't process combat\n", stderr);
            fflush(stderr);
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

char movimento_aleatorio(){
    const char *movimentos_possiveis = "WASD";

    int quantidade_movimentos = strlen(movimentos_possiveis);
    int indice = rand() % quantidade_movimentos;

    return movimentos_possiveis[indice];
}

char movimento_perseguir(enemy_t i){
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

char movimento_simples_vertical(enemy_t i, int dx, int dy){
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

char movimento_simples_horizontal(enemy_t i, int dx, int dy){
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

// Adiciona um movimento especificado na fila 
status_t adicionar_comando_fila(queue_t *fila, char acao, char direcao){
    action_t m = {
        .acao = acao,
        .direcao = direcao
    };

    if(IS_ERROR_STATUS(enqueue(fila, (node_data_t) m))){
        fputs("ERROR - function adicionar_comando_fila: Couldn't enqueue move\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    return SUCCESS;
}

// Processa o próximo movimento na fila
status_t processar_comando_fila(queue_t *fila){
    if(fila->length > 1){
        fputs("ERROR - function processar_comando_fila: Empty queue\n", stderr);
        fflush(stderr);
        return ACTION_SKIPPED;
    }

    node_data_t movimentoData = dequeue(fila);
    if(is_data_empty(movimentoData) == 1){
        fputs("ERROR - function processar_comando_fila: Couldn't get next move\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    action_t m = movimentoData.movimento;

    switch (m.acao)
    {
    case 'M':
        {
            status_t result = mover_personagem(player, m.direcao);

            if(result == SUCCESS){
                log_move(m);
            }

            return result;

            break;
        }
    case 'R':
    {
        status_t result = usar_habilidade(player, LIGHTNING, m.direcao);

            if(result == SUCCESS){
                log_move(m);
            }

            return result;
        break;
    }
    case 'B':
    {
        status_t result = usar_habilidade(player, FIREBALL, m.direcao);

            if(result == SUCCESS){
                log_move(m);
            }

            return result;
        break;
    }
    case 'C':
    {
        status_t result = usar_habilidade(player, HEALING_SPELL, m.direcao);

            if(result == SUCCESS){
                log_move(m);
            }

            return result;
        break;
    }
    default:
        return ACTION_SKIPPED;

        break;
    }

    return SUCCESS;
}

// Libera toda memória dinamicamente alocada pelo programa
status_t liberar_memoria(char **tabuleiro, character_t *p, linked_list_t *listaInimigo, linked_list_t *listaItem, linked_list_t *listaObstaculo, queue_t *filaMovimento){
    free(p);

    if(IS_ERROR_STATUS(delete_linked_list(enemyList))){
        fputs("ERROR - function liberar_memoria: Couldn't delete enemy list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_linked_list(itemList))){
        fputs("ERROR - function liberar_memoria: Couldn't delete item list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_linked_list(obstacleList))){
        fputs("ERROR - function liberar_memoria: Couldn't delete obstacle list\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_queue(moveQueue))){
        fputs("ERROR - function liberar_memoria: Couldn't delete move queue\n", stderr);
        fflush(stderr);
        return ERR_DATA;
    }

    for(int i = 0; i < boardSize; i++){
        free(tabuleiro[i]);
    }

    free(tabuleiro);

    return SUCCESS;
}