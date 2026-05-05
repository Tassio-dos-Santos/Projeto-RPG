#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#define JOGO
#define LINKEDLIST
#define QUEUE
#include "data_structures.c"

// #define DEBUG

#ifdef DEBUG

#define INIMIGOS 1
#define ITENS 1
#define OBSTACULOS 0

#endif

// Constantes
#define INITIAL_HP 100
#define MAXBOARDSZ 30
#define MINBOARDSZ 4
#define RAIOINIMIGO 2

// Variáveis globais
char** board;
int boardSize;
//int atualizarTabuleiro = 1;
Personagem* player;
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
pthread_mutex_t log_queue_mutex;
pthread_mutex_t board_mutex;

// Semáforos
sem_t log_queue_sem;
sem_t render_sem;

int loop();
char** inicializar_tabuleiro(int N);

void *renderer();
void render_board();
void mostrar_tabuleiro();

int atualizar_posicoes();
int atualizar_posicoes_de_lista(linkedList_t *lista);
int gerar_inimigos(int quantidade);
int gerar_itens(int quantidade);
int gerar_obstaculos(int quantidade);
Personagem *criar_personagem(int x, int y);
int adicionar_inimigo(linkedList_t *listaInimigo, int x, int y);
int adicionar_item(linkedList_t *listaItem, int x, int y, int valor);
int adicionar_obstaculo(linkedList_t *listaObstaculo, int x, int y);
int mover_personagem(Personagem *p, char direcao);
int combate(Personagem *p, int indexInimigo);
int coletar_item(Personagem *p, int indexInimigo);
int mover_inimigos();
int mover_inimigo(int indexInimigo);
char movimento_aleatorio();
char movimento_perseguir(Inimigo i);
char movimento_simples_vertical(Inimigo i, int dx, int dy);
char movimento_simples_horizontal(Inimigo i, int dx, int dy);
int is_tile_player_walkable(int x, int y);
int is_tile_enemy_walkable(int x, int y);
int adicionar_comando_fila(queue_t *fila, char acao, char direcao);
int processar_comando_fila(queue_t *fila);

void *logger();
int log_move(Movimento m);
int log_item_collected(Item i);
int log_combat(Inimigo i);

int liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento);

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

    if(boardSize > MAXBOARDSZ) boardSize = MAXBOARDSZ;
    if(boardSize < MINBOARDSZ) boardSize = MINBOARDSZ;

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

    player = criar_personagem(posicao[0], posicao[1]);

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

    player = criar_personagem(PLAYERPOSX, PLAYERPOSY);

    // gerar_inimigos(INIMIGOS);

    adicionar_inimigo(enemyList, 9, 9);
    // gerar_obstaculos(OBSTACULOS);
    adicionar_obstaculo(obstacleList, 5, 9);
    adicionar_obstaculo(obstacleList, 5, 8);
    adicionar_obstaculo(obstacleList, 5, 7);
    adicionar_obstaculo(obstacleList, 5, 6);
    adicionar_obstaculo(obstacleList, 5, 5);
    adicionar_obstaculo(obstacleList, 5, 4);
    adicionar_obstaculo(obstacleList, 5, 3);

    gerar_itens(ITENS);

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
    if(player->vida < 1){
        printf("Voce perdeu.\n");

        return 0;
    }
    
    #ifndef DEBUG
    printf("Acao ('WASD' para movimento, 'E' para ataque, 'Q' para sair do programa): \n");
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

    printf("Direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): \n");
    fflush(stdout);

    while(!_kbhit());
    input = _getch();
    input = toupper(input);

    #endif

    #ifdef DEBUG

    Sleep(500);
    input = 'S';

    #endif

    switch (input)
    {
    case 'W':
        /* TODO: Função para processar o ataque*/
        break;
    
    case 'A':
        /* TODO: Função para processar o ataque*/
        break;
    
    case 'S':
        /* TODO: Função para processar o ataque*/
        break;
    
    case 'D':
        /* TODO: Função para processar o ataque*/
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

void *renderer(){
    while(!end_renderer){
        sem_wait(&render_sem);
        if(end_renderer) break;

        pthread_mutex_lock(&board_mutex);
        if(end_renderer) break;

        render_board();
        display_updated = 1;

        pthread_mutex_unlock(&board_mutex);
    }
}

void render_board(){
    #ifndef DEBUG
    system("cls");
    #endif
    printf("Vida do personagem: %d \tPontuacao do personagem: %d\n\n", player->vida, player->pontos);
    for(int i = boardSize - 1; i > - 1; i--){
        for(int j = 0; j < boardSize; j++){
            switch (board[i][j])
            {
            case 'P':
                printf("\x1b[32m");
                break;
            
            case 'E':
                printf("\x1b[31m");
                break;
            
            case 'I':
                printf("\x1b[34m");
                break;
            
            case 'X':
                printf("\x1b[30m");
                break;
            
            default:
                break;
            }
            printf(" %c\x1b[0m", board[i][j]);
        }
        printf("\n\0");
    }
    printf("\n\n");
}

// Limpa a tela e mostrar as informações do personagem e o tabuleiro
void mostrar_tabuleiro(){
    display_updated = 0;
    sem_post(&render_sem);
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
    board[player->y][player->x] = 'P';
    
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
    linkedNode_t* currentNode = lista->head;
    if(currentNode == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: List head points to NULL\n", stderr);
        fflush(stderr);
        return 0;
    }
    nodeData_t currentData;

    for(int i = 0; i < lista->length; i++){
        // Define o currentData como o dado no nó atual
        currentData = currentNode->data;

        switch (lista->listType){
        case CHARACTER_TYPE:
            // Caso o tipo da lista for de personagem, coloca 'P' na posição do personagem atual
            board[currentData.personagem.y][currentData.personagem.x] = 'P';
            break;
        
        case ENEMY_TYPE:
            // Caso o tipo da lista for de inimigo, coloca 'E' na posição do inimigo atual
            board[currentData.inimigo.y][currentData.inimigo.x] = 'E';
            break;
        
        case ITEM_TYPE:
            // Caso o tipo da lista for de item, coloca 'I' na posição do item atual
            board[currentData.item.y][currentData.item.x] = 'I';
            break;
        
        case OBSTACLE_TYPE:
            // Caso o tipo da lista for de obstaculo, coloca 'X' na posição do obstaculo atual
            board[currentData.obstaculo.y][currentData.obstaculo.x] = 'X';
            break;
        
        default:
            fputs("ERROR - function atualizar_posicoes_de_lista: Invalid list type\n", stderr);
            fflush(stderr);
            return 0;
            break;
        }

        // Passa pro próximo nó
        currentNode = currentNode->next;
    }
}

// Gera inimigos em posições aleatórias na lista de inimigos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int gerar_inimigos(int quantidade){
    Inimigo newEnemy;
    for(int i = 0; i < quantidade; i++){
        int posX, posY;

        do{
            posX = rand() % (boardSize);
            posY = rand() % (boardSize);

        }while(board[posY][posX] != '-');

        if(adicionar_inimigo(enemyList, posX, posY) == 0){
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
        int posX, posY;

        int valor = (rand() + 5) % 101;

        // Gera novas posições, até achar uma desocupada
        do{
            posX = rand() % (boardSize);
            posY = rand() % (boardSize);
        }while(board[posY][posX] != '-');

        if(adicionar_item(itemList, posX, posY, valor) == 0){
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
    int posX, posY;

    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
            posX = rand() % (boardSize);
            posY = rand() % (boardSize);
        }while(board[posY][posY] != '-');

        if(adicionar_obstaculo(obstacleList, posX, posY) == 0){
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
Personagem *criar_personagem(int x, int y){
    Personagem *personagem_novo = (Personagem *) malloc(sizeof(Personagem));
    if(personagem_novo == NULL){
        fputs("ERROR - function criar_personagem: Couldn't allocate memory\n", stderr);
        fflush(stderr);
        return NULL;
    }

    personagem_novo->x = x;
    personagem_novo->y = y;
    personagem_novo->pontos = 0;
    personagem_novo->vida = INITIAL_HP;

    pthread_mutex_lock(&board_mutex);
    // Põe o player no tabuleiro
    board[personagem_novo->y][personagem_novo->x] = 'P';
    pthread_mutex_unlock(&board_mutex);

    return personagem_novo;
}

// Adiciona um inimigo com vida aleatória na posição especificada na lista de inimigos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_inimigo(linkedList_t *listaInimigo, int x, int y){
    Inimigo newEnemy = {
        .vida = (rand() + 5) % 51,
        .x = x,
        .y = y
    };

    pthread_mutex_lock(&board_mutex);
    board[newEnemy.y][newEnemy.x] = 'E';
    pthread_mutex_unlock(&board_mutex);

    if(insert_linked_list(listaInimigo, (nodeData_t) newEnemy, -1) == 0){
        fputs("ERROR - function adicionar_inimigo: Couldn't add enemy to the list\n", stderr);
        fflush(stderr);
        return 0;
    }
}

// Adiciona um item com valor especificado na posição especificada na lista de itens
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_item(linkedList_t *listaItem, int x, int y, int valor){
    Item newItem = {
        .valor = valor,
        .x = x,
        .y = y
    };

    pthread_mutex_lock(&board_mutex);
    board[newItem.y][newItem.x] = 'I';
    pthread_mutex_unlock(&board_mutex);

    if(insert_linked_list(listaItem, (nodeData_t) newItem, -1) == 0){
        fputs("ERROR - function adicionar_item: Couldn't add item to the list\n", stderr);
        fflush(stderr);
        return 0;
    }
}

// Adiciona um obstaculo posição especificada na lista de obstaculos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_obstaculo(linkedList_t *listaObstaculo, int x, int y){
    Obstaculo newObstacle = {
        .x = x,
        .y = y
    };

    pthread_mutex_lock(&board_mutex);
    board[newObstacle.y][newObstacle.x] = 'X';
    pthread_mutex_unlock(&board_mutex);

    if(insert_linked_list(listaObstaculo, (nodeData_t) newObstacle, -1) == 0){
        fputs("ERROR - function adicionar_obstaculo: Couldn't add obstacle to the list\n", stderr);
        fflush(stderr);
        return 0;
    }
}

// Processa o movimento do player
// Caso o movimento não tenha sido realizado por conta de erro em algum processamento retorna -1,
// Caso o movimento tenha sido realizado retorna 0,
// Caso o movimento não tenha sido realizado porque era inválido retorna 1,
// Caso o movimento não tenha sido realizado porque o player morreu retorna 2
int mover_personagem(Personagem *p, char direcao){
    if(!(direcao == 'W' || direcao == 'A' || direcao == 'S' || direcao == 'D')){
        fputs("ERROR - function mover_personagem: Invalid input\n", stderr);
        fflush(stderr);
        return 1;
    }

    int nextPositionX = p->x, nextPositionY = p->y;
    switch (direcao)
    {
    case 'W':
        nextPositionY++;
        break;
    
    case 'A':
        nextPositionX--;
        break;
    
    case 'S':
        nextPositionY--;
        break;
    
    case 'D':
        nextPositionX++;
        break;
    
    default:
        break;
    }

    // Impede que o player saia dos limites do tabuleiro
    if(
        nextPositionX < 0 || 
        nextPositionX > boardSize - 1 ||
        nextPositionY < 0 ||
        nextPositionY > boardSize - 1
    ) return 1;

    // Detecção de colisão
    switch (board[nextPositionY][nextPositionX]){
    case '-':
        break;
    
    case 'E':
    {
        int index = search_position_linked_list(enemyList, nextPositionX, nextPositionY);
        if(index == -1){
            fputs("ERROR - function mover_personagem: Couldn't find enemy in the list\n", stderr);
            fflush(stderr);
            return -1;
        }

        int resultado_combate = combate(p, index);
        if(resultado_combate == -1){
            fputs("ERROR - function mover_personagem: Couldn't process combat\n", stderr);
            fflush(stderr);
            return -1;
        }

        // Inimigo não morreu
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
        int index = search_position_linked_list(itemList, nextPositionX, nextPositionY);
        if(index == -1){
            fputs("ERROR - function mover_personagem: Couldn't find item in the list\n", stderr);
            fflush(stderr);
            return -1;
        }
        if(coletar_item(p, index) == 0){
            fputs("ERROR - function mover_personagem: Couldn't process item colection\n", stderr);
            fflush(stderr);
            return -1;
        }

        break;
    }
    case 'X':
    {
        #ifdef DEBUG
        int index = search_position_linked_list(obstacleList, nextPositionX, nextPositionY);
        Obstaculo obstaculo = search_linked_list(obstacleList, index)->data.obstaculo;
        print_obstacle(obstaculo, stdout);
        #endif
    }
    case 'P':
    default:
        return 1;
        break;
    }

    // Se chegou até essa parte do código (não passou por return), o movimento é executado

    p->x = nextPositionX;
    p->y = nextPositionY;

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
int combate(Personagem *p, int indexInimigo){
    linkedNode_t* noInimigo = search_linked_list(enemyList, indexInimigo);
    if(noInimigo == NULL){
        fputs("ERROR - function combate: Couldn't find enemy in the list\n", stderr);
        fflush(stderr);
        return -1;
    }

    p->vida -= 10;
    noInimigo->data.inimigo.vida -= 20;

    Inimigo inimigo = noInimigo->data.inimigo;

    log_combat(inimigo);

    // Caso o inimigo morra, já o deleta da lista
    if(inimigo.vida <= 0){
        if(remove_linked_list(enemyList, indexInimigo) == 0){
            fputs("ERROR - function combate: Couldn't remove enemy from list\n", stderr);
            fflush(stderr);
            return -1;
        }
    }

    // Caso os dois fiquem vivos
    if(p->vida > 0 && inimigo.vida > 0){
        return 0;
    }
    
    // Caso o player morra
    else if(p->vida <= 0){
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
int coletar_item(Personagem *p, int indexItem){
    linkedNode_t* noItem = search_linked_list(itemList, indexItem);
    if(noItem == NULL){
        fputs("ERROR - function coletar_item: Couldn't find item in the list\n", stderr);
        fflush(stderr);
        return 0;
    }
    Item item = noItem->data.item;

    if(remove_linked_list(itemList, indexItem) == 0){
        fputs("ERROR - function coletar_item: Couldn't remove item from list\n", stderr);
        fflush(stderr);
        return 0;
    }

    p->pontos += item.valor;
    log_item_collected(item);

    #ifdef DEBUG
    print_list(itemList, stdout);
    #endif

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
int mover_inimigo(int indexInimigo){
    linkedNode_t *noInimigo = search_linked_list(enemyList, indexInimigo);

    if(noInimigo == NULL){
        fputs("ERROR - function mover_inimigo: Invalid argument\n", stderr);
        fflush(stderr);
        return -1;
    }

    Inimigo inimigo = noInimigo->data.inimigo;

    int nextPositionX = inimigo.x, nextPositionY = inimigo.y;

    // Lógica de decisão de movimento
    char direcao;

    // Distância entre o player e o inimigo
    int dx = player->x - inimigo.x;
    int dy = player->y - inimigo.y;
    double dis = sqrt(dx*dx + dy*dy);

    // Se o player está dentro do raio de visão do inimigo, o inimigo o persegue
    if(dis <= RAIOINIMIGO){
        direcao = movimento_perseguir(inimigo);
    }
    // Se não estiver ele se move aleatoriamente
    else{
        direcao = movimento_aleatorio();
    }

    switch (direcao)
    {
    case 'W':
        nextPositionY++;
        break;
    
    case 'A':
        nextPositionX--;
        break;
    
    case 'S':
        nextPositionY--;
        break;
    
    case 'D':
        nextPositionX++;
        break;
    
    default:
        break;
    }

    // Impede que o inimigo saia dos limites do tabuleiro
    if(
        nextPositionX < 0 || 
        nextPositionX > boardSize - 1 ||
        nextPositionY < 0 ||
        nextPositionY > boardSize - 1
    ) return 1;

    // Detecção de colisão
    switch (board[nextPositionY][nextPositionX]){
    case '-':
        noInimigo->data.inimigo.x = nextPositionX;
        noInimigo->data.inimigo.y = nextPositionY;

        if(atualizar_posicoes() == 0){
            fputs("ERROR - function mover_inimigo: Couldn't update positions\n", stderr);
            fflush(stderr);
            return -1;
        }

        return 0;

        break;
    
    case 'P':
    {
        int resultado_combate = combate(player, indexInimigo);
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
            noInimigo->data.inimigo.x = nextPositionX;
            noInimigo->data.inimigo.y = nextPositionY;

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

char movimento_perseguir(Inimigo i){
    int dx = player->x - i.x;
    int dy = player->y - i.y;

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

char movimento_simples_vertical(Inimigo i, int dx, int dy){
    // O player está acima
    if(dy > 0){
        // Move pra cima se puder
        if(is_tile_enemy_walkable(i.x, i.y + 1) == 1){
            return 'W';
        }

        // Se não, tenta se mover para baixo
        if(is_tile_enemy_walkable(i.x, i.y - 1) == 1){
            return 'S';
        }
    }

    // O player está abaixo
    else if(dy <= 0){
        // Move pra baixo se puder
        if(is_tile_enemy_walkable(i.x, i.y - 1) == 1){
            return 'S';
        }

        // Se não, tenta se mover para cima
        if(is_tile_enemy_walkable(i.x, i.y + 1) == 1){
            return 'W';
        }
    }

    // Se não conseguiu se mover pra lugar nenhum, retorna \0
    return '\0';
}

char movimento_simples_horizontal(Inimigo i, int dx, int dy){
    // O player está à direita
    if(dx > 0){
        // Move pra direita se puder
        if(is_tile_enemy_walkable(i.x + 1, i.y) == 1){
            return 'D';
        }

        // Se não, tenta se mover para esquerda
        if(is_tile_enemy_walkable(i.x - 1, i.y) == 1){
            return 'A';
        }
    }

    // O player está à esquerda
    else if(dx <= 0){
        // Move pra esquerda se puder
        if(is_tile_enemy_walkable(i.x - 1, i.y) == 1){
            return 'A';
        }

        // Se não, tenta se mover para direita
        if(is_tile_enemy_walkable(i.x + 1, i.y) == 1){
            return 'D';
        }
    }

    // Se não conseguiu se mover pra lugar nenhum, retorna \0
    return '\0';
}

// Verifica se uma casa do tabuleiro é andável pelo player
// Em caso de ser andável retorna 1, caso contrário retorna 0
int is_tile_player_walkable(int x, int y){
    // Verifica se está dentro do tabuleiro
    if(x > boardSize - 1 || x < 0 || y > boardSize - 1 || y < 0){
        return 0;
    }

    // Verifica se há um obstáculo
    if(board[y][x] == 'X'){
        return 0;
    }

    return 1;
}

// Verifica se uma casa do tabuleiro é andável pelos inimigos
// Em caso de ser andável retorna 1, caso contrário retorna 0
int is_tile_enemy_walkable(int x, int y){
    // Verifica se está dentro do tabuleiro
    if(x > boardSize - 1 || x < 0 || y > boardSize - 1 || y < 0){
        return 0;
    }

    // Verifica se há um obstáculo
    if(board[y][x] == 'X' || board[y][x] == 'I' || board[y][x] == 'E'){
        return 0;
    }

    return 1;
}

// Adiciona um movimento especificado na fila
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int adicionar_comando_fila(queue_t *fila, char acao, char direcao){
    Movimento m = {
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

    Movimento m = movimentoData.movimento;

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

void *logger(){
    // Inicia o arquivo de log
    FILE *log_file = fopen("log.txt", "w");

    while(!end_logger){
        // Tira valor do semáforo
        sem_wait(&log_queue_sem);
        if(end_logger) break;

        // Pega o mutex da log queue
        pthread_mutex_lock(&log_queue_mutex);
        if(end_logger) break;

        // Se a log queue estiver vazia
        if(logQueue->length < 1){
            // Devolve o mutex
            pthread_mutex_unlock(&log_queue_mutex);

            // E volta ao início do loop
            continue;
        }

        // Pega o dado da frente da fila
        nodeData_t dataEvento = dequeue(logQueue);

        // Verifica se o dado está vazio
        if(isDataEmpty(dataEvento) == 1){
            // Devolve o mutex
            pthread_mutex_unlock(&log_queue_mutex);

            // E volta ao início do loop
            continue;
        }

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
        if(print_event(evento, log_file) == -1){
            fputs("ERROR - thread logger: Couldn't write log\n", stderr);
            fflush(stderr);
            return NULL;
        }

        // Libera a memória dos ponteiros das entidades
        free(evento.main_entity);
        free(evento.secundary_entity);

        // Devolve o mutex da log queue
        pthread_mutex_unlock(&log_queue_mutex);
    }

    fclose(log_file);
}

// Loga um movimento num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_move(Movimento m){
    // Aloca um espaço na memória para guardar os dados da entidade principal (player) na fila de log
    Personagem *p = (Personagem *) malloc(sizeof(Personagem));
    memcpy(p, player, sizeof(Personagem));

    // Cria o evento
    event_t movimento = {
        .type = MOVE,
        .main_entity = p,
        .secundary_entity = NULL
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(enqueue(logQueue, (nodeData_t) movimento) == 0){
        fputs("ERROR - function log_move: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return 0;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return 1;
}

// Loga um item coletado num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_item_collected(Item item){
    // Aloca um espaço na memória para guardar os dados da entidade principal (player) na fila de log
    Personagem *p = (Personagem *) malloc(sizeof(Personagem));
    memcpy(p, player, sizeof(Personagem));

    // Aloca um espaço na memória para guardar os dados da entidade secundária (item) na fila de log
    Item *i = (Item *) malloc(sizeof(Item));
    memcpy(i, &item, sizeof(Item));

    event_t item_coletado = {
        .type = ITEM_COLLECTED,
        .main_entity = p,
        .secundary_entity = i
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(enqueue(logQueue, (nodeData_t) item_coletado) == 0){
        fputs("ERROR - function log_item_collected: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return 0;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return 1;
}

// Loga um combate num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_combat(Inimigo inimigo){
    // Aloca um espaço na memória para guardar os dados da entidade principal (player) na fila de log
    Personagem *p = (Personagem *) malloc(sizeof(Personagem));
    memcpy(p, player, sizeof(Personagem));

    // Aloca um espaço na memória para guardar os dados da entidade secundária (inimigo) na fila de log
    Inimigo *i = (Inimigo *) malloc(sizeof(Inimigo));
    memcpy(i, &inimigo, sizeof(Inimigo));

    event_t combate = {
        .type = COMBAT,
        .main_entity = p,
        .secundary_entity = i
    };

    // Pega o mutex para usar a log queue
    pthread_mutex_lock(&log_queue_mutex);

    // Bota o evento na fila
    if(enqueue(logQueue, (nodeData_t) combate) == 0){
        fputs("ERROR - function log_combat: Couldn't enqueue log\n", stderr);
        fflush(stderr);
        return 0;
    }

    // Devolve o mutex de usar a log queue
    pthread_mutex_unlock(&log_queue_mutex); 

    // Posta no semáforo da log queue para sinalizar que há mais um evento na fila
    sem_post(&log_queue_sem);

    return 1;
}

// Libera toda memória dinamicamente alocada pelo programa
// Em caso de sucesso retorna 0, em caso de fracasso retorna 1
int liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento){
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