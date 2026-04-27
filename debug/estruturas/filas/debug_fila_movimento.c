#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define JOGO
#define LINKEDLIST
#define QUEUE
#include "C:\Users\tassio.santos\Documents\Fase 3\Jogo_RPG\data_structures.c"

queue_t* fila;

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

    if(enqueue(fila, (nodeData_t) i) == 0){
        printf("Nao foi possivel adicionar o movimento a fila\n");
        fflush(stdout);
    }
}

void remover(){
    nodeData_t d = dequeue(fila);
    if(isDataEmpty(d)){
        printf("Nao foi possivel remover item da fila\n");
        return;
    }
    printf("Movimento removido: \n");
    print_move(d.movimento);
}

void proximoMovimento(){
    nodeData_t d = front(fila);
    if(isDataEmpty(d)){
        printf("Nao foi possivel acessar o proximo movimento da fila\n");
        return;
    }
    printf("Proximo movimento: \n");
    print_move(d.movimento);
}

void imprimirFila(){
    printf("\n----------------- Fila de Movimentos ----------------\n");
    fflush(stdout);

    print_queue(fila);

    printf("\n-------------------------------------------------------\n");
    fflush(stdout);
}

int menu(){
    printf("==================================================\n");
    printf("================= Menu de lista ==================\n");
    printf("==================================================\n");

    printf("Selecione uma opcao abaixo: \n");
    printf("1 - Adicionar movimento na fila\n");
    printf("2 - Remover movimento da fila\n");
    printf("3 - Imprimir proximo movimento\n");
    printf("4 - Imprimir lista\n");
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
        proximoMovimento();
        break;
    
    case 4:
        imprimirFila();
        break;
    
    case 5:
        delete_queue(fila);
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
    fila = create_queue(MOVE_TYPE);
    while(menu());

    return 0;
}