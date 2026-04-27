#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define JOGO
#define LINKEDLIST
#include "C:\Users\tassio.santos\Documents\Fase 3\Jogo_RPG\data_structures.c"

#define DEBUG

#ifdef DEBUG

linkedList_t* lista_principal;

// Declaração das funções

void adicionarLista();
void removerLista();
void acessarLista();
int menu_character(linkedList_t* lista_acessada);
void adicionarPersonagem(linkedList_t* lista_acessada);
void pesquisarPersonagem(linkedList_t* lista_acessada);
int menu_enemy(linkedList_t* lista_acessada);
void adicionarInimigo(linkedList_t* lista_acessada);
void pesquisarInimigo(linkedList_t* lista_acessada);
int menu_item(linkedList_t* lista_acessada);
void adicionarItem(linkedList_t* lista_acessada);
void pesquisarItem(linkedList_t* lista_acessada);
int menu_move(linkedList_t* lista_acessada);
void adicionarMovimento(linkedList_t* lista_acessada);
void pesquisarMovimento(linkedList_t* lista_acessada);
void imprimirLista(linkedList_t* lista_acessada);
void removerNo(linkedList_t* lista_acessada);
void printListas();
int menu();

void adicionarLista(){
    printf("\nSelecione uma opcao abaixo: \n");
    printf("1 - Lista de Personagens\n");
    printf("2 - Lista de Inimigos\n");
    printf("3 - Lista de Itens\n");
    printf("4 - Lista de Movimentos\n");
    printf("5 - Voltar\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    linkedList_t* newList;

    switch (opcao){
    case 1:
        newList = create_linked_list(CHARACTER_TYPE);
        if(newList == NULL){
            printf("\nLista de personagens nao foi criada.\n");
            return;
        }
        insert_linked_list(lista_principal, (nodeData_t) newList, lista_principal->length - 1);
        printf("\nLista de personagens criada!\n");
        break;
    
    case 2:
        newList = create_linked_list(ENEMY_TYPE);
        if(newList == NULL){
            printf("\nLista de inimigos nao foi criada.\n");
            return;
        }
        insert_linked_list(lista_principal, (nodeData_t) newList, lista_principal->length - 1);
        printf("\nLista de inimigos criada!\n");
        break;
    
    case 3:
        newList = create_linked_list(ITEM_TYPE);
        if(newList == NULL){
            printf("\nLista de itens nao foi criada.\n");
            return;
        }
        insert_linked_list(lista_principal, (nodeData_t) newList, lista_principal->length - 1);
        printf("\nLista de itens criada!\n");
        break;
    
    case 4:
        newList = create_linked_list(MOVE_TYPE);
        if(newList == NULL){
            printf("\nLista de movimentos nao foi criada.\n");
            return;
        }
        insert_linked_list(lista_principal, (nodeData_t) newList, lista_principal->length - 1);
        printf("\nLista de movimentos criada!\n");
        break;

    case 5:
        return;
        break;
    
    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }
}

void removerLista(){
    int input;

    printf("Digite a posicao da lista que voce quer remover: ");
    fflush(stdout);
    scanf("%d", &input);

    if(remove_linked_list(lista_principal, input - 1) == 0){
        printf("Nao foi possivel remover a lista\n");
        fflush(stdout);
    }
}

void acessarLista(){
    int input;

    printf("Digite a posicao da lista que voce quer acessar: ");
    fflush(stdout);
    scanf("%d", &input);

    linkedNode_t* no_acessado = search_linked_list(lista_principal, input - 1);

    linkedList_t* lista_acessada = no_acessado->data.lista;

    switch (lista_acessada->listType)
    {
    case CHARACTER_TYPE:
        while(menu_character(lista_acessada));
        break;
    
    case ENEMY_TYPE:
        while(menu_enemy(lista_acessada));
        break;
    
    case ITEM_TYPE:
        while(menu_item(lista_acessada));
        break;
    
    case MOVE_TYPE:
        while(menu_move(lista_acessada));
        break;
    
    default:
        break;
    }
}

int menu_character(linkedList_t* lista_acessada){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar personagem em uma lista\n");
    printf("2 - Remover personagem da lista\n");
    printf("3 - Pesquisar personagem\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Voltar\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarPersonagem(lista_acessada);
        break;
    
    case 2:
        removerNo(lista_acessada);
        break;
    
    case 3:
        pesquisarPersonagem(lista_acessada);
        break;
    
    case 4:
        imprimirLista(lista_acessada);
        break;
    
    case 5:
        Sleep(2);
        return 0;
        break;

    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }

    return 1;
}

void adicionarPersonagem(linkedList_t* lista_acessada){
    Personagem p;

    printf("Digite a vida do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.vida));
    
    printf("Digite os pontos do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.pontos));

    printf("Digite a posicao x do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.x));

    printf("Digite a posicao y do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.y));

    if(insert_linked_list(lista_acessada, (nodeData_t) p, lista_acessada->length - 1) == 0){
        printf("Nao foi possivel adicionar o personagem a lista\n");
        fflush(stdout);
    }
}

void pesquisarPersonagem(linkedList_t* lista_acessada){
    Personagem p;

    printf("Digite a vida do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.vida));
    
    printf("Digite os pontos do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.pontos));

    printf("Digite a posicao x do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.x));

    printf("Digite a posicao y do personagem: ");
    fflush(stdout);
    scanf("%d", &(p.y));

    int index = search_data_linked_list(lista_acessada, (nodeData_t) p);

    if(index == -1){
        printf("Nao foi possivel encontrar o personagem a lista\n");
        fflush(stdout);
    }else{
        printf("O personagem esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

int menu_enemy(linkedList_t* lista_acessada){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar inimigo em uma lista\n");
    printf("2 - Remover inimigo da lista\n");
    printf("3 - Pesquisar inimigo\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Voltar\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarInimigo(lista_acessada);
        break;
    
    case 2:
        removerNo(lista_acessada);
        break;
    
    case 3:
        pesquisarInimigo(lista_acessada);
        break;
    
    case 4:
        imprimirLista(lista_acessada);
        break;
    
    case 5:
        Sleep(2);
        return 0;
        break;
    
    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }

    return 1;
}

void adicionarInimigo(linkedList_t* lista_acessada){
    Inimigo i;

    printf("Digite a vida do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.vida));

    printf("Digite a posicao x do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.x));

    printf("Digite a posicao y do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.y));

    if(insert_linked_list(lista_acessada, (nodeData_t) i, lista_acessada->length - 1) == 0){
        printf("Nao foi possivel adicionar o inimigo a lista\n");
        fflush(stdout);
    }
}

void pesquisarInimigo(linkedList_t* lista_acessada){
    Inimigo i;

    printf("Digite a vida do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.vida));

    printf("Digite a posicao x do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.x));

    printf("Digite a posicao y do inimigo: ");
    fflush(stdout);
    scanf("%d", &(i.y));

    int index = search_data_linked_list(lista_acessada, (nodeData_t) i);

    if(index == -1){
        printf("Nao foi possivel encontrar o inimigo a lista\n");
        fflush(stdout);
    }else{
        printf("O inimigo esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

int menu_item(linkedList_t* lista_acessada){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar item em uma lista\n");
    printf("2 - Remover item da lista\n");
    printf("3 - Pesquisar item\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Voltar\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarItem(lista_acessada);
        break;
    
    case 2:
        removerNo(lista_acessada);
        break;
    
    case 3:
        pesquisarItem(lista_acessada);
        break;
    
    case 4:
        imprimirLista(lista_acessada);
        break;
    
    case 5:
        Sleep(2);
        return 0;
        break;
    
    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }

    return 1;
}

void adicionarItem(linkedList_t* lista_acessada){
    Item i;

    printf("Digite a valor do item: ");
    fflush(stdout);
    scanf("%d", &(i.valor));

    printf("Digite a posicao x do item: ");
    fflush(stdout);
    scanf("%d", &(i.x));

    printf("Digite a posicao y do item: ");
    fflush(stdout);
    scanf("%d", &(i.y));

    if(insert_linked_list(lista_acessada, (nodeData_t) i, lista_acessada->length - 1) == 0){
        printf("Nao foi possivel adicionar o item a lista\n");
        fflush(stdout);
    }
}

void pesquisarItem(linkedList_t* lista_acessada){
    Item i;

    printf("Digite a valor do item: ");
    fflush(stdout);
    scanf("%d", &(i.valor));

    printf("Digite a posicao x do item: ");
    fflush(stdout);
    scanf("%d", &(i.x));

    printf("Digite a posicao y do item: ");
    fflush(stdout);
    scanf("%d", &(i.y));

    int index = search_data_linked_list(lista_acessada, (nodeData_t) i);

    if(index == -1){
        printf("Nao foi possivel encontrar o item a lista\n");
        fflush(stdout);
    }else{
        printf("O item esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

int menu_move(linkedList_t* lista_acessada){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar movimento em uma lista\n");
    printf("2 - Remover movimento da lista\n");
    printf("3 - Pesquisar movimento\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Voltar\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarMovimento(lista_acessada);
        break;
    
    case 2:
        removerNo(lista_acessada);
        break;
    
    case 3:
        pesquisarMovimento(lista_acessada);
        break;
    
    case 4:
        imprimirLista(lista_acessada);
        break;
    
    case 5:
        Sleep(2);
        return 0;
        break;
    
    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }

    return 1;
}

void adicionarMovimento(linkedList_t* lista_acessada){
    Movimento i;

    printf("Digite a acao ('M' para movimento, 'A' para ataque): ");
    fflush(stdout);
    scanf("%c", &(i.acao));
    getchar();

    printf("Digite a direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): ");
    fflush(stdout);
    scanf("%c", &(i.direcao));
    getchar();

    if(insert_linked_list(lista_acessada, (nodeData_t) i, lista_acessada->length - 1) == 0){
        printf("Nao foi possivel adicionar o movimento a lista\n");
        fflush(stdout);
    }
}

void pesquisarMovimento(linkedList_t* lista_acessada){
    Movimento i;

    printf("Digite a acao ('M' para movimento, 'A' para ataque): ");
    fflush(stdout);
    scanf("%c", &(i.acao));
    getchar();

    printf("Digite a direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): ");
    fflush(stdout);
    scanf("%c", &(i.direcao));
    getchar();

    int index = search_data_linked_list(lista_acessada, (nodeData_t) i);

    if(index == -1){
        printf("Nao foi possivel encontrar o movimento na lista\n");
        fflush(stdout);
    }else{
        printf("O movimento esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

void imprimirLista(linkedList_t* lista_acessada){

    switch (lista_acessada->listType)
    {
    case CHARACTER_TYPE:
        printf("\n----------------- Lista de Personagem -----------------\n");
        break;
    
    case ENEMY_TYPE:
        printf("\n----------------- Lista de Inimigos -----------------\n");
        break;
    
    case ITEM_TYPE:
        printf("\n----------------- Lista de Itens -----------------\n");
        break;
    
    case MOVE_TYPE:
        printf("\n----------------- Lista de Movimentos -----------------\n");
        break;
    
    default:
        break;
    }
    fflush(stdout);

    print_list(lista_acessada);

    printf("\n-------------------------------------------------------\n");
    fflush(stdout);
}

void removerNo(linkedList_t* lista_acessada){
    int input;

    switch (lista_acessada->listType)
    {
    case CHARACTER_TYPE:
        printf("Digite a posicao do personagem na lista: ");
        break;
    
    case ENEMY_TYPE:
        printf("Digite a posicao do inimigo na lista: ");
        break;
    
    case ITEM_TYPE:
        printf("Digite a posicao do item na lista: ");
        break;
    
    case MOVE_TYPE:
        printf("Digite a posicao do movimento na lista: ");
        break;
    
    default:
        break;
    }
    fflush(stdout);
    scanf("%d", &input);

    if(remove_linked_list(lista_acessada, input - 1) == 0){
        printf("Nao foi possivel remove-lo da lista\n");
        fflush(stdout);
    }
}

void printListas(){
    printf("\n----------------- Listas -----------------\n");
    fflush(stdout);

    print_list(lista_principal);

    printf("\n-------------------------------------------------------\n");
    fflush(stdout);
}

int menu(){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar uma lista a lista principal\n");
    printf("2 - Remover lista da lista principal\n");
    printf("3 - Acessar lista\n");
    printf("4 - Imprimir listas\n");
    printf("5 - Sair do sistema\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarLista();
        break;
    
    case 2:
        removerLista();
        break;
    
    case 3:
        acessarLista();
        break;
    
    case 4:
        printListas();
        break;
    
    case 5:
        delete_linked_list(lista_principal);
        printf("Volte sempre!\n");
        Sleep(2);
        return 0;
        break;
    
    default:
        printf("Opcao invalida\n");
        Sleep(2);
        break;
    }

    return 1;
}

#endif

int main(){
    #ifdef DEBUG
    lista_principal = create_linked_list(LIST_TYPE);
    while(menu());

    #endif

    return 0;
}