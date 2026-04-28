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

// #define DEBUG

// Constantes
#define INITIAL_HP 100

// Variáveis globais
char** board;
int boardSize;
int atualizarTabuleiro = 1;
Personagem* player;
linkedList_t *enemyList, *itemList, *obstacleList;
queue_t* moveQueue;

FILE* log_file;

int loop();
char** inicializar_tabuleiro(int N);
void mostrar_tabuleiro();
int atualizar_posicoes();
int atualizar_posicoes_de_lista(linkedList_t *lista);
int gerar_inimigos(int quantidade);
int gerar_itens(int quantidade);
int gerar_obstaculos(int quantidade);
Personagem *criar_personagem(int x, int y);
int adicionar_inimigo(linkedList_t *listaInimigo, int x, int y);
int adicionar_item(linkedList_t *listaItem, int x, int y, int valor);
int mover_personagem(Personagem *p, char direcao);
int combate(Personagem *p, int indexInimigo);
int coletar_item(Personagem *p, int indexInimigo);
int mover_inimigos(linkedList_t *listaInimigo, char **tabuleiro);
int adicionar_comando_fila(queue_t *fila, char acao, char direcao);
int processar_comando_fila(queue_t *fila);
int log_action(FILE *logFile, Movimento m);
int log_item_collected(FILE *logFile, Item i);
int log_combat(FILE *logFile, Inimigo i);
int liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento, FILE *logFile);

int main(){
    // Inicializa as estruturas de dados
    enemyList = create_linked_list(ENEMY_TYPE);
    itemList = create_linked_list(ITEM_TYPE);
    obstacleList = create_linked_list(OBSTACLE_TYPE);
    moveQueue = create_queue(MOVE_TYPE);

    // Inicializa o arquivo de log
    log_file = fopen("log.txt", "w");

    // Inicializa os números aleatórios
    srand(time(NULL));

    system("cls");

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

    return liberar_memoria(board, player, enemyList, itemList, obstacleList, moveQueue, log_file);
}

int loop(){
    char input;

    if(atualizarTabuleiro == 1){
        atualizar_posicoes();
        mostrar_tabuleiro(board);

        printf("Acao ('WASD' para movimento, 'E' para ataque, 'Q' para sair do programa): \n");
        fflush(stdout);
    }
    
    while(!_kbhit());
    input = _getch();
    input = toupper(input);

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

            int resultadoMovimentacao = processar_comando_fila(moveQueue);

            // Caso o movimento não tenha sido realizado por conta de um erro, o loop acaba
            if(resultadoMovimentacao == 2){
                fputs("ERROR - function loop: Couldn't move character\n", stderr);
                fflush(stderr);
                return 0;
            }
            // Caso o movimento não tenha sido realizado porque ele era invalido, não atualiza a tabuleiro
            else if(resultadoMovimentacao == 1){
                atualizarTabuleiro = 0;
            }
            // Caso o movimento tenha sido realizado, atualiza a tabuleiro
            else if(resultadoMovimentacao == 0){
                atualizarTabuleiro = 1;
            }
            
            return 1;
        }
    
    case 'E':
        break;
    
    default:
        atualizarTabuleiro = 0;
        return 1;
        break;
    }

    // Essa parte do loop só será executada caso seja selecionado ataque
    
    printf("Direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): \n");
    fflush(stdout);

    while(!_kbhit());
    input = _getch();
    input = toupper(input);

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

    atualizarTabuleiro = 1;

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

// Limpa a tela e mostrar as informações do personagem e o tabuleiro
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

// Atualiza o tabuleiro (matriz de caracteres) de acordo com as posições das entidades nas listas
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int atualizar_posicoes(){
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
        return 0;
    }

    // Põe os itens no tabuleiro
    if(atualizar_posicoes_de_lista(itemList) == 0){
        fputs("ERROR - function atualizar_posicoes: Couldn't update items' position\n", stderr);
        fflush(stderr);
        return 0;
    }

    // Põe os obstáculos no tabuleiro
    if(atualizar_posicoes_de_lista(obstacleList) == 0){
        fputs("ERROR - function atualizar_posicoes: Couldn't update obstacles' position\n", stderr);
        fflush(stderr);
        return 0;
    }

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
    print_list(enemyList);
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
    print_list(itemList);
    #endif
}

