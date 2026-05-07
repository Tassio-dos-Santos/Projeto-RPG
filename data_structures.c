#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Jogo - Datatypes
#define JOGO
#define LINKEDLIST
#define QUEUE
#ifdef JOGO

typedef struct
{
    uint32_t x, y;
} position_t;

typedef struct {
    position_t position;
    int32_t life_points;
    int32_t mana_points;
} character_t;

typedef struct {
    position_t position;
} obstacle_t;

typedef struct enemy_t {
    position_t position;
    int32_t life_points;
} enemy_t;

typedef struct item_t {
    position_t position;
    int32_t valor;
} item_t;

typedef struct action_t {
    char acao; // 'M' para movimento, 'E' para habilidade
    char direcao; // 'W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita
} action_t;

typedef enum {
    FIREBALL,
    LIGHTNING,
    HEALING_SPELL
} skill_t;

typedef enum {
    MOVE,
    COMBAT,
    ITEM_COLLECTED,
    SKILL,
    ENEMY_DAMAGED,
    VICTORY,
    DEFEAT
} event_type_t;

typedef struct {
    event_type_t type;
    void *main_entity, *secundary_entity;
} event_t;

#endif

// Linked List - Declaração de Datatypes e Defines

#ifdef LINKEDLIST

struct linkedList_t;

typedef enum {
    #ifdef JOGO
    CHARACTER_TYPE,
    ENEMY_TYPE,
    ITEM_TYPE,
    MOVE_TYPE,
    OBSTACLE_TYPE,
    EVENT_TYPE,
    #endif
    LIST_TYPE
} list_type_t;

#endif

typedef union nodeData_t{
    #ifdef JOGO
    character_t personagem;
    enemy_t inimigo;
    item_t item;
    action_t movimento;
    obstacle_t obstaculo;
    event_t evento;
    #endif

    #ifdef LINKEDLIST
    struct linkedList_t* lista;
    #endif
} nodeData_t;

// Jogo - Métodos 

#ifdef JOGO

// ---- Declarações ----
// Funções de comparação

int compare_position(position_t p1, position_t p2);
int compare_character(nodeData_t d1, nodeData_t d2);
int compare_enemy(nodeData_t d1, nodeData_t d2);
int compare_item(nodeData_t d1, nodeData_t d2);
int compare_move(nodeData_t d1, nodeData_t d2);
int compare_obstacle(nodeData_t d1, nodeData_t d2);
int compare_event(nodeData_t d1, nodeData_t d2);

// Funções de impressão

int print_position(position_t p, FILE* file);
int print_character(character_t p, FILE* file);
int print_enemy(enemy_t i, FILE* file);
int print_item(item_t i, FILE* file);
int print_move(action_t m, FILE* file);
int print_obstacle(obstacle_t o, FILE* file);
int print_event(event_t e, FILE* file);

// ---- Métodos Básicos ----

