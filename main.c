#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>

#define JOGO
#define LINKEDLIST
#define QUEUE
#include "data_structures.c"

#define DEBUG

// Constantes
#define INITIAL_HP 100

// Variáveis globais
char** board;
int boardSize;
Personagem* player;
linkedList_t *enemyList, *itemList, *obstacleList;
queue_t* moveQueue;

int loop();
char** inicializar_tabuleiro(int N);
void mostrar_tabuleiro();
void atualizar_posicoes();
void atualizar_posicoes_de_lista(linkedList_t *lista);
void gerar_inimigos(int quantidade);
void gerar_itens(int quantidade);
void gerar_obstaculos(int quantidade);
Personagem *criar_personagem(int x, int y);
void adicionar_inimigo(linkedList_t *listaInimigo, int x, int y);
void adicionar_item(linkedList_t *listaItem, int x, int y, int valor);
void mover_personagem(Personagem *p, char direcao, char **tabuleiro, linkedList_t *listaItem, linkedList_t *listaInimigo);
void mover_inimigos(linkedList_t *listaInimigo, char **tabuleiro);
void processar_comando_fila(queue_t *filaMovimento, Personagem *p, char **tabuleiro);
void liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, queue_t *filaMovimento);

int main(){
    // Inicializa as estruturas de dados
    enemyList = create_linked_list(ENEMY_TYPE);
    itemList = create_linked_list(ITEM_TYPE);
    obstacleList = create_linked_list(OBSTACLE_TYPE);
    moveQueue = create_queue(MOVE_TYPE);

    // Inicializa os números aleatórios
    srand(time(NULL));

    #ifndef DEBUG
    printf("Digite o numeros de linhas e colunas do tabuleiro: ");
    fflush(stdout);
    scanf("%d", &boardSize);

    board = inicializar_tabuleiro(boardSize);

    int posicao[2] = {0};

    printf("Digite a posicao x do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[0]));

    printf("Digite a posicao y do personagem: ");
    fflush(stdout);
    scanf("%d", &(posicao[1]));

    player = criar_personagem(posicao[0], posicao[1]);

    gerar_inimigos(3);

    gerar_itens(5);

    gerar_obstaculos(4);

    while(loop());

    #endif

    #ifdef DEBUG
    #define BOARDSZ 10
    #define PLAYERPOSX 0
    #define PLAYERPOSY 0

    boardSize = BOARDSZ;

    board = inicializar_tabuleiro(boardSize);

    player = criar_personagem(PLAYERPOSX, PLAYERPOSY);

    gerar_inimigos(2);
    gerar_itens(2);
    gerar_obstaculos(2);

    while(loop());

    #endif

    delete_linked_list(enemyList);
    delete_linked_list(itemList);
    delete_linked_list(obstacleList);
    delete_queue(moveQueue);

    return 0;
}

int loop(){
    atualizar_posicoes();
    mostrar_tabuleiro(board);

    char acao, direcao;
    
    printf("Acao ('M' para movimento, 'A' para ataque, 'Q' para sair do programa): \n");
    fflush(stdout);
    
    while(!_kbhit());
    acao = _getch();
    acao = toupper(acao);

    if(acao == 'Q') return 0;
    if(!(acao == 'M' || acao == 'A')) return 1;

    printf("Direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): \n");
    fflush(stdout);

    while(!_kbhit());
    direcao = _getch();
    direcao = toupper(direcao);

    switch (acao)
    {
    case 'M':
        mover_personagem(player, direcao, board, itemList, enemyList);
        break;
    
    case 'A':
        /* TODO: Função para processar o ataque*/
        break;
    
    default:
        break;
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

void mostrar_tabuleiro(){
    #ifndef DEBUG
    system("cls");
    #endif
    printf("Vida do personagem: %d \tPontuacao do personagem: %d\n\n", player->vida, player->pontos);
    for(int i = boardSize - 1; i > - 1; i--){
        for(int j = 0; j < boardSize; j++){
            printf(" %c", board[i][j]);
        }
        printf("\n\0");
    }
    printf("\n\n");
}

void atualizar_posicoes(){
    // Reseta o tabuleiro
    for(int i = 0; i < boardSize; i++){
        for(int j = 0; j < boardSize; j++){
            board[i][j] = '-';
        }
    }

    // Põe o player no tabuleiro
    board[player->y][player->x] = 'P';
    
    // Põe os inimigos no tabuleiro
    atualizar_posicoes_de_lista(enemyList);

    // Põe os itens no tabuleiro
    atualizar_posicoes_de_lista(itemList);

    // Põe os obstáculos no tabuleiro
    atualizar_posicoes_de_lista(obstacleList);
}

void atualizar_posicoes_de_lista(linkedList_t *lista){
    // Começa a percorrer a lista pela cabeça da lista
    linkedNode_t* currentNode = lista->head;
    if(currentNode == NULL){
        fputs("ERROR - function atualizar_posicoes_de_lista: List head points to NULL\n", stderr);
        fflush(stderr);
        return;
    }
    nodeData_t currentData;

    for(int i = 0; i < enemyList->length; i++){
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
            return;
            break;
        }

        // Passa pro próximo nó
        currentNode = currentNode->next;
    }
}

void gerar_inimigos(int quantidade){
    Inimigo newEnemy;
    for(int i = 0; i < quantidade; i++){
        int posX, posY;
        do{
            posX = rand() % (boardSize);
            posY = rand() % (boardSize);

        }while(board[posY][posX] != '-');

        adicionar_inimigo(enemyList, posX, posY);
    }

    #ifdef DEBUG
    print_list(enemyList);
    #endif
}

void gerar_itens(int quantidade){
    for(int i = 0; i < quantidade; i++){
        int posX, posY;

        int valor = (rand() + 5) % 101;

        // Gera novas posições, até achar uma desocupada
        do{
            posX = rand() % (boardSize);
            posY = rand() % (boardSize);
        }while(board[posY][posX] != '-');

        adicionar_item(itemList, posX, posY, valor);
    }

    #ifdef DEBUG
    print_list(itemList);
    #endif
}

void gerar_obstaculos(int quantidade){
    Obstaculo newObstacle;
    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
        newObstacle.x = rand() % (boardSize);
        newObstacle.y = rand() % (boardSize);
        }while(board[newObstacle.y][newObstacle.x] != '-');

        board[newObstacle.y][newObstacle.x] = 'X';

        insert_linked_list(obstacleList, (nodeData_t) newObstacle, -1);
    }

    #ifdef DEBUG
    print_list(obstacleList);
    #endif
}

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

    // Põe o player no tabuleiro
    board[personagem_novo->y][personagem_novo->x] = 'P';

    return personagem_novo;
}

