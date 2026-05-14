#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "utils/status.h"
#define JOGO
#include "utils/data_structures.h"
#include "utils/input.h"
#include "enemies_processing.h"
#include "player_processing.h"
#include "board_processing.h"

#include "logger.h"
#include "renderer.h"

// #define DEBUG

#ifdef DEBUG

#endif

// Constantes
#define ENEMY_QUANTITY 10
#define ITEM_QUANTITY 10
#define OBSTACLE_QUANTITY 10

// Variáveis globais
char** board;
int boardSize;
extern char keyboard_input;
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

// Funções de processamento de comandos
status_t adicionar_comando_fila(queue_t *fila, char acao, char direcao);
status_t processar_comando_fila(queue_t *fila);

// Função de encerramento
status_t finalizar_programa();

int main(){
    // Inicializa as estruturas de dados
    enemyList = create_linked_list(ENEMY_TYPE);
    itemList = create_linked_list(ITEM_TYPE);
    obstacleList = create_linked_list(OBSTACLE_TYPE);
    moveQueue = create_queue(MOVE_TYPE);
    logQueue = create_queue(LOG_TYPE);

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
    if(board == NULL){
        finalizar_programa();
        return 1;
    }

    int posicao[2] = {0};

    printf("Digite a posicao x do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[0]));
    // Limita a posição inicial do personagem para sempre ficar dentro do tabuleiro
    if(posicao[0] > boardSize - 1){
        posicao[0] = boardSize - 1;
        LOG_WARNING("Initial player's position is outside of the board limits");
    }
    if(posicao[0] < 0){
        posicao[0] = 0;
        LOG_WARNING("Initial player's position is outside of the board limits");
    }

    printf("Digite a posicao y do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[1]));
    // Limita a posição inicial do personagem para sempre ficar dentro do tabuleiro
    if(posicao[1] > boardSize - 1){
        posicao[1] = boardSize - 1;
        LOG_WARNING("Initial player's position is outside of the board limits");
    }
    if(posicao[1] < 0){
        posicao[1] = 0;
        LOG_WARNING("Initial player's position is outside of the board limits");
    }

    position_t player_initial_position = {
        .x = posicao[0],
        .y = posicao[1]
    };

    player = criar_personagem(player_initial_position);
    if(player == NULL){
        finalizar_programa();
        return 1;
    }

    if(IS_ERROR_STATUS(gerar_inimigos(enemyList, quantidade_inimigos))){
        finalizar_programa();
        return 1;
    }

    if(IS_ERROR_STATUS(gerar_itens(itemList, quantidade_itens))){
        finalizar_programa();
        return 1;
    }

    if(IS_ERROR_STATUS(gerar_obstaculos(obstacleList, quantidade_obstaculos))){
        finalizar_programa();
        return 1;
    }

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

    gerar_inimigos(enemyList, ENEMY_QUANTITY);

    // for(int i = 9; i > 7; i--){
    //     position_t enemy_position = {
    //         .x = 9,
    //         .y = i
    //     };

    //     adicionar_inimigo(enemyList, enemy_position);
    // }

    gerar_obstaculos(obstacleList, OBSTACLE_QUANTITY);

    // for(int i = 9; i > 2; i--){
    //     position_t obstacle_position = {
    //         .x = 5,
    //         .y = i
    //     };

    //     adicionar_obstaculo(obstacleList, obstacle_position);
    // }

    gerar_itens(itemList, ITEM_QUANTITY);

    while(loop() == SUCCESS);

    #endif

    if(IS_ERROR_STATUS(finalizar_programa())){
        return 1;
    }
    else{
        return 0;
    } 
}

status_t loop(){
    if(IS_ERROR_STATUS(atualizar_posicoes())){
        LOG_ERROR("Couldn't update positions");
        return ERR_INTERNAL;
    }

    mostrar_tabuleiro(board);

    while(display_updated == 0);

    // Caso todos itens forem coletados ou todos os inimigos derrotados
    if(itemList->length < 1 || enemyList->length < 1){
        printf("Voce venceu!\n");

        log_victory(*player);

        return ACTION_SKIPPED;
    }
    // Caso o player esteja morto, o jogo acaba
    if(player->life_points < 1){
        printf("Voce perdeu.\n");

        log_defeat(*player);

        return ACTION_SKIPPED;
    }
    
    #ifndef DEBUG
    printf("Acao ('WASD' para movimento, 'C' para feitico de cura, 'R' para relampago, 'B' para bola de fogo, 'Q' para sair do programa): \n");
    fflush(stdout);

    update_keyboard_input();
    #endif

    #ifdef DEBUG

    fflush(stdout);
    Sleep(500);
    keyboard_input = 'S';

    #endif

    switch (keyboard_input)
    {
    case 'Q':
        return ACTION_SKIPPED;
        break;
    
    case 'W':
    case 'A':
    case 'S':
    case 'D':{
            if(IS_ERROR_STATUS(adicionar_comando_fila(moveQueue, 'M', keyboard_input))){
                LOG_ERROR("Couldn't add move into queue");
                return ERR_INTERNAL;
            }

            status_t movimento_player = processar_comando_fila(moveQueue);

            // Caso o movimento não tenha sido realizado por conta de um erro, o loop acaba
            if(IS_ERROR_STATUS(movimento_player)){
                LOG_ERROR("Couldn't move character");
                return ERR_INTERNAL;
            }
            
            else if(movimento_player == ACTION_SKIPPED){
                // Caso o player morra, o loop é reiniciado novamente, para processar a morte do player
                if(player->life_points < 1){
                    return SUCCESS;
                }
            }
        }
    break;

    case 'C':
        if(IS_ERROR_STATUS(adicionar_comando_fila(moveQueue, 'C', 'W'))){
            LOG_ERROR("Couldn't add move into queue");
            return ERR_INTERNAL;
        }

        // Caso o feitiço não tenha sido realizado por conta de um erro, o loop acaba
        if(IS_ERROR_STATUS(processar_comando_fila(moveQueue))){
            LOG_ERROR("Couldn't cast the healing spell");
            return ERR_INTERNAL;
        }

        break;
    
    case 'R':
        printf("Direcao: 'WASD'\n");

        update_keyboard_input();

        if(IS_ERROR_STATUS(adicionar_comando_fila(moveQueue, 'R', keyboard_input))){
            LOG_ERROR("Couldn't add move into queue");
            return ERR_INTERNAL;
        }

        // Caso o feitiço não tenha sido realizado por conta de um erro, o loop acaba
        if(IS_ERROR_STATUS(processar_comando_fila(moveQueue))){
            LOG_ERROR("Couldn't cast the lightning spell");
            return ERR_INTERNAL;
        }

        break;
    
    case 'B':
        printf("Direcao: 'WASD'\n");

        update_keyboard_input();

        if(IS_ERROR_STATUS(adicionar_comando_fila(moveQueue, 'B', keyboard_input))){
            LOG_ERROR("Couldn't add move into queue");
            return ERR_INTERNAL;
        }

        // Caso o feitiço não tenha sido realizado por conta de um erro, o loop acaba
        if(IS_ERROR_STATUS(processar_comando_fila(moveQueue))){
            LOG_ERROR("Couldn't cast the fireball spell");
            return ERR_INTERNAL;
        }

        break;
    
    default:
        //atualizarTabuleiro = 0;
        return SUCCESS;
        break;
    }

    if(IS_ERROR_STATUS(mover_inimigos(enemyList))){
        LOG_ERROR("Couldn't move enemies");
        return ERR_INTERNAL;
    }

    return SUCCESS;
}

// Adiciona um movimento especificado na fila 
status_t adicionar_comando_fila(queue_t *fila, char acao, char direcao){
    action_t m = {
        .acao = acao,
        .direcao = direcao
    };

    if(IS_ERROR_STATUS(enqueue(fila, (node_data_t) m))){
        LOG_ERROR("Couldn't enqueue move");
        return ERR_DATA;
    }

    return SUCCESS;
}

// Processa o próximo movimento na fila
status_t processar_comando_fila(queue_t *fila){
    if(fila->length > 1){
        LOG_ERROR("Empty queue");
        return ACTION_SKIPPED;
    }

    node_data_t movimentoData = dequeue(fila);
    if(is_data_empty(movimentoData) == 1){
        LOG_ERROR("Couldn't get next move");
        return ERR_DATA;
    }

    action_t m = movimentoData.movimento;

    switch (m.acao)
    {
    case 'M':
        {
            return mover_personagem(player, m.direcao);

            break;
        }
    case 'R':
    {
        return usar_habilidade(player, LIGHTNING, m.direcao);

        break;
    }
    case 'B':
    {
        return usar_habilidade(player, FIREBALL, m.direcao);

        break;
    }
    case 'C':
    {
        return usar_habilidade(player, HEALING_SPELL, m.direcao);

        break;
    }
    default:
        return ACTION_SKIPPED;

        break;
    }

    return SUCCESS;
}

// Libera toda memória dinamicamente alocada pelo programa
status_t finalizar_programa(){
    // Finaliza a thread logger
    end_logger = 1;
    sem_post(&log_queue_sem);
    pthread_mutex_unlock(&log_queue_mutex);
    pthread_join(logger_thr, NULL);

    sem_destroy(&log_queue_sem);
    pthread_mutex_destroy(&log_queue_mutex);

    // Finaliza a thread renderer
    end_renderer = 1;
    sem_post(&render_sem);
    pthread_mutex_unlock(&board_mutex);
    pthread_join(renderer_thr, NULL);

    sem_destroy(&render_sem);
    pthread_mutex_destroy(&board_mutex);

    if(player != NULL){
        free(player);
        player = NULL;
    }

    if(IS_ERROR_STATUS(delete_linked_list(enemyList))){
        LOG_ERROR("Couldn't delete enemy list");
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_linked_list(itemList))){
        LOG_ERROR("Couldn't delete item list");
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_linked_list(obstacleList))){
        LOG_ERROR("Couldn't delete obstacle list");
        return ERR_DATA;
    }

    if(IS_ERROR_STATUS(delete_queue(moveQueue))){
        LOG_ERROR("Couldn't delete move queue");
        return ERR_DATA;
    }

    for(int i = 0; i < boardSize; i++){
        if(board[i] != NULL){
            free(board[i]);
            board[i] = NULL;
        }
        
    }

    if(board != NULL){
        free(board);
        board = NULL;
    }

    return SUCCESS;
}