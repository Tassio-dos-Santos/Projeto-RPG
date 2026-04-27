#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

// Jogo - Datatypes

#ifdef JOGO

typedef struct {
    int x, y;
    int vida;
    int pontos;
} Personagem;

typedef struct {
    int x, y;
} Obstaculo;

typedef struct Inimigo {
    int x, y;
    int vida;
} Inimigo;

typedef struct Item {
    int x, y;
    int valor;
} Item;

typedef struct Movimento {
    char acao; // 'M' para movimento, 'A' para ataque
    char direcao; // 'W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita
} Movimento;

#endif

// Linked List - Declaração de Datatypes e Defines

#ifdef LINKEDLIST

struct linkedList_t;

#ifdef JOGO
#define CHARACTER_TYPE 0
#define ENEMY_TYPE 1
#define ITEM_TYPE 2
#define MOVE_TYPE 3
#define OBSTACLE_TYPE 4
#endif

#define LIST_TYPE 5

#endif

typedef union nodeData_t{
    #ifdef JOGO
    Personagem personagem;
    Inimigo inimigo;
    Item item;
    Movimento movimento;
    Obstaculo obstaculo;
    #endif

    #ifdef LINKEDLIST
    struct linkedList_t* lista;
    #endif
} nodeData_t;

// Jogo - Métodos 

#ifdef JOGO

// ---- Declarações ----
int compare_character(nodeData_t d1, nodeData_t d2);
int compare_enemy(nodeData_t d1, nodeData_t d2);
int compare_item(nodeData_t d1, nodeData_t d2);
int compare_move(nodeData_t d1, nodeData_t d2);
int compare_obstacle(nodeData_t d1, nodeData_t d2);
int compare_list(nodeData_t d1, nodeData_t d2);
void print_character(Personagem p);
void print_enemy(Inimigo i);
void print_item(Item i);
void print_move(Movimento m);
void print_obstacle(Obstaculo o);

// ---- Métodos Básicos ----

