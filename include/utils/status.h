#ifndef STATUS_H
#define STATUS_H

typedef enum status_t {
    // Sucessos
    // Sucesso padrão, a função fez tudo que se pretendia fazer
    SUCCESS         =  0,

    // Sucesso parcial, a função não fez tudo que se pretendia fazer, mas não por um erro
    PARTIAL_SUCCESS =  1,

    // A ação foi pulada, mas não por um erro
    ACTION_SKIPPED  =  2,


    // Erros
    // Houve erro de manipulação de memória
    ERR_MEMORY      = -1,

    // Houve erro no uso de uma data structure
    ERR_DATA        = -2,

    // Houve um erro relacionado a um input inválido
    ERR_INVALID_IN  = -3,

    // Houve um erro com relação à origem da ação
    ERR_INVALID_SRC = -4,

    // Houve um erro com relação ao destino da ação
    ERR_INVALID_DST = -5,

    // Houve algum erro em função interna
    ERR_INTERNAL    = -6,

    // Houve um erro relacionado a falta de recursos
    ERR_RESOURCES   = -7
} status_t;

#define IS_ERROR_STATUS(s) ((s) < 0)
#define IS_SUCCESS_STATUS(s) ((s) >= 0)

#endif