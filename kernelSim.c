#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

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

int main(void)
{
    int pid1, pid2, pid3, pid4, pid5, pid6, pid7;
    Processo processos[6]; // Uma ficha para cada processo. É o jeito do KernelSim ter informação sobre cada um
    printf("Inicializando kernelSim...\n");

    // Vou criar a memória compartilada para os processos e kernelSim.

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Erro ao criar a aplicacao A1");
        exit(1);
    }
    else if ( pid1 == 0) // Entrando no processo filho
    {
        // Processo filho para A1
        exit(0);
    }

    pid2 = fork();
    if(pid2 < 0)
    {
        perror("Erro ao criar aplicacao A2");
        exit(1);
    }
    else if(pid2 == 0)
    {
        // Processo filho para A2
        exit(0);
    }

    pid3 = fork();
    if(pid3 < 0)
    {
        perror("Erro ao criar aplicacao A3");
        exit(1);
    }
    else if(pid3 == 0)
    {
        // Processo filho para A3
        exit(0);
    }

    pid4 = fork();
    if(pid4 < 0)
    {
        perror("Erro ao criar aplicacao A4");
        exit(1);
    }
    else if(pid4 == 0)
    {
        // Processo filho para A4
        exit(0);
    }

    pid5 = fork();
    if(pid5 < 0)
    {
        perror("Erro ao criar aplicacao A5");
        exit(1);
    }
    else if(pid5 == 0)
    {
        // Processo filho para A5
        exit(0);
    }

    pid6 = fork();
    if(pid6 < 0)
    {
        perror("Erro ao criar aplicacao A6");
        exit(1);
    }
    else if(pid6 == 0)
    {
        // Processo filho para A6
        exit(0);
    }

    pid7 = fork();
    if(pid7 < 0)
    {
        perror("Erro ao criar InterControllerSim");
        exit(1);
    }
    else if(pid7 == 0)
    {
        // Processo filho para InterControllerSim
        exit(0);
    }

    return 0;
}