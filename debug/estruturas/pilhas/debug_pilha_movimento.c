#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define JOGO
#define LINKEDLIST
#define STACK
#include "C:\Users\tassio.santos\Documents\Fase 3\Jogo_RPG\data_structures.c"

stack_t* pilha;

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

    if(push(pilha, (nodeData_t) i) == 0){
        printf("Nao foi possivel adicionar o movimento a pilha\n");
        fflush(stdout);
    }
}

void remover(){
    nodeData_t d = pop(pilha);
    if(isDataEmpty(d)){
        printf("Nao foi possivel remover item da pilha\n");
        return;
    }
    printf("Movimento removido: \n");
    print_move(d.movimento);
}

void ultimoMovimento(){
    nodeData_t d = top(pilha);
    if(isDataEmpty(d)){
        printf("Nao foi possivel acessar o ultimo movimento da pilha\n");
        return;
    }
    printf("Ultimo movimento: \n");
    print_move(d.movimento);
}

void imprimirPilha(){
    printf("\n----------------- Pilha de Movimentos ----------------\n");
    fflush(stdout);

    print_stack(pilha);

    printf("\n-------------------------------------------------------\n");
    fflush(stdout);
}

int menu(){
    printf("==================================================\n");
    printf("================= Menu de pilha ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar movimento na pilha\n");
    printf("2 - Remover movimento da pilha\n");
    printf("3 - Imprimir ultimo movimento\n");
    printf("4 - Imprimir pilha\n");
    printf("5 - Sair do sistema\n");

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
        ultimoMovimento();
        break;
    
    case 4:
        imprimirPilha();
        break;
    
    case 5:
        delete_stack(pilha);
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

int main(){
    pilha = create_stack(MOVE_TYPE);
    while(menu());

    return 0;
}