#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define MAX 20 // Lembrar de no teste final, usar entre 5000 e 10000 como pede o enunciado

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

Processo *processos = NULL;
int indice = 0;

// Funcao de syscall emulada (Etapa 6)
void chamar_syscall(OpPendente op, int *N)
{
    // 1. Grava a operacao pendente no PCB da memoria compartilhada
    processos[indice].opPendente = op;

    // 2. Notifica o KernelSim via pipe (evento = 10 + indice do processo)
    int evento = 10 + indice;
    write(1, &evento, sizeof(int));

    // 3. Aguarda o KernelSim processar a syscall e liberar (opPendente voltar a NENHUM)
    // O usleep impede que o compilador otimize o laço e evita consumo excessivo de CPU
    while (processos[indice].opPendente != NENHUM)
    {
        usleep(1000);
    }

    // 4. Se a operacao foi leitura, atualiza a variavel local N com o valor recebido no PCB
    if (op == LEITURA)
    {
        *N = processos[indice].n;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        exit(1);
    }

    indice = atoi(argv[1]);
    int shmid = atoi(argv[2]);

    processos = (Processo *)shmat(shmid, NULL, 0);
    if (processos == (void *)-1)
    {
        perror("Erro ao anexar memoria compartilhada com shmat");
        exit(1);
    }

    // Semente aleatoria baseada no PID para garantir sequencias unicas por processo
    srand(getpid());

    int PC = 1;
    int N = 0;

    // Loop do processo de aplicacao
    while (PC < MAX)
    {
        PC++;
        processos[indice].pc = PC;

        usleep(500000);

        // Sorteio de 15% de probabilidade de disparar uma syscall
        int d = rand() % 100 + 1;
        if (d < 15)
        {
            if (d % 2 != 0) // d impar: leitura (recv)
            {
                chamar_syscall(LEITURA, &N);
            }
            else            // d par: escrita (send)
            {
                chamar_syscall(ESCRITA, &N);
            }
        }

        usleep(500000);
    }
    
    shmdt(processos);
    return 0;
}