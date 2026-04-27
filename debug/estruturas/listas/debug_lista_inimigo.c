#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define JOGO
#define LINKEDLIST
#include "C:\Users\tassio.santos\Documents\Fase 3\Jogo_RPG\data_structures.c"

#define DEBUG

#ifdef DEBUG

linkedList_t* lista;

void adicionar(){
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

    if(insert_linked_list(lista, (nodeData_t) i, lista->length - 1) == 0){
        printf("Nao foi possivel adicionar o inimigo a lista\n");
        fflush(stdout);
    }
}

void remover(){
    int input;

    printf("Digite a posicao do inimigo na lista: ");
    fflush(stdout);
    scanf("%d", &input);

    if(remove_linked_list(lista, input - 1) == 0){
        printf("Nao foi possivel remover o inimigo a lista\n");
        fflush(stdout);
    }
}

void pesquisar(){
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

    int index = search_data_linked_list(lista, (nodeData_t) i);

    if(index == -1){
        printf("Nao foi possivel encontrar o inimigo a lista\n");
        fflush(stdout);
    }else{
        printf("O inimigo esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

void imprimirLista(){
    printf("\n----------------- Lista de Inimigos -----------------\n");
    fflush(stdout);

    print_list(lista);

    printf("\n-------------------------------------------------------\n");
    fflush(stdout);
}

void excluirLista(){
    delete_linked_list(lista);
}

int menu(){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar inimigo em uma lista\n");
    printf("2 - Remover inimigo da lista\n");
    printf("3 - Pesquisar inimigo\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Excluir lista\n");
    printf("6 - Sair do sistema\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionar();
        break;
    
    case 2:
        remover();
        break;
    
    case 3:
        pesquisar();
        break;
    
    case 4:
        imprimirLista();
        break;
    
    case 5:
        excluirLista();
        printf("Volte sempre!\n");
        Sleep(2);
        return 0;
        break;
    
    case 6:
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
    lista = create_linked_list(ENEMY_TYPE);
    while(menu());

    #endif

    return 0;
}