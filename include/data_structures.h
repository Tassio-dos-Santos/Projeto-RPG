#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JOGO

#ifdef JOGO
#define LINKEDLIST
#define QUEUE
#endif

#ifdef QUEUE
#define LINKEDLIST
#endif

#ifdef STACK
#define LINKEDLIST
#endif

#ifdef DEQUE
#define LINKEDLIST
#endif

// Jogo - Datatypes
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

typedef enum {
    CHARACTER,
    ENEMY,
    OBSTACLE,
    ITEM
} entity_type_t;

typedef union {
    character_t character;
    enemy_t enemy;
    obstacle_t obstacle;
    item_t item;
} entity_t;

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
    DAMAGE,
    VICTORY,
    DEFEAT
} event_type_t;

typedef union {
    int32_t life_points;
    int32_t mana_points;
    skill_t skill;
} auxiliar_event_data_t;


typedef struct {
    event_type_t event_type;
    entity_t main_entity, secundary_entity;
    entity_type_t main_entity_type, secundary_entity_type;
    auxiliar_event_data_t auxiliar_data;
} event_t;

#endif

// Linked List - Declaração de Datatype

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

// Jogo - Declaração de Métodos 

#ifdef JOGO

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

#endif

// Linked List - Definição de Datatypes e Declaração de Métodos

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

// Linked List - Declaração de Métodos

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

#endif

// Double Linked List - Definição de Datatypes

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

// Queue - Definição de Datatypes e Declaração de Métodos

#ifdef QUEUE

// Queue - Datatypes

typedef linkedList_t queue_t;

// Queue - Declaração de Métodos

queue_t* create_queue(int listType);
nodeData_t front(queue_t* queue);
int enqueue(queue_t* queue, nodeData_t data);
nodeData_t dequeue(queue_t* queue);
void print_queue(queue_t* queue, FILE* file);
int delete_queue(queue_t* queue);

#endif

// Stack - Definição de Datatypes e Declaração de Métodos

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

#endif

// Deque - Definição de Datatypes e Declaração de Métodos

#ifdef DEQUE

#endif

#endif