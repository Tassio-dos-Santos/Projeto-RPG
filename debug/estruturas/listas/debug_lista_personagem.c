#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define JOGO
#define LINKEDLIST
#include "C:\Users\tassio.santos\Documents\Fase 3\Jogo_RPG\data_structures.c"

#define DEBUG

#ifdef DEBUG

linkedList_t* lista;

void adicionarPersonagem(){
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

    if(insert_linked_list(lista, (nodeData_t) p, lista->length - 1) == 0){
        printf("Nao foi possivel adicionar o personagem a lista\n");
        fflush(stdout);
    }
}

void removerPersonagem(){
    int input;

    printf("Digite a posicao do personagem na lista: ");
    fflush(stdout);
    scanf("%d", &input);

    if(remove_linked_list(lista, input - 1) == 0){
        printf("Nao foi possivel remover o personagem a lista\n");
        fflush(stdout);
    }
}

void pesquisarPersonagem(){
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

    int index = search_data_linked_list(lista, (nodeData_t) p);

    if(index == -1){
        printf("Nao foi possivel encontrar o personagem a lista\n");
        fflush(stdout);
    }else{
        printf("O personagem esta na posicao %d!\n", index + 1);
        fflush(stdout);
    }
}

void imprimirLista(){
    printf("\n----------------- Lista de Personagem -----------------\n");
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
    printf("1 - Adicionar personagem em uma lista\n");
    printf("2 - Remover personagem da lista\n");
    printf("3 - Pesquisar personagem\n");
    printf("4 - Imprimir lista\n");
    printf("5 - Excluir lista\n");
    printf("6 - Sair do sistema\n");

    int opcao;
    scanf("%d", &opcao);
    getchar();

    switch (opcao){
    case 1:
        adicionarPersonagem();
        break;
    
    case 2:
        removerPersonagem();
        break;
    
    case 3:
        pesquisarPersonagem();
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
    lista = create_linked_list(CHARACTER_TYPE);
    while(menu());

    #endif

    return 0;
}