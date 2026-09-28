#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define MAX 10

typedef enum {PRONTO, BLOQUEADO, EXECUTANDO, TERMINADO} Estado;
typedef enum {NENHUM, LEITURA, ESCRITA} OpPendente;

typedef struct Processo 
{
    int pid;
    int pc; // Contador de processo 
    int n;
    Estado estado;
    OpPendente opPendente;
    int acessosLeitura;
    int acessosEscrita;
} Processo;

int main(int argc, char *argv[])
{
    int pc = 0;
    int n = 0;
    Processo *processos = (Processo *)shmat(atoi(argv[2]), NULL, 0);
    if (processos ==  (void *)-1)
    {
        perror("Erro ao anexar memoria compartilhada com shmat");
        exit(1);
    }

    for(int i = 0; i < MAX; i++)
    {
        usleep(500000);
        processos[atoi(argv[1])].pc++;
    }

    return 0;
}