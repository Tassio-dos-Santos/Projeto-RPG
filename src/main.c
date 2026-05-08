#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "logger.h"

#include "renderer.h"

#define JOGO
#include "data_structures.h"

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
linkedList_t *enemyList, *itemList, *obstacleList;
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
int loop();

// Função de inicialização
char** inicializar_tabuleiro(int N);
int gerar_inimigos(int quantidade);
int gerar_itens(int quantidade);
int gerar_obstaculos(int quantidade);
character_t *criar_personagem(position_t position);
int adicionar_inimigo(linkedList_t *lista_inimigo, position_t position);
int adicionar_item(linkedList_t *lista_item, position_t position, int valor);
int adicionar_obstaculo(linkedList_t *listaObstaculo, position_t position);

// Funções de processamento
int atualizar_posicoes();
int atualizar_posicoes_de_lista(linkedList_t *lista);
int adicionar_comando_fila(queue_t *fila, char acao, char direcao);
int processar_comando_fila(queue_t *fila);
int dano_no_inimigo(int dano, position_t position);
int destruir_entidade(position_t position);

// Funções de ações do personagem
int mover_personagem(character_t *personagem, char direcao);
int combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type);
int coletar_item(character_t *personagem, position_t item_position);
int usar_habilidade(character_t *personagem, skill_t skill, char direcao);
int bola_de_fogo(character_t *personagem, char direcao);
int relampago(character_t *personagem, char direcao);
int feitico_cura(character_t *personagem);

// Funções de ação dos inimigos
int mover_inimigos();
int mover_inimigo(int indexInimigo);
char movimento_aleatorio();
char movimento_perseguir(enemy_t i);
char movimento_simples_vertical(enemy_t i, int dx, int dy);
char movimento_simples_horizontal(enemy_t i, int dx, int dy);

// Funções auxiliares
int is_tile_player_walkable(position_t position);
int is_tile_enemy_walkable(position_t position);
int is_position_valid(position_t position);

// Função de encerramento
int liberar_memoria(char **tabuleiro, character_t *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento);

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

    while(loop());

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

    while(loop());

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

    return liberar_memoria(board, player, enemyList, itemList, obstacleList, moveQueue);
}