void adicionar_inimigo(linkedList_t *listaInimigo, int x, int y){
    Inimigo newEnemy = {
        .vida = (rand() + 5) % 51,
        .x = x,
        .y = y
    };

    board[newEnemy.y][newEnemy.x] = 'E';
    insert_linked_list(listaInimigo, (nodeData_t) newEnemy, -1);
}

void adicionar_item(linkedList_t *listaItem, int x, int y, int valor){
    Item newItem = {
        .valor = valor,
        .x = x,
        .y = y
    };

    board[newItem.y][newItem.x] = 'I';

    insert_linked_list(listaItem, (nodeData_t) newItem, -1);
}

void mover_personagem(Personagem *p, char direcao, char **tabuleiro, linkedList_t *listaItem, linkedList_t *listaInimigo){
    if(!(direcao == 'W' || direcao == 'A' || direcao == 'S' || direcao == 'D')){
        fputs("ERROR - function mover_personagem: Invalid input\n", stderr);
        fflush(stderr);
        return;
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

    if(nextPositionX < 0) nextPositionX = 0;
    if(nextPositionX > boardSize - 1) nextPositionX = boardSize - 1;
    if(nextPositionY < 0) nextPositionY = 0;
    if(nextPositionY > boardSize - 1) nextPositionY = boardSize - 1;

    int index;

    // Detecção de colisão
    switch (board[nextPositionY][nextPositionX]){
    case '-':
        p->x = nextPositionX;
        p->y = nextPositionY;

        Movimento newMove = {
            .acao = 'M',
            .direcao = direcao
        };

        enqueue(moveQueue, (nodeData_t) newMove);

        break;
    
    case 'E':
        index = search_position_linked_list(enemyList, nextPositionX, nextPositionY);
        Inimigo inimigo = search_linked_list(enemyList, index)->data.inimigo;

        #ifdef DEBUG
        print_enemy(inimigo);
        #endif

        p->x = nextPositionX;
        p->y = nextPositionY;

        p->vida -= inimigo.vida;

        remove_linked_list(enemyList, index);
        break;
    
    case 'I':
        index = search_position_linked_list(itemList, nextPositionX, nextPositionY);
        Item item = search_linked_list(itemList, index)->data.item;

        #ifdef DEBUG
        print_item(item);
        #endif

        p->x = nextPositionX;
        p->y = nextPositionY;

        p->pontos += item.valor;

        remove_linked_list(itemList, index);
        break;
    
    case 'X':
        #ifdef DEBUG
        index = search_position_linked_list(obstacleList, nextPositionX, nextPositionY);
        Obstaculo obstaculo = search_linked_list(obstacleList, index)->data.obstaculo;
        print_obstacle(obstaculo);
        #endif
        break;
    
    case 'P':
        break;

    default:
        break;
    }

}

void coletar_item(){

}

void mover_inimigos(linkedList_t *listaInimigo, char **tabuleiro){

}

void processar_comando_fila(queue_t *filaMovimento, Personagem *p, char **tabuleiro){

}

void liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, queue_t *filaMovimento){

}