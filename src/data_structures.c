#include "utils\data_structures.h"

// Jogo - Métodos 

#ifdef JOGO

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
int compare_character(node_data_t d1, node_data_t d2){
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
int compare_enemy(node_data_t d1, node_data_t d2){
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
int compare_item(node_data_t d1, node_data_t d2){
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
int compare_move(node_data_t d1, node_data_t d2){
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
int compare_obstacle(node_data_t d1, node_data_t d2){
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
int compare_event(node_data_t d1, node_data_t d2){
    event_t o1 = d1.evento;
    event_t o2 = d2.evento;
    if(
        memcmp(&o1, &o2, sizeof(event_t)) == 0
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
status_t print_position(position_t p, FILE* file){
    if(fprintf(file ,"posicao: [%d, %d]\n", p.x, p.y) == -1) return ERR_INTERNAL;

    fflush(file);

    return SUCCESS;
}

// ---------------- Imprimir personagem ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_character(character_t p, FILE* file){
    if(fprintf(file ,"vida: %d\nmana: %d\n", p.life_points, p.mana_points) == -1) return ERR_INTERNAL;

    return print_position(p.position, file);
}

// ---------------- Imprimir inimigo ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_enemy(enemy_t i, FILE* file){
    if(fprintf(file ,"vida: %d\n", i.life_points) == -1) return ERR_INTERNAL;

    return print_position(i.position, file);
}

// ---------------- Imprimir item ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_item(item_t i, FILE* file){
    if(fprintf(file ,"valor: %d\n", i.valor) == -1) return ERR_INTERNAL;

    return print_position(i.position, file);
}

// ---------------- Imprimir movimento ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_move(action_t m, FILE* file){
    if(fprintf(file ,"acao: %c\ndirecao: %c\n", m.acao, m.direcao) == -1) return ERR_INTERNAL;

    fflush(file);

    return 1;
}

// ---------------- Imprimir obstaculo ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_obstacle(obstacle_t o, FILE* file){
    return print_position(o.position, file);
}

// ---------------- Imprimir evento ---------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t print_event(event_t event, FILE* file){
    switch (event.event_type)
    {
    case MOVE:
    {
        // Caso o main entity seja um character
        if(event.main_entity_type == CHARACTER){
            character_t player = event.main_entity.character;

            if(fprintf(file ,"player moveu para a ") == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(player.position, file))) return ERR_INTERNAL;
        }

        // Caso o main entity seja um enemy
        else if(event.main_entity_type == ENEMY){
            enemy_t enemy = event.main_entity.enemy;

            if(fprintf(file ,"Inimigo moveu para a ") == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(enemy.position, file))) return ERR_INTERNAL;
        }

        if(fprintf(file ,"\n") == -1) return ERR_INTERNAL;

        break;
    }
    case COMBAT:
    {
        // Caso o main entity seja um character e o secundary seja um enemy
        if(event.main_entity_type == CHARACTER && event.secundary_entity_type == ENEMY){
            character_t player = event.main_entity.character;
            enemy_t enemy = event.secundary_entity.enemy;

            if(fprintf(file ,"Player combateu o inimigo na ") == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(enemy.position, file))) return ERR_INTERNAL;

            if(fprintf(file ,"Vida atual do Player: %d      Vida atual do inimigo: %d\n\n", player.life_points, enemy.life_points) == -1) return ERR_INTERNAL;
        }

        // Caso o main entity seja um enemy e o secundary seja um character
        else if(event.main_entity_type == ENEMY && event.secundary_entity_type == CHARACTER){
            character_t player = event.secundary_entity.character;
            enemy_t enemy = event.main_entity.enemy;

            if(fprintf(file ,"O inimigo combateu o player na ") == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(player.position, file))) return ERR_INTERNAL;

            if(fprintf(file ,"Vida atual do Player: %d      Vida atual do inimigo: %d\n\n", player.life_points, enemy.life_points) == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity types\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }
        
        break;
    }
    case ITEM_COLLECTED:
    {
        // Caso o main entity seja um character e o secundary seja um enemy
        if(event.main_entity_type == CHARACTER && event.secundary_entity_type == ITEM){
            character_t player = event.main_entity.character;
            item_t item = event.secundary_entity.item;

            if(fprintf(file ,"Player coletou o item na ") == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(item.position, file))) return ERR_INTERNAL;

            if(fprintf(file ,"Mana atual do Player: %d      Valor do item: %d\n\n", player.mana_points, item.valor) == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity types\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }

        break;
    }
    case SKILL:
    {
        // Caso o main entity seja um character
        if(event.main_entity_type == CHARACTER){
            character_t player = event.main_entity.character;
            skill_t skill = event.auxiliar_data.skill;

            if(fprintf(file ,"Player usou a skill ") == -1) return ERR_INTERNAL;

            switch (skill)
            {
            case HEALING_SPELL:
                if(fprintf(file ,"Feitiço de cura\n") == -1) return ERR_INTERNAL;
                break;
            
            case FIREBALL:
                if(fprintf(file ,"Feitiço de bola de fogo\n") == -1) return ERR_INTERNAL;
                break;
            
            case LIGHTNING:
                if(fprintf(file ,"Feitiço de relampago\n") == -1) return ERR_INTERNAL;
                break;
            
            default:
                break;
            }

            if(fprintf(file ,"Mana atual do Player: %d\n\n", player.mana_points) == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity type\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }

        break;
    }
    case DAMAGE:
    {
        // Caso o main entity seja um character
        if(event.main_entity_type == CHARACTER){
            character_t player = event.main_entity.character;
            int32_t damage = event.auxiliar_data.life_points;

            if(fprintf(file ,"Player levou %d de dano\n", damage) == -1) return ERR_INTERNAL;

            if(fprintf(file ,"Vida atual do Player: %d\n\n", player.life_points) == -1) return ERR_INTERNAL;
        }

        // Caso o main entity seja um enemy
        else if(event.main_entity_type == ENEMY){
            enemy_t enemy = event.main_entity.enemy;
            int32_t damage = event.auxiliar_data.life_points;

            if(fprintf(file ,"Inimigo levou %d de dano\n", damage) == -1) return ERR_INTERNAL;

            if(fprintf(file ,"Vida atual do inimigo: %d\t", enemy.life_points) == -1) return ERR_INTERNAL;

            if(IS_ERROR_STATUS(print_position(enemy.position, file))) return ERR_INTERNAL;
            if(fprintf(file, "\n") == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity types\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }

        break;
    }
    case VICTORY:
    {
        // Caso o main entity seja um character
        if(event.main_entity_type == CHARACTER){
            character_t player = event.main_entity.character;

            if(fprintf(file ,"Player venceu\nVida do personagem: %d      Mana do personagem: %d\n\n", player.life_points, player.mana_points) == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity types\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }
        
        break;
    }
    case DEFEAT:
    {
        // Caso o main entity seja um character
        if(event.main_entity_type == CHARACTER){
            character_t player = event.main_entity.character;

            if(fprintf(file ,"Player perdeu\nVida do personagem: %d      Mana do personagem: %d\n\n", player.life_points, player.mana_points) == -1) return ERR_INTERNAL;
        }

        // Caso não seja desses tipos, dá erro
        else{
            fputs("ERROR - function print_event: Invalid entity types\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }

        break;
    }
    default:
        break;
    }

    fflush(file);

    return SUCCESS;
}

#endif

// Linked List - Métodos

#ifdef LINKEDLIST

// ---- Métodos Básicos ----

// ---------------- Comparar listas ---------------- 
// Retorna 1 em caso do dado estar vazio, retorna 0 caso contrário
bool is_data_empty(node_data_t d){
    return (d.lista == NULL);
}

// ---------------- Comparar listas ---------------- 
// Retorna 1 em caso das listas forem as mesmas, retorna 0 caso contrário
int compare_list(node_data_t d1, node_data_t d2){
    linked_list_t* l1 = d1.lista;
    linked_list_t* l2 = d2.lista;
    return (l1 == l2);
}

// ---------------- Criar lista ---------------- 
// Retorna o endereço da lista em caso de sucesso, retorna NULL em caso de fracasso
linked_list_t* create_linked_list(list_type_t listType){
    if(!(listType >= CHARACTER_TYPE && listType <= LIST_TYPE)){
        fputs("ERROR - function create_linked_list: Invalid list type\n", stderr);
        fflush(stderr);
        return NULL;
    }

    // Aloca o espaço na memória para uma nova lista
    linked_list_t* newList = (linked_list_t*) malloc(sizeof(linked_list_t));

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
status_t insert_linked_list(linked_list_t* list, node_data_t data, int index){
    // Se o index for igual ou maior que 0
    if(index >= 0){
        if(index >= list->length){
            fputs("ERROR - function insert_linked_list: Index out of range\n", stderr);
            fflush(stderr);
            return ERR_INVALID_IN;
        }

        // O local onde o nó vai ser posto é encontrado
        linked_node_t* currentNode = search_linked_list(list, index);

        // Verifica se encontrou o local na lista
        if(currentNode == NULL){
            fputs("ERROR - function insert_linked_list: Couldn't find target node\n", stderr);
            fflush(stderr);
            return ERR_DATA;
        }

        // O espaço para o novo nó é alocado
        linked_node_t* newNode = (linked_node_t*) malloc(sizeof(linked_node_t));

        // Verifica se memória foi alocada ou não para o novo nó
        if(newNode == NULL){
            fputs("ERROR - function insert_linked_list: Couldn't allocate memory for a new node\n", stderr);
            fflush(stderr);
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
            fputs("ERROR - function insert_linked_list: Couldn't allocate memory for a new node\n", stderr);
            fflush(stderr);
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
    if(index < 0 || index >= list->length){
        fputs("ERROR - function remove_linked_list: Index out of range\n", stderr);
        fflush(stderr);
        return ERR_INVALID_IN;
    }

    // Caso não seja o primeiro item da lista
    if(index > 0){
        // Pega o nó alvo e o nó anterior a ele
        linked_node_t* previousNode = search_linked_list(list, index - 1);
        if(previousNode == NULL){
            fputs("ERROR - function remove_linked_list: Couldn't find target node\n", stderr);
            fflush(stderr);
            return ERR_DATA;
        }
        linked_node_t* targetNode = previousNode->next;

        if(list->listType == LIST_TYPE){
            if(IS_ERROR_STATUS(delete_linked_list(targetNode->data.lista))){
                fputs("ERROR - function remove_linked_list: Couldn't delete target node's list\n", stderr);
                fflush(stderr);
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
            fputs("ERROR - function remove_linked_list: List head points to NULL\n", stderr);
            fflush(stderr);
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
    if(index < 0 || index >= list->length){
        fputs("ERROR - function search_linked_list: Index out of range\n", stderr);
        fflush(stderr);
        printf("Index: %d\nList Length: %d\n", index, list->length);
        return NULL;
    }

    linked_node_t* currentNode = list->head;
    for(int i = 0; i < index; i++){
        currentNode = currentNode->next;
    }

    return currentNode;
}

// --------------- Deletar lista --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t delete_linked_list(linked_list_t* list){
    // Se a lista não está vazia, libera cada nó da lista primeiro
    if(list->length != 0){
        // A variável target_node é usada para apontar para os nós que são liberados
        // e current_node é usada para percorrer a lista
        linked_node_t* target_node, * current_node = list->head;
        for(int i = 0; i < list->length; i++){
            // O nó a ser liberado é o nó atual
            target_node = current_node;

            if(current_node == NULL){
                fputs("ERROR - function delete_linked_list: current_node points to NULL\n", stderr);
                fflush(stderr);
                return ERR_DATA;
            }

            // current_node avança
            current_node = current_node->next;

            if(list->listType == LIST_TYPE){
                if(IS_ERROR_STATUS(delete_linked_list(target_node->data.lista))){
                    fputs("ERROR - function delete_linked_list: Couldn't delete target node's list\n", stderr);
                    fflush(stderr);
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
    if(list->length < 1){
        fputs("ERROR - function search_data_linked_list: Empty list\n", stderr);
        fflush(stderr);
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
int search_position_linked_list(linked_list_t* list, position_t searched_position){
    if(list->length < 1){
        fputs("ERROR - function search_data_linked_list: Empty list\n", stderr);
        fflush(stderr);
        return -1;
    }

    linked_node_t* current_node = list->head;

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
status_t print_list(linked_list_t* list, FILE* file){
    linked_node_t* current_node = list->head;
    int mode = list->listType;
    for(int i = 0; i < list->length; i++){
        if(current_node == NULL){
            fputs("ERROR - function print_list: current_node points to NULL\n", stderr);
            fflush(stderr);
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
            fputs("ERROR - function print_list: Invalid mode\n", stderr);
            fflush(stderr);
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
    if (queue->length < 1)
    {
        fputs("ERROR - function front: Empty queue\n", stderr);
        fflush(stderr);
        node_data_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else if (queue->head == NULL)
    {
        fputs("ERROR - function front: Queue head points to NULL\n", stderr);
        fflush(stderr);
        node_data_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else{
        return queue->head->data;
    }
}

// ------------ Enqueue ------------
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t enqueue(queue_t* queue, node_data_t data){
    return insert_linked_list((linked_list_t*) queue, data, queue->length - 1);
}

// ------------ Dequeue ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
node_data_t dequeue(queue_t* queue){
    node_data_t result = front(queue);

    if(IS_ERROR_STATUS(remove_linked_list((linked_list_t *) queue, 0))){
        fputs("ERROR - function dequeue: Couldn't remove node from queue\n", stderr);
        fflush(stderr);
    }
    
    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
status_t print_queue(queue_t* queue, FILE* file){
    return print_list((linked_list_t*) queue, file);
}

// --------------- Deletar fila --------------- 
// Retorna 1 em caso de sucesso, retorna 0 em caso de fracasso
status_t delete_queue(queue_t* queue){
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
    if (stack->length < 1)
    {
        fputs("ERROR - function top: Empty stack\n", stderr);
        fflush(stderr);
        node_data_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else if (stack->head == NULL)
    {
        fputs("ERROR - function top: Stack head points to NULL\n", stderr);
        fflush(stderr);
        node_data_t emptyData;
        emptyData.lista = NULL;
        return emptyData;
    }else{
        return stack->head->data;
    }
}

// ------------ Push ------------
status_t push(stack_t* stack, node_data_t data){
    return insert_linked_list((linked_list_t*) stack, data, -1);
}

// ------------ Pop ------------
// Retorna o dado do primeiro elemento em caso de sucesso, não retorna nada em caso de fracasso
node_data_t pop(stack_t* stack){
    node_data_t result = top(stack);

    if(IS_ERROR_STATUS(remove_linked_list((linked_list_t *) stack, 0))){
        fputs("ERROR - function pop: Couldn't remove node from stack\n", stderr);
        fflush(stderr);
    }

    return result;
}

// ------------ Impressão da Fila ------------
// Printa cada item da Fila
status_t print_stack(stack_t* stack){
    return print_list((linked_list_t*) stack);
}

// --------------- Deletar pilha --------------- 
status_t delete_stack(stack_t* stack){
    return delete_linked_list((linked_list_t*) stack);
}

#endif

// Deque - Métodos

#ifdef DEQUE

#endif