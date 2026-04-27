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
    Movimento i;

    printf("Digite a acao ('M' para movimento, 'A' para ataque): ");
    fflush(stdout);
    scanf("%c", &(i.acao));
    getchar();

    printf("Digite a direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): ");
    fflush(stdout);
    scanf("%c", &(i.direcao));
    getchar();

    if(insert_linked_list(lista, (nodeData_t) i, lista->length - 1) == 0){
        printf("Nao foi possivel adicionar o movimento a lista\n");
        fflush(stdout);
    }
}

void remover(){
    int input;

    printf("Digite a posicao do movimento na lista: ");
    fflush(stdout);
    scanf("%d", &input);

    if(remove_linked_list(lista, input - 1) == 0){
        printf("Nao foi possivel remover o movimento a lista\n");
        fflush(stdout);
    }
}

void pesquisar(){
    Movimento i;

    printf("Digite a acao ('M' para movimento, 'A' para ataque): ");
    fflush(stdout);
    scanf("%c", &(i.acao));
    getchar();

    printf("Digite a direcao ('W' para cima, 'S' para baixo, 'A' para esquerda, 'D' para direita): ");
    fflush(stdout);
    scanf("%c", &(i.direcao));
    getchar();

    int index = search_data_linked_list(lista, (nodeData_t) i);

    if(index == -1){
        printf("Nao foi possivel encontrar o movimento na lista\n");
        fflush(stdout);
    }else{
        printf("O movimento esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

void imprimirLista(){
    printf("\n----------------- Lista de Movimentos ----------------\n");
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
    printf("1 - Adicionar movimento em uma lista\n");
    printf("2 - Remover movimento da lista\n");
    printf("3 - Pesquisar movimento\n");
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
    lista = create_linked_list(MOVE_TYPE);
    while(menu());

    #endif

    return 0;
}