// Gera obstáculos em posições aleatórias na lista de obstáculos
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int gerar_obstaculos(int quantidade){
    Obstaculo newObstacle;
    for(int i = 0; i < quantidade; i++){
        // Gera novas posições, até achar uma desocupada
        do{
            newObstacle.x = rand() % (boardSize);
            newObstacle.y = rand() % (boardSize);
        }while(board[newObstacle.y][newObstacle.x] != '-');

        board[newObstacle.y][newObstacle.x] = 'X';

        if(insert_linked_list(obstacleList, (nodeData_t) newObstacle, -1) == 0){
            fputs("ERROR - function gerar_obstaculos: Couldn't add obstacle to the list\n", stderr);
            fflush(stderr);
            return 0;
        }
    }

    #ifdef DEBUG
    print_list(obstacleList);
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

    // Põe o player no tabuleiro
    board[personagem_novo->y][personagem_novo->x] = 'P';

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

    board[newEnemy.y][newEnemy.x] = 'E';
    if(insert_linked_list(listaInimigo, (nodeData_t) newEnemy, -1) == 0){
        fputs("ERROR - function adicionar_inimigo: Couldn't add enemy to the list\n", stderr);
        fflush(stderr);
        return 0;
    }
}

// Adiciona um item com valor aleatória na posição especificada na lista de itens
// Em caso de sucesso retorna 1, em caso de falha retorna 0
int adicionar_item(linkedList_t *listaItem, int x, int y, int valor){
    Item newItem = {
        .valor = valor,
        .x = x,
        .y = y
    };

    board[newItem.y][newItem.x] = 'I';

    if(insert_linked_list(listaItem, (nodeData_t) newItem, -1) == 0){
        fputs("ERROR - function adicionar_item: Couldn't add item to the list\n", stderr);
        fflush(stderr);
        return 0;
    }
}

// Processa o movimento do player
// Caso o movimento tenha sido realizado retorna 0,
// Caso o movimento não tenha sido realizado porque era inválido retorna 1,
// Caso o movimento não tenha sido realizado por conta de erro em algum processamento retorna 2
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

    int index;

    // Detecção de colisão
    switch (board[nextPositionY][nextPositionX]){
    case '-':
        

        break;
    
    case 'E':
        index = search_position_linked_list(enemyList, nextPositionX, nextPositionY);
        if(index == -1){
            fputs("ERROR - function mover_personagem: Couldn't find enemy in the list\n", stderr);
            fflush(stderr);
            return 2;
        }
        if(combate(p, index) == 0){
            fputs("ERROR - function mover_personagem: Couldn't process combat\n", stderr);
            fflush(stderr);
            return 2;
        }
        break;
    
    case 'I':
        index = search_position_linked_list(itemList, nextPositionX, nextPositionY);
        if(index == -1){
            fputs("ERROR - function mover_personagem: Couldn't find item in the list\n", stderr);
            fflush(stderr);
            return 2;
        }
        if(coletar_item(p, index) == 0){
            fputs("ERROR - function mover_personagem: Couldn't process item colection\n", stderr);
            fflush(stderr);
            return 2;
        }

        /*
        index = search_position_linked_list(itemList, nextPositionX, nextPositionY);
        Item item = search_linked_list(itemList, index)->data.item;

        #ifdef DEBUG
        print_item(item);
        #endif

        p->x = nextPositionX;
        p->y = nextPositionY;

        p->pontos += item.valor;

        remove_linked_list(itemList, index);
        */
        break;
    
    case 'X':
        #ifdef DEBUG
        index = search_position_linked_list(obstacleList, nextPositionX, nextPositionY);
        Obstaculo obstaculo = search_linked_list(obstacleList, index)->data.obstaculo;
        print_obstacle(obstaculo);
        #endif

        return 1;
        break;
    
    case 'P':
        return 1;
        break;

    default:
        return 1;
        break;
    }

    // Se chegou até essa parte do código (não passou por return), o movimento é executado

    p->x = nextPositionX;
    p->y = nextPositionY;

    return 0;
}

