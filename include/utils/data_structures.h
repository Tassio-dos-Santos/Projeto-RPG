#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/status.h"

#define LOG
#ifdef LOG

#define LOG_LENGTH 512

typedef struct {
    char text[LOG_LENGTH];
} log_t;

#endif

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

struct linked_list_t;

typedef enum {
    #ifdef JOGO
    CHARACTER_TYPE,
    ENEMY_TYPE,
    ITEM_TYPE,
    MOVE_TYPE,
    OBSTACLE_TYPE,
    LOG_TYPE,
    #endif
    LIST_TYPE
} list_type_t;

#endif

typedef union node_data_t{
    #ifdef JOGO
    character_t personagem;
    enemy_t inimigo;
    item_t item;
    action_t movimento;
    obstacle_t obstaculo;
    #endif

    #ifdef LOG
    log_t log;
    #endif

    #ifdef LINKEDLIST
    struct linked_list_t* lista;
    #endif
} node_data_t;

// Empty data
static node_data_t empty_data = {0};

// Jogo - Declaração de Métodos 

#ifdef JOGO

// Funções de comparação

bool compare_position(position_t p1, position_t p2);
bool compare_character(node_data_t d1, node_data_t d2);
bool compare_enemy(node_data_t d1, node_data_t d2);
bool compare_item(node_data_t d1, node_data_t d2);
bool compare_move(node_data_t d1, node_data_t d2);
bool compare_obstacle(node_data_t d1, node_data_t d2);
bool compare_log(node_data_t d1, node_data_t d2);

// Funções de impressão

status_t print_position(position_t p, FILE* file);
status_t print_character(character_t p, FILE* file);
status_t print_enemy(enemy_t i, FILE* file);
status_t print_item(item_t i, FILE* file);
status_t print_move(action_t m, FILE* file);
status_t print_obstacle(obstacle_t o, FILE* file);
status_t print_log(log_t log, FILE* file);

#endif

// Linked List - Definição de Datatypes e Declaração de Métodos

#ifdef LINKEDLIST

// Linked List - Definição de Datatypes

typedef struct linked_node_t {
    node_data_t data;
    struct linked_node_t * next;
} linked_node_t;

typedef struct linked_list_t {
    list_type_t listType; // CHARACTER_TYPE - ENEMY_TYPE - ITEM_TYPE - MOVE_TYPE - LIST_TYPE
    linked_node_t * head, * tail;
    uint32_t length;
} linked_list_t;

// Datatype de funções de comparação entre nós
typedef bool (*comparison)(union node_data_t, union node_data_t);

// Linked List - Declaração de Métodos

bool is_data_empty(node_data_t d);
bool compare_list(node_data_t d1, node_data_t d2);
status_t print_list(linked_list_t* list, FILE* file);
linked_list_t* create_linked_list(list_type_t listType);
status_t insert_linked_list(linked_list_t* list, node_data_t data, int index);
status_t remove_linked_list(linked_list_t* list, int index);
linked_node_t* search_linked_list(linked_list_t* list, int index);
status_t delete_linked_list(linked_list_t* list);
int search_data_linked_list(linked_list_t* list, node_data_t data);
int search_position_linked_list(linked_list_t* list, position_t position_searched);

#endif

// Double Linked List - Definição de Datatypes

#ifdef DOUBLELINKEDLIST

// Double Linked List - Datatypes

typedef struct double_linked_node_t {
    node_data_t data;
    struct double_linked_node_t * next, * previous;
} double_linked_node_t;

typedef struct doubleLinkedList {
    list_type_t listType; // CHARACTER_TYPE - ENEMY_TYPE - ITEM_TYPE - MOVE_TYPE - LIST_TYPE
    linked_node_t * head, * tail;
    uint32_t length;
} doubleLinkedList_t;



#endif

// Queue - Definição de Datatypes e Declaração de Métodos

#ifdef QUEUE

// Queue - Datatypes

typedef linked_list_t queue_t;

// Queue - Declaração de Métodos

queue_t* create_queue(int listType);
node_data_t front(queue_t* queue);
status_t enqueue(queue_t* queue, node_data_t data);
node_data_t dequeue(queue_t* queue);
status_t print_queue(queue_t* queue, FILE* file);
status_t delete_queue(queue_t* queue);

#endif

// Stack - Definição de Datatypes e Declaração de Métodos

#ifdef STACK

// Stack - Datatypes

typedef linked_list_t stack_t;

// Stack - Métodos

// ---- Declarações ----
stack_t* create_stack(int listType);
node_data_t top(stack_t* stack);
status_t push(stack_t* stack, node_data_t data);
node_data_t pop(stack_t* stack);
status_t print_stack(stack_t* stack);
status_t delete_stack(stack_t* stack);

#endif

// Deque - Definição de Datatypes e Declaração de Métodos

#ifdef DEQUE

#endif

#endif