int loop(){
    char input;

    if(atualizar_posicoes() == 0){
        fputs("ERROR - function loop: Couldn't update positions\n", stderr);
        fflush(stderr);
        return 0;
    }

    mostrar_tabuleiro(board);

    while(display_updated == 0);

    // Caso todos itens forem coletados ou todos os inimigos derrotados
    if(itemList->length < 1 || enemyList->length < 1){
        printf("Voce venceu!\n");

        return 0;
    }
    // Caso o player esteja morto, o jogo acaba
    if(player->life_points < 1){
        printf("Voce perdeu.\n");

        return 0;
    }
    
    #ifndef DEBUG
    printf("Acao ('WASD' para movimento, 'E' para habilidade, 'Q' para sair do programa): \n");
    fflush(stdout);

    while(!_kbhit());
    input = _getch();
    input = toupper(input);
    #endif

    #ifdef DEBUG

    fflush(stdout);
    Sleep(500);
    input = 'S';

    #endif

    switch (input)
    {
    case 'Q':
        return 0;
        break;
    
    case 'W':
    case 'A':
    case 'S':
    case 'D':{
            if(adicionar_comando_fila(moveQueue, 'M', input) == 0){
                fputs("ERROR - function loop: Couldn't add move into queue\n", stderr);
                fflush(stderr);
                return 0;
            }

            int movimento_player = processar_comando_fila(moveQueue);

            // Caso o movimento não tenha sido realizado por conta de um erro, o loop acaba
            if(movimento_player == -1){
                fputs("ERROR - function loop: Couldn't move character\n", stderr);
                fflush(stderr);
                return 0;
            }
            // Caso o movimento não tenha sido realizado porque ele era invalido, não atualiza a tabuleiro
            else if(movimento_player == 1){
                //atualizarTabuleiro = 0;
            }
            // Caso o player morra, o jogo acaba
            else if(movimento_player == 2){
                return 1;
            }
            // Caso o movimento tenha sido realizado, atualiza a tabuleiro
            else if(movimento_player == 0){
                //atualizarTabuleiro = 1;
            }

            int movimento_inimigos = mover_inimigos();

            if(movimento_inimigos == -1){
                fputs("ERROR - function loop: Couldn't move enemies\n", stderr);
                fflush(stderr);
                return 0;
            }
            
            return 1;
        }
    
    case 'E':
        break;
    
    default:
        //atualizarTabuleiro = 0;
        return 1;
        break;
    }

    // Essa parte do loop só será executada caso seja selecionado ataque
    #ifndef DEBUG

    printf("Habilidade ('C' para feitico de cura, 'R' para relampago, 'B' para bola de fogo): \n");
    fflush(stdout);

    while(!_kbhit());
    input = _getch();
    input = toupper(input);

    #endif

    #ifdef DEBUG

    Sleep(500);
    input = 'S';

    #endif

    char skill = input;

    switch (skill)
    {
    case 'C':
        usar_habilidade(player, HEALING_SPELL, 'W');
        break;
    
    case 'R':
        printf("Direcao: 'WASD'\n");
        while(!_kbhit());
        input = _getch();
        input = toupper(input);
        usar_habilidade(player, LIGHTNING, input);
        break;
    
    case 'B':
        printf("Direcao: 'WASD'\n");
        while(!_kbhit());
        input = _getch();
        input = toupper(input);
        usar_habilidade(player, FIREBALL, input);
        break;
    
    default:
        break;
    }

    //atualizarTabuleiro = 1;

    if(mover_inimigos() == -1){
        fputs("ERROR - function loop: Couldn't move enemies\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
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
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int atualizar_posicoes(){
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
    if(atualizar_posicoes_de_lista(enemyList) == 0){
        fputs("ERROR - function atualizar_posicoes: Couldn't update enemies' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return 0;
    }

    // Põe os itens no tabuleiro
    if(atualizar_posicoes_de_lista(itemList) == 0){
        fputs("ERROR - function atualizar_posicoes: Couldn't update items' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return 0;
    }

    // Põe os obstáculos no tabuleiro
    if(atualizar_posicoes_de_lista(obstacleList) == 0){
        fputs("ERROR - function atualizar_posicoes: Couldn't update obstacles' position\n", stderr);
        fflush(stderr);
        pthread_mutex_unlock(&board_mutex);
        return 0;
    }

    pthread_mutex_unlock(&board_mutex);

    return 1;
}

// Atualiza o tabuleiro (matriz de caracteres) de acordo com as posições das entidades em uma lista específica
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int atualizar_posicoes_de_lista(linkedList_t *lista){
    if(lista == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: Invalid list\n", stderr);
        fflush(stderr);
        return 0;
    }

    if(lista->length < 1){
        return 1;
    }

    // Começa a percorrer a lista pela cabeça da lista
    linkedNode_t* current_node = lista->head;
    if(current_node == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: List head points to NULL\n", stderr);
        fflush(stderr);
        return 0;
    }
    nodeData_t current_data;

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
            return 0;
            break;
        }

        // Passa pro próximo nó
        current_node = current_node->next;
    }
}

// Gera inimigos em posições aleatórias na lista de inimigos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int gerar_inimigos(int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t enemy_position = {0};

        do{
            enemy_position.x = rand() % (boardSize);
            enemy_position.y = rand() % (boardSize);

        }while(board[enemy_position.y][enemy_position.x] != '-');

        if(adicionar_inimigo(enemyList, enemy_position) == 0){
            fputs("ERROR - function gerar_inimigos: Couldn't add enemy to the list\n", stderr);
            fflush(stderr);
            return 0;
        }
    }

    #ifdef DEBUG
    print_list(enemyList, stdout);
    #endif
}

// Gera itens em posições aleatórias na lista de itens
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int gerar_itens(int quantidade){
    for(int i = 0; i < quantidade; i++){
        position_t item_position = {0};

        int valor = (rand() + 5) % 101;

        // Gera novas posições, até achar uma desocupada
        do{
            item_position.x = rand() % (boardSize);
            item_position.y = rand() % (boardSize);
        }while(board[item_position.y][item_position.x] != '-');

        if(adicionar_item(itemList, item_position, valor) == 0){
            fputs("ERROR - function gerar_itens: Couldn't add item to the list\n", stderr);
            fflush(stderr);
            return 0;
        }
    }

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif
}

// Gera obstáculos em posições aleatórias na lista de obstáculos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int gerar_obstaculos(int quantidade){
    position_t obstacle_position = {0};

    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
            obstacle_position.x = rand() % (boardSize);
            obstacle_position.y = rand() % (boardSize);
        }while(board[obstacle_position.y][obstacle_position.x] != '-');

        if(adicionar_obstaculo(obstacleList, obstacle_position) == 0){
            fputs("ERROR - function gerar_obstaculos: Couldn't add obstacle to the list\n", stderr);
            fflush(stderr);
            return 0;
        }
    }

    #ifdef DEBUG
    print_list(obstacleList, stdout);
    #endif
}