// ---------------- Comparar posições ---------------- 
// Retorna 1 em caso dos personagens serem iguais, retorna 0 caso contrário
int compare_position(position_t p1, position_t p2){
    if(
        memcmp(&p1, &p2, sizeof(position_t)) == 0
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar personagens ---------------- 
// Retorna 1 em caso dos personagens serem iguais, retorna 0 caso contrário
int compare_character(nodeData_t d1, nodeData_t d2){
    character_t p1 = d1.personagem;
    character_t p2 = d2.personagem;
    if(
        p1.mana_points == p2.mana_points &&
        p1.life_points == p2.life_points &&
        compare_position(p1.position, p2.position) == 1
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
    enemy_t i1 = d1.inimigo;
    enemy_t i2 = d2.inimigo;
    if(
        i1.life_points == i2.life_points &&
        compare_position(i1.position, i2.position) == 1
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
    item_t i1 = d1.item;
    item_t i2 = d2.item;
    if(
        i1.valor == i2.valor &&
        compare_position(i1.position, i2.position) == 1
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
    action_t m1 = d1.movimento;
    action_t m2 = d2.movimento;
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
    obstacle_t o1 = d1.obstaculo;
    obstacle_t o2 = d2.obstaculo;
    if(
        compare_position(o1.position, o2.position) == 1
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Comparar eventos ---------------- 
// Retorna 1 em caso dos eventos serem iguais, retorna 0 caso contrário
int compare_event(nodeData_t d1, nodeData_t d2){
    event_t o1 = d1.evento;
    event_t o2 = d2.evento;
    if(
        o1.type == o2.type &&
        o1.main_entity == o2.main_entity &&
        o1.secundary_entity == o2.secundary_entity
    ){
        return 1;
    }
    else{
        return 0;
    }
}

// ---------------- Imprimir posição ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
// Obs.: Dá fflush no file passado como argumento no fim da execução
int print_position(position_t p, FILE* file){
    if(fprintf(file ,"posicao: [%d, %d]\n", p.x, p.y) == -1) return 0;

    fflush(file);

    return 1;
}

// ---------------- Imprimir personagem ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_character(character_t p, FILE* file){
    if(fprintf(file ,"vida: %d\nmana: %d\n", p.life_points, p.mana_points) == -1) return 0;

    return print_position(p.position, file);
}

// ---------------- Imprimir inimigo ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_enemy(enemy_t i, FILE* file){
    if(fprintf(file ,"vida: %d\n", i.life_points) == -1) return 0;

    return print_position(i.position, file);
}

// ---------------- Imprimir item ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_item(item_t i, FILE* file){
    if(fprintf(file ,"valor: %d\n", i.valor) == -1) return 0;

    return print_position(i.position, file);
}

// ---------------- Imprimir movimento ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_move(action_t m, FILE* file){
    if(fprintf(file ,"acao: %c\ndirecao: %c\n", m.acao, m.direcao) == -1) return 0;

    fflush(file);

    return 1;
}

// ---------------- Imprimir obstaculo ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_obstacle(obstacle_t o, FILE* file){
    return print_position(o.position, file);
}

// ---------------- Imprimir evento ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
int print_event(event_t e, FILE* file){
    switch (e.type)
    {
    case MOVE:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        fprintf(file ,"player moveu para a ");

        print_position(p.position, file);

        fprintf(file ,"\n");
        break;
    }
    case COMBAT:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        if(e.secundary_entity == NULL){
            fputs("ERROR - function print_event: Invalid secundary entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        enemy_t i = *(enemy_t *) e.secundary_entity;

        fprintf(file ,"Player combateu o inimigo na ");

        print_position(i.position, file);

        fprintf(file ,"Vida atual do Player: %d      Vida atual do inimigo: %d\n\n", p.life_points, i.life_points);
        break;
    }
    case ITEM_COLLECTED:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        if(e.secundary_entity == NULL){
            fputs("ERROR - function print_event: Invalid secundary entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        item_t i = *(item_t *) e.secundary_entity;

        fprintf(file ,"Player coletou o item na ");

        print_position(i.position, file);

        fprintf(file ,"Mana atual do Player: %d      Valor do item: %d\n\n", p.mana_points, i.valor);

        break;
    }
    case SKILL:
    {
        /*character_t *p = (character_t *) e.main_entity;

        fprintf(file ,"\n\n");*/
        break;
    }
    case ENEMY_DAMAGED:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        if(e.secundary_entity == NULL){
            fputs("ERROR - function print_event: Invalid secundary entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        enemy_t i = *(enemy_t *) e.secundary_entity;

        fprintf(file ,"O inimigo na ");

        print_position(i.position, file);

        fprintf(file, "levou dano\n");

        fprintf(file ,"Mana atual do Player: %d      Vida atual do inimigo: %d\n\n", p.mana_points, i.life_points);
        break;
    }
    case VICTORY:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        fprintf(file ,"Player venceu\nVida do personagem: %d      Mana do personagem: %d\n\n", p.life_points, p.mana_points);
        break;
    }
    case DEFEAT:
    {
        if(e.main_entity == NULL){
            fputs("ERROR - function print_event: Invalid main entity\n", stderr);
            fflush(stderr);
            return 0;
        }
        character_t p = *(character_t *) e.main_entity;
        fprintf(file ,"Player perdeu\nVida do personagem: %d      Mana do personagem: %d\n\n", p.life_points, p.mana_points);
        break;
    }
    default:
        break;
    }

    fflush(file);

    return 1;
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
    list_type_t listType; // CHARACTER_TYPE - ENEMY_TYPE - ITEM_TYPE - MOVE_TYPE - LIST_TYPE
    linkedNode_t * head, * tail;
    int length;
} linkedList_t;

// Datatype de funções de comparação entre nós
typedef int (*comparison)(union nodeData_t, union nodeData_t);

// Linked List - Métodos

// Declarações

int isDataEmpty(nodeData_t d);
int compare_list(nodeData_t d1, nodeData_t d2);
void print_list(linkedList_t* list, FILE* file);
linkedList_t* create_linked_list(int listType);
int insert_linked_list(linkedList_t* list, nodeData_t data, int index);
int remove_linked_list(linkedList_t* list, int index);
linkedNode_t* search_linked_list(linkedList_t* list, int index);
int delete_linked_list(linkedList_t* list);
int search_data_linked_list(linkedList_t* list, nodeData_t data);
int search_position_linked_list(linkedList_t* list, position_t position_searched);

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
int search_position_linked_list(linkedList_t* list, position_t searched_position){
    if(list->length < 1){
        fputs("ERROR - function search_data_linked_list: Empty list\n", stderr);
        fflush(stderr);
        return -1;
    }

    linkedNode_t* current_node = list->head;

    for(int i = 0; i < list->length; i++){
        position_t current_position;
        switch (list->listType)
        {
        case CHARACTER_TYPE:
            current_position = current_node->data.personagem.position;
            break;
        
            
        case ENEMY_TYPE:
            current_position = current_node->data.personagem.position;
            break;
        
            
        case ITEM_TYPE:
            current_position = current_node->data.personagem.position;
            break;
        
        case OBSTACLE_TYPE:
            current_position = current_node->data.personagem.position;
            break;
        
        default:
            fputs("ERROR - function search_position_linked_list: Invalid list type\n", stderr);
            fflush(stderr);
            return -1;
            break;
        }

        // Se a posição atual for igual a procurada retorna i
        if(compare_position(current_position, searched_position) == 1) return i;

        // Se não passa pro próximo nó
        current_node = current_node->next;
    }

    return -1;
}

#endif

// ------------ Impressão da Lista ------------
// Printa cada item da lista
void print_list(linkedList_t* list, FILE* file){
    linkedNode_t* currentNode = list->head;
    int mode = list->listType;
    for(int i = 0; i < list->length; i++){
        switch (mode)
        {
        #ifdef JOGO

        case CHARACTER_TYPE:
            printf("\nPersonagem  %d\n", i + 1);
            fflush(stdout);

            print_character(currentNode->data.personagem, file);
            break;
        
        case ENEMY_TYPE:
            printf("\nInimigo  %d\n", i + 1);
            fflush(stdout);

            print_enemy(currentNode->data.inimigo, file);
            break;
        
        case ITEM_TYPE:
            printf("\nItem  %d\n", i + 1);
            fflush(stdout);

            print_item(currentNode->data.item, file);
            break;
        
        case MOVE_TYPE:
            printf("\nMovimento  %d\n", i + 1);
            fflush(stdout);

            print_move(currentNode->data.movimento, file);
            break;
            
        case OBSTACLE_TYPE:
            printf("\nObstaculo  %d\n", i + 1);
            fflush(stdout);

            print_obstacle(currentNode->data.obstaculo, file);
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
void print_queue(queue_t* queue, FILE* file);
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
void print_queue(queue_t* queue, FILE* file){
    print_list((linkedList_t*) queue, file);
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

#endif