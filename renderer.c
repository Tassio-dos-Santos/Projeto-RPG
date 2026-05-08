#include "renderer.h"

// Thread do renderizador
void *renderer(){
    while(!end_renderer){
        sem_wait(&render_sem);
        if(end_renderer) break;

        pthread_mutex_lock(&board_mutex);
        if(end_renderer) break;

        render_board();
        display_updated = 1;

        pthread_mutex_unlock(&board_mutex);
    }
}

// Processa a renderização
void render_board(){
    #ifndef DEBUG
    system("cls");
    #endif
    printf("Vida do personagem: %d \tMana do personagem: %d\n\n", player->life_points, player->mana_points);
    for(int i = boardSize - 1; i > - 1; i--){
        for(int j = 0; j < boardSize; j++){
            switch (board[i][j])
            {
            case 'P':
                printf("\x1b[32m");
                break;
            
            case 'E':
                printf("\x1b[31m");
                break;
            
            case 'I':
                printf("\x1b[34m");
                break;
            
            case 'X':
                printf("\x1b[30m");
                break;
            
            case '~':
                printf("\x1b[1;36m");
                break;
            
            case '@':
                printf("\x1b[1;31m");
                break;
            
            default:
                break;
            }
            printf(" %c\x1b[0m", board[i][j]);
        }
        printf("\n\0");
    }
    printf("\n\n");
}

// Limpa a tela e mostrar as informações do personagem e o tabuleiro
void mostrar_tabuleiro(){
    display_updated = 0;
    sem_post(&render_sem);
}