// ---------------- Comparar personagens ---------------- 
// Retorna 1 em caso dos personagens serem iguais, retorna 0 caso contrário
int compare_character(nodeData_t d1, nodeData_t d2){
    Personagem p1 = d1.personagem;
    Personagem p2 = d2.personagem;
    if(
        p1.pontos == p2.pontos &&
        p1.vida == p2.vida &&
        p1.x == p2.x &&
        p1.y == p2.y
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar inimigos ---------------- 
// Retorna 1 em caso dos inimigos serem iguais, retorna 0 caso contrário
int compare_enemy(nodeData_t d1, nodeData_t d2){
    Inimigo i1 = d1.inimigo;
    Inimigo i2 = d2.inimigo;
    if(
        i1.vida == i2.vida &&
        i1.x == i2.x &&
        i1.y == i2.y
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar itens ---------------- 
// Retorna 1 em caso dos itens serem iguais, retorna 0 caso contrário
int compare_item(nodeData_t d1, nodeData_t d2){
    Item i1 = d1.item;
    Item i2 = d2.item;
    if(
        i1.valor == i2.valor &&
        i1.x == i2.x &&
        i1.y == i2.y
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar movimento ---------------- 
// Retorna 1 em caso dos movimentos serem iguais, retorna 0 caso contrário
int compare_move(nodeData_t d1, nodeData_t d2){
    Movimento m1 = d1.movimento;
    Movimento m2 = d2.movimento;
    if(
        m1.acao == m2.acao &&
        m1.direcao == m2.direcao
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar obstaculos ---------------- 
// Retorna 1 em caso dos obstaculos serem iguais, retorna 0 caso contrário
int compare_obstacle(nodeData_t d1, nodeData_t d2){
    Obstaculo o1 = d1.obstaculo;
    Obstaculo o2 = d2.obstaculo;
    if(
        o1.x == o2.x &&
        o1.y == o2.y
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Imprimir personagem ---------------- 
void print_character(Personagem p){
    printf("Vida: %d\n", p.vida);
    fflush(stdout);

    printf("Pontos: %d\n", p.pontos);
    fflush(stdout);

    printf("Posicao: [%d, %d]\n", p.x, p.y);
    fflush(stdout);
}

// ---------------- Imprimir inimigo ---------------- 
void print_enemy(Inimigo i){
    printf("Vida: %d\n", i.vida);
    fflush(stdout);

    printf("Posicao: [%d, %d]\n", i.x, i.y);
    fflush(stdout);
}

// ---------------- Imprimir item ---------------- 
void print_item(Item i){
    printf("Valor: %d\n", i.valor);
    fflush(stdout);

    printf("Posicao: [%d, %d]\n", i.x, i.y);
    fflush(stdout);
}

// ---------------- Imprimir movimento ---------------- 
void print_move(Movimento m){
    printf("Acao: %c\n", m.acao);
    fflush(stdout);

    printf("Direcao: %c\n", m.direcao);
    fflush(stdout);
}

// ---------------- Imprimir obstaculo ---------------- 
void print_obstacle(Obstaculo o){
    printf("Posicao: [%d, %d]\n", o.x, o.y);
    fflush(stdout);
}

#endif

// Linked List - Definição de Datatypes e Métodos

#ifdef LINKEDLIST

// Linked List - Definição de Datatypes

typedef struct linkedNode_t {
    nodeData_t data;
    struct linkedNode_t * next;
} linkedNode_t;

typedef struct linkedList_t {
    int listType; // CHARACTER_TYPE - ENEMY_TYPE - ITEM_TYPE - MOVE_TYPE - LIST_TYPE
    linkedNode_t * head, * tail;
    int length;
} linkedList_t;

// Datatype de funções de comparação entre nós
typedef int (*comparison)(union nodeData_t, union nodeData_t);

// Linked List - Métodos

// Declarações

int isDataEmpty(nodeData_t d);
linkedList_t* create_linked_list(int listType);
int insert_linked_list(linkedList_t* list, nodeData_t data, int index);
int remove_linked_list(linkedList_t* list, int index);
linkedNode_t* search_linked_list(linkedList_t* list, int index);
int delete_linked_list(linkedList_t* list);
int search_data_linked_list(linkedList_t* list, nodeData_t data);
int search_position_linked_list(linkedList_t* list, int x, int y);
void print_list(linkedList_t* list);

// ---- Métodos Básicos ----

// ---------------- Comparar listas ---------------- 
// Retorna 1 em caso do dado estar vazio, retorna 0 caso contrário
int isDataEmpty(nodeData_t d){
    return (d.lista == NULL);
}

// ---------------- Comparar listas ---------------- 
// Retorna 1 em caso das listas serem iguais, retorna 0 caso contrário
int compare_list(nodeData_t d1, nodeData_t d2){
    linkedList_t* l1 = d1.lista;
    linkedList_t* l2 = d2.lista;
    return (l1 == l2);
}

// ---------------- Criar lista ---------------- 
// Retorna o endereço da lista em caso de sucesso, retorna NULL em caso de fracasso
linkedList_t* create_linked_list(int listType){
    if(!(listType >= CHARACTER_TYPE && listType <= LIST_TYPE)){
        fputs("ERROR - function create_linked_list: Invalid list type\n", stderr);
        fflush(stderr);
        return NULL;
    }

    // Aloca o espaço na memória para uma nova lista
    linkedList_t* newList = (linkedList_t*) malloc(sizeof(linkedList_t));

    // Verifica se pôde alocar a memória da lista
    if(newList == NULL){
        fputs("ERROR - function create_linked_list: Couldn't allocate memory for a new list\n", stderr);
        fflush(stderr);
        return NULL;
    }

    // A nova lista é iniciada com tamanho 0
    newList->length = 0;

    // A nova lista é iniciada com a cabeça e a cauda apontada para NULL
    newList->head = NULL;
    newList->tail = NULL;

    // Define o tipo da lista
    newList->listType = listType;

    // Retorna a nova lista
    return newList;
}

// ------------ Inserção por índice ------------
// Insere um novo nó depois do nó com o indíce passado como argumento
// Insere na cabeça caso passe indíce negativo
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int insert_linked_list(linkedList_t* list, nodeData_t data, int index){
    // Se o index for igual ou maior que 0
    if(index >= 0){
        if(index >= list->length){
            fputs("ERROR - function insert_linked_list: Index out of range\n", stderr);
            fflush(stderr);
            return 0;
        }

        // O local onde o nó vai ser posto é encontrado
        linkedNode_t* currentNode = search_linked_list(list, index);

        // Verifica se encontrou o local na lista
        if(currentNode == NULL){
            fputs("ERROR - function insert_linked_list: Couldn't find target node\n", stderr);
            fflush(stderr);
            return 0;
        }

        // O espaço para o novo nó é alocado
        linkedNode_t* newNode = (linkedNode_t*) malloc(sizeof(linkedNode_t));

        // Verifica se memória foi alocada ou não para o novo nó
        if(newNode == NULL){
            fputs("ERROR - function insert_linked_list: Couldn't allocate memory for a new node\n", stderr);
            fflush(stderr);
            return 0;
        }

        // Os dados são postos no novo nó
        newNode->data = data;

        // O novo nó aponta onde seu antecessor apontava
        newNode->next = currentNode->next;

        // O antecessor aponta para o novo nó
        currentNode->next = newNode;

        // Se a cauda era o antecessor, o novo nó se torna a cauda
        if(list->tail == currentNode){
            list->tail = newNode;
        }
    }

    // Se o index for menor que 0
    else{
        // O espaço para o novo nó é alocado
        linkedNode_t* newNode = (linkedNode_t*) malloc(sizeof(linkedNode_t));

        // Verifica se memória foi alocada ou não para o novo nó
        if(newNode == NULL){
            fputs("ERROR - function insert_linked_list: Couldn't allocate memory for a new node\n", stderr);
            fflush(stderr);
            return 0;
        }

        // Os dados são postos no novo nó
        newNode->data = data;

        // O novo nó aponta para onde a cabeça apontava
        newNode->next = list->head;

        // O novo nó se torna a cabeça
        list->head = newNode;

        // Se a cauda apontar para NULL, ou seja lista vazia, cauda aponta pro novo nó
        if(list->tail == NULL){
            list->tail = newNode;
        }
    }

    // O tamanho da lista aumenta em um
    list->length++;

    return 1;
}

// ------------ Remoção por índice -------------
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int remove_linked_list(linkedList_t* list, int index){
    if(index < 0 || index >= list->length){
        fputs("ERROR - function remove_linked_list: Index out of range\n", stderr);
        fflush(stderr);
        return 0;
    }

    // Caso não seja o primeiro item da lista
    if(index > 0){
        // Pega o nó alvo e o nó anterior a ele
        linkedNode_t* previousNode = search_linked_list(list, index - 1);
        if(previousNode == NULL){
            fputs("ERROR - function remove_linked_list: Couldn't find target node\n", stderr);
            fflush(stderr);
            return 0;
        }
        linkedNode_t* targetNode = previousNode->next;

        if(list->listType == LIST_TYPE){
            if(delete_linked_list(targetNode->data.lista) == 0){
                fputs("ERROR - function remove_linked_list: Couldn't delete target node's list\n", stderr);
                fflush(stderr);
                return 0;
            }
        }

        // Faz o nó anterior apontar para o nó posterior ao alvo
        previousNode->next = targetNode->next;

        // Caso o nó removido for a cauda, o nó anterior se torna a cauda
        if(list->tail == targetNode){
            list->tail = previousNode;
        }

        // Libera o nó
        free(targetNode);
    }
    
    // Caso seja o primeiro item da lista
    else if(index == 0){
        // Pega o alvo que está na cabeça da lista
        linkedNode_t* targetNode = list->head;

        // Muda a cabeça para o segundo nó
        list->head = targetNode->next;

        // Caso o nó removido também for a cauda, a cauda aponta pra NULL
        if(list->tail == targetNode){
            list->tail = NULL;
        }

        // Libera o nó da memória
        free(targetNode);
    }

    list->length--;
    return 1;
}

// ------------ Pesquisa por índice ------------
// Retorna o endereço do nó procurado em caso de sucesso, retorna NULL em caso de fracasso
linkedNode_t* search_linked_list(linkedList_t* list, int index){
    if(index < 0 || index >= list->length){
        fputs("ERROR - function search_linked_list: Index out of range\n", stderr);
        fflush(stderr);
        printf("Index: %d\nList Length: %d\n", index, list->length);
        return NULL;
    }

    linkedNode_t* currentNode = list->head;
    for(int i = 0; i < index; i++){
        currentNode = currentNode->next;
    }

    return currentNode;
}

// --------------- Deletar lista --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int delete_linked_list(linkedList_t* list){
    // Se a lista não está vazia, libera cada nó da lista primeiro
    if(list->length != 0){
        // A variável targetNode é usada para apontar para os nós que são liberados
        // e currentNode é usada para percorrer a lista
        linkedNode_t* targetNode, * currentNode = list->head;
        for(int i = 0; i < list->length; i++){
            // O nó a ser liberado é o nó atual
            targetNode = currentNode;

            // currentNode avança
            currentNode = currentNode->next;

            if(list->listType == LIST_TYPE){
                if(delete_linked_list(targetNode->data.lista) == 0){
                    fputs("ERROR - function delete_linked_list: Couldn't delete target node's list\n", stderr);
                    fflush(stderr);
                    return 0;
                }
            }

            // O nó é liberado
            free(targetNode);
        }
    }

    free(list);
    return 1;
}

// ------------ Pesquisa por Dado ------------
// Retorna o indíce do nó em caso de sucesso, retorna -1 em caso de fracasso
int search_data_linked_list(linkedList_t* list, nodeData_t data){
    if(list->length < 1){
        fputs("ERROR - function search_data_linked_list: Empty list\n", stderr);
        fflush(stderr);
        return -1;
    }

    linkedNode_t* currentNode = list->head;

    comparison compare;

    switch (list->listType)
    {
    #ifdef JOGO
    case CHARACTER_TYPE:
        compare = compare_character;
        break;
    
        
    case ENEMY_TYPE:
        compare = compare_enemy;
        break;
    
        
    case ITEM_TYPE:
        compare = compare_item;
        break;
    
        
    case MOVE_TYPE:
        compare = compare_move;
        break;
    
    case OBSTACLE_TYPE:
        compare = compare_obstacle;
        break;

    #endif
        
    case LIST_TYPE:
        compare = compare_list;
        break;
    
    default:
        fputs("ERROR - function search_data_linked_list: Invalid list type\n", stderr);
        fflush(stderr);
        return -1;
        break;
    }

    for(int i = 0; i < list->length; i++){
        // Se o personagem do nó for igual ao do argumento, retorna o nó atual
        if(compare(currentNode->data, data)){
            return i;
        }

        // Se não passa pro próximo nó
        currentNode = currentNode->next;
    }

    return -1;
}

#ifdef JOGO
// ------------ Pesquisa por Posição no Tabuleiro ------------
// Retorna o indíce do nó em caso de sucesso, retorna -1 em caso de fracasso
int search_position_linked_list(linkedList_t* list, int x, int y){
    if(list->length < 1){
        fputs("ERROR - function search_data_linked_list: Empty list\n", stderr);
        fflush(stderr);
        return -1;
    }

    linkedNode_t* currentNode = list->head;

    for(int i = 0; i < list->length; i++){
        // Se o personagem do nó for igual ao do argumento, retorna o nó atual
        switch (list->listType)
        {
        case CHARACTER_TYPE:
            if(x == currentNode->data.personagem.x && y == currentNode->data.personagem.y){
                return i;
            }
            break;
        
            
        case ENEMY_TYPE:
            if(x == currentNode->data.inimigo.x && y == currentNode->data.inimigo.y){
                return i;
            }
            break;
        
            
        case ITEM_TYPE:
            if(x == currentNode->data.item.x && y == currentNode->data.item.y){
                return i;
            }
            break;
        
        case OBSTACLE_TYPE:
            if(x == currentNode->data.obstaculo.x && y == currentNode->data.obstaculo.y){
                return i;
            }
            break;
        
        default:
            fputs("ERROR - function search_position_linked_list: Invalid list type\n", stderr);
            fflush(stderr);
            return -1;
            break;
        }

        // Se não passa pro próximo nó
        currentNode = currentNode->next;
    }

    return -1;
}

#endif

// ------------ Impressão da Lista ------------
// Printa cada item da lista
void print_list(linkedList_t* list){
    linkedNode_t* currentNode = list->head;
    int mode = list->listType;
    for(int i = 0; i < list->length; i++){
        switch (mode)
        {
        #ifdef JOGO

        case CHARACTER_TYPE:
            printf("\nPersonagem  %d\n", i + 1);
            fflush(stdout);

            print_character(currentNode->data.personagem);
            break;
        
        case ENEMY_TYPE:
            printf("\nInimigo  %d\n", i + 1);
            fflush(stdout);

            print_enemy(currentNode->data.inimigo);
            break;
        
        case ITEM_TYPE:
            printf("\nItem  %d\n", i + 1);
            fflush(stdout);

            print_item(currentNode->data.item);
            break;
        
        case MOVE_TYPE:
            printf("\nMovimento  %d\n", i + 1);
            fflush(stdout);

            print_move(currentNode->data.movimento);
            break;
            
        case OBSTACLE_TYPE:
            printf("\nObstaculo  %d\n", i + 1);
            fflush(stdout);

            print_obstacle(currentNode->data.obstaculo);
            break;

        #endif
        
        case LIST_TYPE:
            printf("\nLista  %d\n", i + 1);
            fflush(stdout);

            linkedList_t* currentList = currentNode->data.lista;

            printf("Lista de ");
            fflush(stdout);

            switch (currentList->listType)
            {
            case CHARACTER_TYPE:
                printf("personagem\n");
                break;
            
            case ENEMY_TYPE:
                printf("inimigo\n");
                break;
            
            case ITEM_TYPE:
                printf("item\n");
                break;
            
            case MOVE_TYPE:
                printf("movimento\n");
                break;
            
            case LIST_TYPE:
                printf("lista\n");
                break;
            
            default:
                break;
            }

            break;
        
        default:
            fputs("ERROR - function print_list: Invalid mode\n", stderr);
            fflush(stderr);
            return;
            break;
        }

        currentNode = currentNode->next;
    }
}

#endif

// Double Linked List - Definição de Datatypes e Métodos

#ifdef DOUBLELINKEDLIST

// Double Linked List - Datatypes

typedef struct doubleLinkedNode {
    nodeData_t data;
    struct doubleLinkedNode * next, * previous;
} doubleLinkedNode_t;

typedef struct doubleLinkedList {
    int listType; // CHARACTER_TYPE - ENEMY_TYPE - ITEM_TYPE - MOVE_TYPE - LIST_TYPE
    linkedNode_t * head, * tail;
    int length;
} doubleLinkedList_t;



#endif

// Queue - Definição de Datatypes e Métodos

#ifdef QUEUE

// Queue - Datatypes

typedef linkedList_t queue_t;

// Queue - Métodos

// ---- Declarações ----
queue_t* create_queue(int listType);
nodeData_t front(queue_t* queue);
int enqueue(queue_t* queue, nodeData_t data);
nodeData_t dequeue(queue_t* queue);
void print_queue(queue_t* queue);
int delete_queue(queue_t* queue);

// ---- Métodos Básicos ----

// ------------ Criar fila ------------
// Retorna a fila criada em caso de sucesso, retorna NULL em caso de fracasso
queue_t* create_queue(int listType){
    queue_t* newQueue = (queue_t*) create_linked_list(listType);

    // Retorna a nova fila
    return newQueue;
}

// ------------ Front ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
nodeData_t front(queue_t* queue){
    if (queue->length < 1)
    {
        fputs("ERROR - function front: Empty queue\n", stderr);
        fflush(stderr);
        nodeData_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else if (queue->head == NULL)
    {
        fputs("ERROR - function front: Queue head points to NULL\n", stderr);
        fflush(stderr);
        nodeData_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else{
        return queue->head->data;
    }
}

// ------------ Enqueue ------------
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int enqueue(queue_t* queue, nodeData_t data){
    return insert_linked_list((linkedList_t*) queue, data, queue->length - 1);
}

// ------------ Dequeue ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
nodeData_t dequeue(queue_t* queue){
    nodeData_t result = front(queue);

    if(remove_linked_list((linkedList_t *) queue, 0) == 0){
        fputs("ERROR - function dequeue: Couldn't remove node from queue\n", stderr);
        fflush(stderr);
    }
    
    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
void print_queue(queue_t* queue){
    print_list((linkedList_t*) queue);
}

// --------------- Deletar fila --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int delete_queue(queue_t* queue){
    return delete_linked_list((linkedList_t*) queue);
}

#endif

// Stack - Definição de Datatypes e Métodos

#ifdef STACK

// Stack - Datatypes

typedef linkedList_t stack_t;

// Stack - Métodos

// ---- Declarações ----
stack_t* create_stack(int listType);
nodeData_t top(stack_t* stack);
int push(stack_t* stack, nodeData_t data);
nodeData_t pop(stack_t* stack);
void print_stack(stack_t* stack);
int delete_stack(stack_t* stack);

// ---- Métodos Básicos ----

// ------------ Criar pilha ------------
// Retorna a pilha criada em caso de sucesso, retorna NULL em caso de fracasso
stack_t* create_stack(int listType){
    stack_t* newStack = (stack_t*) create_linked_list(listType);

    // Retorna a nova pilha
    return newStack;
}

// ------------ Front ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
nodeData_t top(stack_t* stack){
    if (stack->length < 1)
    {
        fputs("ERROR - function top: Empty stack\n", stderr);
        fflush(stderr);
        nodeData_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else if (stack->head == NULL)
    {
        fputs("ERROR - function top: Stack head points to NULL\n", stderr);
        fflush(stderr);
        nodeData_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else{
        return stack->head->data;
    }
}

// ------------ Enqueue ------------
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int push(stack_t* stack, nodeData_t data){
    return insert_linked_list((linkedList_t*) stack, data, -1);
}

// ------------ Dequeue ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
nodeData_t pop(stack_t* stack){
    nodeData_t result = top(stack);

    if(remove_linked_list((linkedList_t *) stack, 0) == 0){
        fputs("ERROR - function pop: Couldn't remove node from stack\n", stderr);
        fflush(stderr);
    }

    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
void print_stack(stack_t* stack){
    print_list((linkedList_t*) stack);
}

// --------------- Deletar fila --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int delete_stack(stack_t* stack){
    return delete_linked_list((linkedList_t*) stack);
}

#endif

// Deque - Definição de Datatypes e Métodos

#ifdef DEQUE

#endif