// Processa o combate entre o player e um inimigo
// Em caso de sucesso retorna 1, em caso de fracasso retorna 0
int combate(Personagem *p, int indexInimigo){
    linkedNode_t* noInimigo = search_linked_list(enemyList, indexInimigo);
    if(noInimigo == NULL){
        fputs("ERROR - function combate: Couldn't find enemy in the list\n", stderr);
        fflush(stderr);
        return 0;
    }
    Inimigo inimigo = noInimigo->data.inimigo;

    if(remove_linked_list(enemyList, indexInimigo) == 0){
        fputs("ERROR - function combate: Couldn't remove enemy from list\n", stderr);
        fflush(stderr);
        return 0;
    }

    p->vida -= inimigo.vida;
    log_combat(log_file, inimigo);

    #ifdef DEBUG
    print_list(enemyList);
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
    log_item_collected(log_file, item);

    #ifdef DEBUG
    print_list(itemList);
    #endif

    return 1;
}

int mover_inimigos(linkedList_t *listaInimigo, char **tabuleiro){

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
// Em caso do movimento ser realizado, retorna 0
// Em caso do movimento não ser realizado por ser inválido, retorna 1
// Em caso do movimento não ser realizado por ter ocorrido erro, retorna 2
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
                log_action(log_file, m);
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

// Loga um movimento num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_action(FILE *logFile, Movimento m){
    if(logFile == NULL){
        fputs("ERROR - function log_action: Invalid file stream\n", stderr);
        fflush(stderr);
        return 0;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", local);

    int result = fprintf(logFile, "\n%s\tAcao: %c\tDirecao: %c\n", timeString, m.acao, m.direcao);

    if(result == -1){
        fputs("ERROR - function log_action: Couldn't write log\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
}

// Loga um item coletado num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_item_collected(FILE *logFile, Item i){
    if(logFile == NULL){
        fputs("ERROR - function log_item_collected: Invalid file stream\n", stderr);
        fflush(stderr);
        return 0;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", local);

    int result = fprintf(logFile, "\n%s\tItem coletado\tValor: %d\tPosicao: [%d, %d]\n", timeString, i.valor, i.x, i.y);

    if(result == -1){
        fputs("ERROR - function log_item_collected: Couldn't write log\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
}

// Loga um combate num arquivo de log especificado
// Em caso de sucesso retorna 1, em caso de falha retorna 0 
int log_combat(FILE *logFile, Inimigo i){
    if(logFile == NULL){
        fputs("ERROR - function log_combat: Invalid file stream\n", stderr);
        fflush(stderr);
        return 0;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", local);

    int result = fprintf(logFile, "\n%s\tInimigo combatido\tVida: %d\tPosicao: [%d, %d]\n", timeString, i.vida, i.x, i.y);

    if(result == -1){
        fputs("ERROR - function log_combat: Couldn't write log\n", stderr);
        fflush(stderr);
        return 0;
    }

    return 1;
}

// Libera toda memória dinamicamente alocada pelo programa
// Em caso de sucesso retorna 0, em caso de fracasso retorna 1
int liberar_memoria(char **tabuleiro, Personagem *p, linkedList_t *listaInimigo, linkedList_t *listaItem, linkedList_t *listaObstaculo, queue_t *filaMovimento, FILE *logFile){
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

    if(fclose(logFile) == EOF){
        fputs("ERROR - function liberar_memoria: Couldn't close log file\n", stderr);
        fflush(stderr);
        return 1;
    }

    return 0;
}