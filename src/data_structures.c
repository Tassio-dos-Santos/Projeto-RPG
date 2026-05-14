#include "utils\data_structures.h"
#include "logger.h"

// Jogo - Métodos 

#ifdef JOGO

// ---- Métodos Básicos ----

// ---------------- Comparar posições ---------------- 
// Retorna true em caso dos personagens serem iguais, retorna false caso contrário
bool compare_position(position_t p1, position_t p2){
    if(
        memcmp(&p1, &p2, sizeof(position_t)) == 0
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar personagens ---------------- 
// Retorna true em caso dos personagens serem iguais, retorna false caso contrário
bool compare_character(node_data_t d1, node_data_t d2){
    character_t p1 = d1.personagem;
    character_t p2 = d2.personagem;
    if(
        p1.mana_points == p2.mana_points &&
        p1.life_points == p2.life_points &&
        compare_position(p1.position, p2.position) == 1
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar inimigos ---------------- 
// Retorna true em caso dos inimigos serem iguais, retorna false caso contrário
bool compare_enemy(node_data_t d1, node_data_t d2){
    enemy_t i1 = d1.inimigo;
    enemy_t i2 = d2.inimigo;
    if(
        i1.life_points == i2.life_points &&
        compare_position(i1.position, i2.position) == 1
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar itens ---------------- 
// Retorna true em caso dos itens serem iguais, retorna false caso contrário
bool compare_item(node_data_t d1, node_data_t d2){
    item_t i1 = d1.item;
    item_t i2 = d2.item;
    if(
        i1.valor == i2.valor &&
        compare_position(i1.position, i2.position) == 1
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar movimento ---------------- 
// Retorna true em caso dos movimentos serem iguais, retorna false caso contrário
bool compare_move(node_data_t d1, node_data_t d2){
    action_t m1 = d1.movimento;
    action_t m2 = d2.movimento;
    if(
        m1.acao == m2.acao &&
        m1.direcao == m2.direcao
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar obstaculos ---------------- 
// Retorna true em caso dos obstaculos serem iguais, retorna false caso contrário
bool compare_obstacle(node_data_t d1, node_data_t d2){
    obstacle_t o1 = d1.obstaculo;
    obstacle_t o2 = d2.obstaculo;
    if(
        compare_position(o1.position, o2.position) == 1
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Comparar eventos ---------------- 
// Retorna true em caso dos eventos serem iguais, retorna false caso contrário
bool compare_log(node_data_t d1, node_data_t d2){
    log_t o1 = d1.log;
    log_t o2 = d2.log;
    if(
        strncmp(o1.text, o2.text, LOG_LENGTH) == 0
    ){
        return true;
    }
    else{
        return false;
    }
}

// ---------------- Imprimir posição ---------------- 
// Obs.: Dá fflush no file passado como argumento no fim da execução
status_t print_position(position_t p, FILE* file){
    if(fprintf(file ,"posicao: [%d, %d]\n", p.x, p.y) == -1) return ERR_INTERNAL;

    fflush(file);

    return SUCCESS;
}

// ---------------- Imprimir personagem ---------------- 
status_t print_character(character_t p, FILE* file){
    if(fprintf(file ,"vida: %d\nmana: %d\n", p.life_points, p.mana_points) == -1) return ERR_INTERNAL;

    return print_position(p.position, file);
}

// ---------------- Imprimir inimigo ---------------- 
status_t print_enemy(enemy_t i, FILE* file){
    if(fprintf(file ,"vida: %d\n", i.life_points) == -1) return ERR_INTERNAL;

    return print_position(i.position, file);
}

// ---------------- Imprimir item ---------------- 
status_t print_item(item_t i, FILE* file){
    if(fprintf(file ,"valor: %d\n", i.valor) == -1) return ERR_INTERNAL;

    return print_position(i.position, file);
}

// ---------------- Imprimir movimento ---------------- 
status_t print_move(action_t m, FILE* file){
    if(fprintf(file ,"acao: %c\ndirecao: %c\n", m.acao, m.direcao) == -1) return ERR_INTERNAL;

    fflush(file);

    return 1;
}

// ---------------- Imprimir obstaculo ---------------- 
status_t print_obstacle(obstacle_t o, FILE* file){
    return print_position(o.position, file);
}

// ---------------- Imprimir evento ---------------- 
status_t print_log(log_t log, FILE* file){
    if((fprintf(file, "%s", (char *) log.text)) == -1) return ERR_INTERNAL;

    fflush(file);

    return SUCCESS;
}

#endif

// Linked List - Métodos

#ifdef LINKEDLIST

// ---- Métodos Básicos ----

// ---------------- Comparar listas ---------------- 
// Retorna true em caso do dado estar vazio, retorna false caso contrário
bool is_data_empty(node_data_t d){
    return (memcmp(&empty_data, &d, sizeof(node_data_t)) == 0);
}

// ---------------- Comparar listas ---------------- 
// Retorna true em caso das listas forem as mesmas, retorna false caso contrário
bool compare_list(node_data_t d1, node_data_t d2){
    linked_list_t* l1 = d1.lista;
    linked_list_t* l2 = d2.lista;
    return (l1 == l2);
}

// ---------------- Criar lista ---------------- 
// Retorna o endereço da lista em caso de sucesso, retorna NULL em caso de fracasso
linked_list_t* create_linked_list(list_type_t listType){
    if(!(listType >= CHARACTER_TYPE && listType <= LIST_TYPE)){
        LOG_ERROR("Invalid list type");
        return NULL;
    }

    // Aloca o espaço na memória para uma nova lista
    linked_list_t* newList = (linked_list_t*) malloc(sizeof(linked_list_t));

    // Verifica se pôde alocar a memória da lista
    if(newList == NULL){
        LOG_ERROR("Couldn't allocate memory for a new list");
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
status_t insert_linked_list(linked_list_t* list, node_data_t data, int index){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    // Se o index for igual ou maior que 0
    if(index >= 0){
        if(index >= list->length){
            LOG_ERROR("Index out of range");
            return ERR_INVALID_IN;
        }

        // O local onde o nó vai ser posto é encontrado
        linked_node_t* currentNode = search_linked_list(list, index);

        // Verifica se encontrou o local na lista
        if(currentNode == NULL){
            LOG_ERROR("Couldn't find target node");
            return ERR_DATA;
        }

        // O espaço para o novo nó é alocado
        linked_node_t* newNode = (linked_node_t*) malloc(sizeof(linked_node_t));

        // Verifica se memória foi alocada ou não para o novo nó
        if(newNode == NULL){
            LOG_ERROR("Couldn't allocate memory for a new node");
            return ERR_MEMORY;
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
        linked_node_t* newNode = (linked_node_t*) malloc(sizeof(linked_node_t));

        // Verifica se memória foi alocada ou não para o novo nó
        if(newNode == NULL){
            LOG_ERROR("Couldn't allocate memory for a new node");
            return ERR_MEMORY;
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

    return SUCCESS;
}

// ------------ Remoção por índice -------------
status_t remove_linked_list(linked_list_t* list, int index){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    if(index < 0 || index >= list->length){
        LOG_ERROR("Index out of range");
        return ERR_INVALID_IN;
    }

    // Caso não seja o primeiro item da lista
    if(index > 0){
        // Pega o nó alvo e o nó anterior a ele
        linked_node_t* previousNode = search_linked_list(list, index - 1);
        if(previousNode == NULL){
            LOG_ERROR("Couldn't find target node");
            return ERR_DATA;
        }
        linked_node_t* targetNode = previousNode->next;

        if(list->listType == LIST_TYPE){
            if(IS_ERROR_STATUS(delete_linked_list(targetNode->data.lista))){
                LOG_ERROR("Couldn't delete target node's list");
                return ERR_DATA;
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
        linked_node_t* target_node = list->head;

        if(target_node == NULL){
            LOG_ERROR("List head points to NULL");
            return ERR_DATA;
        }

        // Muda a cabeça para o segundo nó
        list->head = target_node->next;

        // Caso o nó removido também for a cauda, a cauda aponta pra NULL
        if(list->tail == target_node){
            list->tail = NULL;
        }

        // Libera o nó da memória
        free(target_node);
    }

    list->length--;
    return 1;
}

// ------------ Pesquisa por índice ------------
// Retorna o endereço do nó procurado em caso de sucesso, retorna NULL em caso de fracasso
linked_node_t* search_linked_list(linked_list_t* list, int index){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return NULL;
    }

    if(index < 0 || index >= list->length){
        LOG_ERROR("Index out of range");
        return NULL;
    }

    linked_node_t* currentNode = list->head;
    for(int i = 0; i < index; i++){
        if(currentNode == NULL){
            LOG_ERROR("Broken link in the list");
            return NULL;
        }
        currentNode = currentNode->next;
    }

    return currentNode;
}

// --------------- Deletar lista --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t delete_linked_list(linked_list_t* list){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    // Se a lista não está vazia, libera cada nó da lista primeiro
    if(list->length != 0){
        // A variável target_node é usada para apontar para os nós que são liberados
        // e current_node é usada para percorrer a lista
        linked_node_t* target_node, * current_node = list->head;
        for(int i = 0; i < list->length; i++){
            // O nó a ser liberado é o nó atual
            target_node = current_node;

            if(current_node == NULL){
                LOG_ERROR("current_node points to NULL");
                return ERR_DATA;
            }

            // current_node avança
            current_node = current_node->next;

            if(list->listType == LIST_TYPE){
                if(IS_ERROR_STATUS(delete_linked_list(target_node->data.lista))){
                    LOG_ERROR("Couldn't delete target node's list");
                    return ERR_DATA;
                }
            }

            // O nó é liberado
            free(target_node);
        }
    }

    free(list);
    return 1;
}

// ------------ Pesquisa por Dado ------------
// Retorna o indíce do nó em caso de sucesso, retorna -1 em caso de fracasso
int search_data_linked_list(linked_list_t* list, node_data_t data){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return -1;
    }

    if(list->length < 1){
        LOG_ERROR("Empty list");
        return -1;
    }

    linked_node_t* currentNode = list->head;

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
        LOG_ERROR("Invalid list type");
        return -1;
        break;
    }

    for(int i = 0; i < list->length; i++){
        if(currentNode == NULL){
            LOG_ERROR("Broken link in the list");
            return -1;
        }

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
int search_position_linked_list(linked_list_t* list, position_t searched_position){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return -1;
    }

    if(list->length < 1){
        LOG_ERROR("Empty list");
        return -1;
    }

    linked_node_t* current_node = list->head;

    for(int i = 0; i < list->length; i++){
        position_t current_position;

        if(current_node == NULL){
            LOG_ERROR("Broken link in the list");
            return -1;
        }

        switch (list->listType)
        {
        case CHARACTER_TYPE:
            current_position = current_node->data.personagem.position;
            break;
        
            
        case ENEMY_TYPE:
            current_position = current_node->data.inimigo.position;
            break;
        
            
        case ITEM_TYPE:
            current_position = current_node->data.item.position;
            break;
        
        case OBSTACLE_TYPE:
            current_position = current_node->data.obstaculo.position;
            break;
        
        default:
            LOG_ERROR("Invalid list type");
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
status_t print_list(linked_list_t* list, FILE* file){
    if(list == NULL){
        LOG_ERROR("Invalid list");
        return ERR_INVALID_IN;
    }

    linked_node_t* current_node = list->head;
    int mode = list->listType;
    for(int i = 0; i < list->length; i++){
        if(current_node == NULL){
            LOG_ERROR("Broken link in the list");
            return ERR_DATA;
        }
        switch (mode)
        {
        #ifdef JOGO

        case CHARACTER_TYPE:
            fprintf(file, "\nPersonagem  %d\n", i + 1);

            print_character(current_node->data.personagem, file);
            break;
        
        case ENEMY_TYPE:
            fprintf(file, "\nInimigo  %d\n", i + 1);

            print_enemy(current_node->data.inimigo, file);
            break;
        
        case ITEM_TYPE:
            fprintf(file, "\nItem  %d\n", i + 1);

            print_item(current_node->data.item, file);
            break;
        
        case MOVE_TYPE:
            fprintf(file, "\nMovimento  %d\n", i + 1);

            print_move(current_node->data.movimento, file);
            break;
            
        case OBSTACLE_TYPE:
            fprintf(file, "\nObstaculo  %d\n", i + 1);

            print_obstacle(current_node->data.obstaculo, file);
            break;

        #endif
        
        case LIST_TYPE:
            fprintf(file, "\nLista  %d\n", i + 1);

            linked_list_t* current_list = current_node->data.lista;

            fprintf(file, "Lista de ");

            switch (current_list->listType)
            {
            case CHARACTER_TYPE:
                fprintf(file, "personagem\n");
                break;
            
            case ENEMY_TYPE:
                fprintf(file, "inimigo\n");
                break;
            
            case ITEM_TYPE:
                fprintf(file, "item\n");
                break;
            
            case MOVE_TYPE:
                fprintf(file, "movimento\n");
                break;
            
            case LIST_TYPE:
                fprintf(file, "lista\n");
                break;
            
            default:
                break;
            }

            break;
        
        default:
            LOG_ERROR("Invalid mode");
            return ERR_INVALID_IN;
            break;
        }

        current_node = current_node->next;
    }

    return SUCCESS;
}

#endif

// Double Linked List - Métodos

#ifdef DOUBLELINKEDLIST



#endif

// Queue - Métodos

#ifdef QUEUE

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
node_data_t front(queue_t* queue){
    if(queue == NULL){
        LOG_ERROR("Invalid queue");
        return empty_data;
    }

    if (queue->length < 1)
    {
        LOG_ERROR("Empty queue");
        return empty_data;
    }else if (queue->head == NULL)
    {
        LOG_ERROR("Queue head points to NULL");
        return empty_data;
    }else{
        return queue->head->data;
    }
}

// ------------ Enqueue ------------
status_t enqueue(queue_t* queue, node_data_t data){
    if(queue == NULL){
        LOG_ERROR("Invalid queue");
        return ERR_INVALID_IN;
    }

    return insert_linked_list((linked_list_t*) queue, data, queue->length - 1);
}

// ------------ Dequeue ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
node_data_t dequeue(queue_t* queue){
    if(queue == NULL){
        LOG_ERROR("Invalid queue");
        return empty_data;
    }

    node_data_t result = front(queue);

    if(IS_ERROR_STATUS(remove_linked_list((linked_list_t *) queue, 0))){
        LOG_ERROR("Couldn't remove node from queue");
    }
    
    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
status_t print_queue(queue_t* queue, FILE* file){
    if(queue == NULL){
        LOG_ERROR("Invalid queue");
        return ERR_INVALID_IN;
    }

    return print_list((linked_list_t*) queue, file);
}

// --------------- Deletar fila --------------- 
status_t delete_queue(queue_t* queue){
    if(queue == NULL){
        LOG_ERROR("Invalid queue");
        return ERR_INVALID_IN;
    }

    return delete_linked_list((linked_list_t*) queue);
}

#endif

// Stack - Métodos

#ifdef STACK

// ---- Métodos Básicos ----

// ------------ Criar pilha ------------
// Retorna a pilha criada em caso de sucesso, retorna NULL em caso de fracasso
stack_t* create_stack(int listType){
    stack_t* newStack = (stack_t*) create_linked_list(listType);

    // Retorna a nova pilha
    return newStack;
}

// ------------ Top ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
node_data_t top(stack_t* stack){
    if(stack == NULL){
        LOG_ERROR("Invalid stack");
        return empty_data;
    }

    if (stack->length < 1)
    {
        LOG_ERROR("Empty stack");
        return empty_data;
    }else if (stack->head == NULL)
    {
        LOG_ERROR("Stack head points to NULL");
        return empty_data;
    }else{
        return stack->head->data;
    }
}

// ------------ Push ------------
status_t push(stack_t* stack, node_data_t data){
    if(stack == NULL){
        LOG_ERROR("Invalid stack");
        return ERR_INVALID_IN;
    }

    return insert_linked_list((linked_list_t*) stack, data, -1);
}

// ------------ Pop ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
node_data_t pop(stack_t* stack){
    if(stack == NULL){
        LOG_ERROR("Invalid stack");
        return empty_data;
    }

    node_data_t result = top(stack);

    if(IS_ERROR_STATUS(remove_linked_list((linked_list_t *) stack, 0))){
        LOG_ERROR("Couldn't remove node from stack");
    }

    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
status_t print_stack(stack_t* stack){
    if(stack == NULL){
        LOG_ERROR("Invalid stack");
        return ERR_INVALID_IN;
    }

    return print_list((linked_list_t*) stack);
}

// --------------- Deletar pilha --------------- 
status_t delete_stack(stack_t* stack){
    if(stack == NULL){
        LOG_ERROR("Invalid stack");
        return ERR_INVALID_IN;
    }

    return delete_linked_list((linked_list_t*) stack);
}

#endif

// Deque - Métodos

#ifdef DEQUE

#endif