// Cria um personagem na posição especificada
// Retorna o endereço para o personagem em caso de sucesso e NULL em caso de fracasso
character_t *criar_personagem(position_t position){
    if(is_position_valid(position) == 0){
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
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_inimigo(linkedList_t *lista_inimigo, position_t position){
    if(lista_inimigo == NULL){
        fputs("ERROR - function adicionar_inimigo: Invalid list\n", stderr);
        fflush(stderr);
        return 0;
    }

    if(is_position_valid(position) == 0){
        fputs("ERROR - function adicionar_inimigo: Invalid position\n", stderr);
        fflush(stderr);
        return 0;
    }

    enemy_t new_enemy = {
        .life_points = (rand() + 5) % 51,
        .position = position
    };

    if(insert_linked_list(lista_inimigo, (nodeData_t) new_enemy, -1) == 0){
        fputs("ERROR - function adicionar_inimigo: Couldn't add enemy to the list\n", stderr);
        fflush(stderr);
        return 0;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'E';
    pthread_mutex_unlock(&board_mutex);

    return 1;
}

// Adiciona um item com valor especificado na posição especificada na lista de itens
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_item(linkedList_t *lista_item, position_t position, int valor){
    if(lista_item == NULL){
        fputs("ERROR - function adicionar_item: Invalid list\n", stderr);
        fflush(stderr);
        return 0;
    }

    if(is_position_valid(position) == 0){
        fputs("ERROR - function adicionar_item: Invalid position\n", stderr);
        fflush(stderr);
        return 0;
    }

    item_t new_item = {
        .valor = valor,
        .position = position
    };

    if(insert_linked_list(lista_item, (nodeData_t) new_item, -1) == 0){
        fputs("ERROR - function adicionar_item: Couldn't add item to the list\n", stderr);
        fflush(stderr);
        return 0;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'I';
    pthread_mutex_unlock(&board_mutex);

    return 1;
}

// Adiciona um obstaculo posição especificada na lista de obstaculos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_obstaculo(linkedList_t *lista_obstaculo, position_t position){
    if(lista_obstaculo == NULL){
        fputs("ERROR - function adicionar_obstaculo: Invalid list\n", stderr);
        fflush(stderr);
        return 0;
    }

    if(is_position_valid(position) == 0){
        fputs("ERROR - function adicionar_obstaculo: Invalid position\n", stderr);
        fflush(stderr);
        return 0;
    }

    obstacle_t new_obstacle = {
        .position = position
    };

    if(insert_linked_list(lista_obstaculo, (nodeData_t) new_obstacle, -1) == 0){
        fputs("ERROR - function adicionar_obstaculo: Couldn't add obstacle to the list\n", stderr);
        fflush(stderr);
        return 0;
    }

    pthread_mutex_lock(&board_mutex);
    board[position.y][position.x] = 'X';
    pthread_mutex_unlock(&board_mutex);

    return 1;
}

// Processa o movimento do player
// Caso o movimento não tenha sido realizado por conta de erro em algum processamento retorna -1,
// Caso o movimento tenha sido realizado retorna 0,
// Caso o movimento não tenha sido realizado porque era inválido retorna 1,
// Caso o movimento não tenha sido realizado porque o player morreu retorna 2
int mover_personagem(character_t *personagem, char direcao){
    if(!(direcao == 'W' || direcao == 'A' || direcao == 'S' || direcao == 'D')){
        fputs("ERROR - function mover_personagem: Invalid input\n", stderr);
        fflush(stderr);
        return 1;
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
    if(is_position_valid(next_position) == 0) return 1;

    // Detecção de colisão
    switch (board[next_position.y][next_position.x]){
    case '-':
        break;
    
    case 'E':
    {
        int resultado_combate = combate(personagem, next_position, CHARACTER);
        if(resultado_combate == -1){
            fputs("ERROR - function mover_personagem: Couldn't process combat\n", stderr);
            fflush(stderr);
            return -1;
        }

        // O inimigo não morreu
        else if(resultado_combate == 0){
            return 1;
        }

        // Player morreu
        else if(resultado_combate == 2){
            return 2;
        }

        // Caso só inimigo morreu, continua
        break;
    }
    case 'I':
    {
        if(coletar_item(personagem, next_position) == 0){
            fputs("ERROR - function mover_personagem: Couldn't process item colection\n", stderr);
            fflush(stderr);
            return -1;
        }

        break;
    }
    case 'X':
    {
        #ifdef DEBUG
        int index = search_position_linked_list(obstacleList, next_position);
        obstacle_t obstaculo = search_linked_list(obstacleList, index)->data.obstaculo;
        print_obstacle(obstaculo, stdout);
        #endif
    }
    case 'P':
    default:
        return 1;
        break;
    }

    // Se chegou até essa parte do código (não passou por return), o movimento é executado

    personagem->position = next_position;

    if(atualizar_posicoes() == 0){
        fputs("ERROR - function mover_personagem: Couldn't update positions\n", stderr);
        fflush(stderr);
        return -1;
    }

    return 0;
}

// Processa o combate entre o player e um inimigo
// Caso ninguém morra, retorna 0
// Caso o inimigo morra, retorna 1
// Caso o player morra, retorna 2
// Caso dê erro, retorna -1
int combate(character_t *personagem, position_t enemy_position, entity_type_t attacker_type){
    if(is_position_valid(enemy_position) == 0){
        fputs("ERROR - function combate: Invalid position\n", stderr);
        fflush(stderr);
        return -1;
    }

    if(board[enemy_position.y][enemy_position.x] != 'E'){
        fputs("ERROR - function combate: No enemy on the assigned position\n", stderr);
        fflush(stderr);
        return -1;
    }

    if(personagem == NULL){
        fputs("ERROR - function combate: Invalid character\n", stderr);
        fflush(stderr);
        return -1;
    }

    // Dá dano no personagem
    personagem->life_points -= 10;

    // Pega os dados do inimigo e salva localmente, para caso ele seja eliminado da lista
    int index_inimigo = search_position_linked_list(enemyList, enemy_position);

    linkedNode_t *no_inimigo = search_linked_list(enemyList, index_inimigo);
    if(no_inimigo == NULL){
        fputs("ERROR - function combate: Invalid node\n", stderr);
        fflush(stderr);
        return 0;
    }
    enemy_t inimigo = no_inimigo->data.inimigo;

    // Executa o dano no inimigo na lista
    dano_no_inimigo(20, enemy_position);

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
        return 0;
    }
    
    // Caso o player morra
    else if(personagem->life_points <= 0){
        return 2;
    }

    // Caso só o inimigo morra
    else{
        return 1;
    }

    #ifdef DEBUG
    print_list(enemyList, stdout);
    #endif

    return 1;
}

// Processa a coleta de um item
// Em caso de sucesso retorna 1, em caso de fracasso retorna 0
int coletar_item(character_t *personagem, position_t item_position){
    if(is_position_valid(item_position) == 0){
        fputs("ERROR - function coletar_item: Invalid position\n", stderr);
        fflush(stderr);
        return -1;
    }

    if(board[item_position.y][item_position.x] != 'I'){
        fputs("ERROR - function coletar_item: No item on the assigned position\n", stderr);
        fflush(stderr);
        return -1;
    }

    if(personagem == NULL){
        fputs("ERROR - function coletar_item: Invalid character\n", stderr);
        fflush(stderr);
        return -1;
    }

    int index_item = search_position_linked_list(itemList, item_position);
    linkedNode_t* no_item = search_linked_list(itemList, index_item);
    if(no_item == NULL){
        fputs("ERROR - function coletar_item: Couldn't find item in the list\n", stderr);
        fflush(stderr);
        return 0;
    }
    item_t item = no_item->data.item;
    if(remove_linked_list(itemList, index_item) == 0){
        fputs("ERROR - function coletar_item: Couldn't remove item from list\n", stderr);
        fflush(stderr);
        return 0;
    }

    personagem->mana_points += item.valor;
    log_item_collected(item);

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif

    return 1;
}

// Processa o uso de uma habilidade
// Caso não tenha mana o suficiente, retorna 0
// Caso a habilidade seja usada, retorna 1
// Caso dê erro, retorna -1
int usar_habilidade(character_t *personagem, skill_t skill, char direcao){
    int result;
    if(personagem == NULL){
        fputs("ERROR - function usar_habilidade: pointer to player points to NULL\n", stderr);
        fflush(stderr);
        return -1;
    }

    switch (skill)
    {
    case HEALING_SPELL:
        if(personagem->mana_points >= HEALING_SPELL_COST){
            personagem->mana_points -= HEALING_SPELL_COST;
            feitico_cura(personagem);
            return 1;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", HEALING_SPELL_COST);
            result = 0;
        }
        break;
    
    case LIGHTNING:
        if(personagem->mana_points >= LIGHTNING_SPELL_COST){
            personagem->mana_points -= LIGHTNING_SPELL_COST;
            relampago(personagem, direcao);
            result = 1;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", LIGHTNING_SPELL_COST);
            result = 0;
        }
        break;
    
    case FIREBALL:
        if(personagem->mana_points >= FIREBALL_SPELL_COST){
            personagem->mana_points -= FIREBALL_SPELL_COST;
            bola_de_fogo(personagem, direcao);
            result = 1;
        }

        else{
            printf("\nPontos de mana insuficientes!\n%d pontoss de mana sao necessarios para usar a habilidade\n", FIREBALL_SPELL_COST);
            result = 0;
        }
        break;
    
    default:
        return -1;
        break;
    }

    Sleep(1000);
    return result;
}

// Processa o feitiço de bola de fogo
// Caso dê certo, retorna 1
// Caso dê errado, retorna 0
int bola_de_fogo(character_t *personagem, char direcao){
    if(personagem == NULL){
        fputs("ERROR - function relampago: pointer to player points to NULL\n", stderr);
        fflush(stderr);
        return 0;
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
            return 0;
            break;
        }
        
        if(is_position_valid(current_position) == 0){
            break;
        }

        switch (board[current_position.y][current_position.x])
        {
        case '-':
            
            break;
        
        case 'E':
        {
            if(dano_no_inimigo(FIREBALL_SPELL_DAMAGE, current_position) == 0){
                fputs("ERROR - function relampago: Couldn't process the damage\n", stderr);
                fflush(stderr);
                return 0;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(destruir_entidade(current_position) == 0){
                fputs("ERROR - function relampago: Couldn't destroy the entity\n", stderr);
                fflush(stderr);
                return 0;
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

    return 1;
}

// Processa o feitiço de relâmpago
// Caso dê certo, retorna 1
// Caso dê errado, retorna 0
int relampago(character_t *personagem, char direcao){
    if(personagem == NULL){
        fputs("ERROR - function relampago: pointer to player points to NULL\n", stderr);
        fflush(stderr);
        return 0;
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
            return 0;
            break;
        }
        
        if(is_position_valid(current_position) == 0){
            break;
        }

        switch (board[current_position.y][current_position.x])
        {
        case '-':
            
            break;
        
        case 'E':
        {
            if(dano_no_inimigo(LIGHTNING_SPELL_DAMAGE, current_position) == 0){
                fputs("ERROR - function relampago: Couldn't process the damage\n", stderr);
                fflush(stderr);
                return 0;
            }

            break;
        }
        case 'I':
        case 'X':
        {
            if(destruir_entidade(current_position) == 0){
                fputs("ERROR - function relampago: Couldn't destroy the entity\n", stderr);
                fflush(stderr);
                return 0;
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

    return 1;
}

// Dá o dano especificado no inimigo na posição especificada
// Caso dê certo, retorna 1
// Caso dê errado, retorna 0
int dano_no_inimigo(int dano, position_t position){
    if(is_position_valid(position) == 0){
        fputs("ERROR - function dano_no_inimigo: Invalid position\n", stderr);
        fflush(stderr);
        return 0;
    }

    int index_inimigo = search_position_linked_list(enemyList, position);
    linkedNode_t* no_inimigo = search_linked_list(enemyList, index_inimigo);

    enemy_t* inimigo = &no_inimigo->data.inimigo;

    inimigo->life_points -= dano;

    log_damage((entity_t) *inimigo, ENEMY, dano);
    
    if(inimigo->life_points < 1){
        if(destruir_entidade(position) == 0){
            fputs("ERROR - function dano_no_inimigo: Couldn't destroy the entity\n", stderr);
            fflush(stderr);
            return 0;
        }
    }

    return 1;
}

// Destroi a entidade na posição especificada
// Retorna 1 em caso de sucesso
// Retorna 0 em caso de fracasso
int destruir_entidade(position_t position){
    if(is_position_valid(position) == 0){
        fputs("ERROR - function destruir_entidade: Invalid position\n", stderr);
        fflush(stderr);
        return 0;
    }

    linkedList_t* list = NULL;

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
        return 1;
        break;
    }

    int index = search_position_linked_list(list, position);
    if(remove_linked_list(list, index) == 0){
        fputs("ERROR - function destruir_entidade: Couldn't remove entity from list\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
}

// Processa o feitiço de cura
// Caso dê certo, retorna 1
// Caso dê errado, retorna 0
int feitico_cura(character_t *personagem){
    if(personagem == NULL){
        fputs("ERROR - function feitico_cura: pointer p points to NULL\n", stderr);
        fflush(stderr);
        return 0;
    }

    personagem->life_points += HEALING_SPELL_HEALING;

    return 1;
}

// Processa o movimento dos inimigos
// Caso os movimentos não tenham sido realizados por conta de erro em algum processamento retorna -1,
// Caso os movimentos tenham sido realizados retorna 0,
// Caso o player morra retorna 1
int mover_inimigos(){
    if(enemyList->length == 0) return 0;

    for(int i = 0; i < enemyList->length; i++){
        int resultado_movimento = mover_inimigo(i);

        if(resultado_movimento == -1){
            fputs("ERROR - function mover_inimigos: Couldn't move enemy\n", stderr);
            fflush(stderr);
            return -1;
        }

        else if(resultado_movimento == 3){
            return 1;
        }
    }

    return 0;
}

// Processa o movimento de um inimigo específico
// Caso o movimento não tenha sido realizado por conta de erro em algum processamento retorna -1,
// Caso o movimento tenha sido realizado retorna 0,
// Caso o movimento não tenha sido realizado porque era inválido retorna 1,
// Caso o movimento não tenha sido realizado porque o inimigo morreu retorna 2
// Caso o movimento tenha sido realizado mas o player morreu retorna 3
int mover_inimigo(int index_inimigo){
    linkedNode_t *no_inimigo = search_linked_list(enemyList, index_inimigo);

    if(no_inimigo == NULL){
        fputs("ERROR - function mover_inimigo: Invalid argument\n", stderr);
        fflush(stderr);
        return -1;
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
    if(is_position_valid(next_position) == 0) return 1;

    // Detecção de colisão
    switch (board[next_position.y][next_position.x]){
    case '-':
        no_inimigo->data.inimigo.position = next_position;

        if(atualizar_posicoes() == 0){
            fputs("ERROR - function mover_inimigo: Couldn't update positions\n", stderr);
            fflush(stderr);
            return -1;
        }

        return 0;

        break;
    
    case 'P':
    {
        int resultado_combate = combate(player, inimigo.position, ENEMY);
        if(resultado_combate == -1){
            fputs("ERROR - function mover_inimigo: Couldn't process combat\n", stderr);
            fflush(stderr);
            return -1;
        }

        // Caso ninguém morra
        else if(resultado_combate == 0){
            return 1;
        }

        // Caso o inimigo morra
        else if(resultado_combate == 1){
            return 2;
        }

        // Caso o player morra
        else if(resultado_combate == 2){
            no_inimigo->data.inimigo.position = next_position;

            if(atualizar_posicoes() == 0){
                fputs("ERROR - function mover_inimigo: Couldn't update positions\n", stderr);
                fflush(stderr);
                return -1;
            }

            return 3;
        }

        break;
    }

    case 'E':
    case 'I':
    case 'X':
    default:
        return 1;
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
        
        if(is_tile_enemy_walkable(next_position) == 1){
            return 'W';
        }

        // Se não, tenta se mover para baixo
        next_position.x = i.position.x;
        next_position.y = i.position.y - 1;

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'S';
        }
    }

    // O player está abaixo
    else if(dy <= 0){
        // Move pra baixo se puder
        next_position.x = i.position.x;
        next_position.y = i.position.y - 1;

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'S';
        }

        // Se não, tenta se mover para cima
        next_position.x = i.position.x;
        next_position.y = i.position.y + 1;

        if(is_tile_enemy_walkable(next_position) == 1){
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

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'D';
        }

        // Se não, tenta se mover para esquerda
        next_position.x = i.position.x - 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'A';
        }
    }

    // O player está à esquerda
    else if(dx <= 0){
        // Move pra esquerda se puder
        next_position.x = i.position.x - 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'A';
        }

        // Se não, tenta se mover para direita
        next_position.x = i.position.x + 1;
        next_position.y = i.position.y;

        if(is_tile_enemy_walkable(next_position) == 1){
            return 'D';
        }
    }

    // Se não conseguiu se mover pra lugar nenhum, retorna \0
    return '\0';
}

// Verifica se uma casa do tabuleiro é andável pelo player
// Em caso de ser andável retorna 1, caso contrário retorna 0
int is_tile_player_walkable(position_t position){
    // Verifica se está dentro do tabuleiro
    if(is_position_valid(position) == 0){
        return 0;
    }

    // Verifica se há um obstáculo
    char tile = board[position.y][position.x];
    if(tile == 'X'){
        return 0;
    }

    return 1;
}

// Verifica se uma casa do tabuleiro é andável pelos inimigos
// Em caso de ser andável retorna 1, caso contrário retorna 0
int is_tile_enemy_walkable(position_t position){
    // Verifica se está dentro do tabuleiro
    if(is_position_valid(position) == 0){
        return 0;
    }

    // Verifica se há um obstáculo
    char tile = board[position.y][position.x];
    if(tile == 'X' || tile == 'I' || tile == 'E'){
        return 0;
    }

    return 1;
}

// Verifica se é uma posição válida no tabuleiro
// Em caso de ser retorna 1, caso contrário retorna 0
int is_position_valid(position_t position){
    // Verifica se a posição está dentro dos limites do tabuleiro
    return (
        position.x < 0 || 
        position.x > boardSize - 1 ||
        position.y < 0 ||
        position.y > boardSize - 1
    ) ? 0 : 1;
}

// Adiciona um movimento especificado na fila
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int adicionar_comando_fila(queue_t *fila, char acao, char direcao){
    action_t m = {
        .acao = acao,
        .direcao = direcao
    };

    if(enqueue(fila, (nodeData_t) m) == 0){
        fputs("ERROR - function adicionar_comando_fila: Couldn't enqueue move\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
}

// Processa o próximo movimento na fila
// Em caso do movimento não ser realizado por ter ocorrido erro, retorna -1
// Em caso do movimento ser realizado, retorna 0
// Em caso do movimento não ser realizado por ser inválido, retorna 1
// Em caso do movimento não ser realizado porque o player morreu, retorna 2
int processar_comando_fila(queue_t *fila){
    if(fila->length > 1){
        fputs("ERROR - function processar_comando_fila: Empty queue\n", stderr);
        fflush(stderr);
        return 2;
    }

    nodeData_t movimentoData = dequeue(fila);
    if(isDataEmpty(movimentoData) == 1){
        fputs("ERROR - function processar_comando_fila: Couldn't get next move\n", stderr);
        fflush(stderr);
        return 2;
    }

    action_t m = movimentoData.movimento;

    switch (m.acao)
    {
    case 'M':
        {
            int result = mover_personagem(player, m.direcao);

            if(result == 0){
                log_move(m);
            }

            return result;

            break;
        }
    case 'E':
        /* code */
        break;
    
    default:
        return 1;

        break;
    }

    return 0;
}

// Libera toda memória dinamicamente alocada pelo programa
// Em caso de sucesso retorna 0, em caso de fracasso retorna 1
int liberar_memoria(char **tabuleiro, character_t *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento){
    free(p);

    if(delete_linked_list(enemyList) == 0){
        fputs("ERROR - function liberar_memoria: Couldn't delete enemy list\n", stderr);
        fflush(stderr);
        return 1;
    }

    if(delete_linked_list(itemList) == 0){
        fputs("ERROR - function liberar_memoria: Couldn't delete item list\n", stderr);
        fflush(stderr);
        return 1;
    }

    if(delete_linked_list(obstacleList) == 0){
        fputs("ERROR - function liberar_memoria: Couldn't delete obstacle list\n", stderr);
        fflush(stderr);
        return 1;
    }

    if(delete_queue(moveQueue) == 0){
        fputs("ERROR - function liberar_memoria: Couldn't delete move queue\n", stderr);
        fflush(stderr);
        return 1;
    }

    for(int i = 0; i < boardSize; i++){
        free(tabuleiro[i]);
    }

    free(tabuleiro);

    return 0;
}