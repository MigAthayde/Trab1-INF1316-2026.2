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
    printf("Inicializando kernelSim...\n");

    // 2. Criar a memoria compartilhada ANTES de qualquer fork()
    Processo *processos = mmap(NULL, sizeof(Processo) * 6, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (processos == MAP_FAILED)
    {
        perror("Erro ao criar memoria compartilhada com mmap");
        exit(1);
    }

    // 3. Inicializar cada posicao da tabela de processos (PCB)
    for (int i = 0; i < 6; i++)
    {
        processos[i].pid = 0;
        processos[i].pc = 0;
        processos[i].n = 0;
        processos[i].estado = PRONTO;
        processos[i].opPendente = NENHUM;
        processos[i].acessosLeitura = 0;
        processos[i].acessosEscrita = 0;
    }

    // 4. Preencher o PID de cada processo logo apos o fork correspondente
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Erro ao criar a aplicacao A1");
        exit(1);
    }
    else if (pid1 == 0) // Entrando no processo filho A1
    {
        // 5. Teste para verificar se a memoria compartilhada funciona (Passo 5)
        processos[0].pc = 999;
        printf("[A1] Escrevi na memoria compartilhada: processos[0].pc = %d\n", processos[0].pc);
        exit(0);
    }
    else
    {
        processos[0].pid = pid1;
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        perror("Erro ao criar aplicacao A2");
        exit(1);
    }
    else if (pid2 == 0)
    {
        // Processo filho para A2
        exit(0);
    }
    else
    {
        processos[1].pid = pid2;
    }

    pid3 = fork();
    if (pid3 < 0)
    {
        perror("Erro ao criar aplicacao A3");
        exit(1);
    }
    else if (pid3 == 0)
    {
        // Processo filho para A3
        exit(0);
    }
    else
    {
        processos[2].pid = pid3;
    }

    pid4 = fork();
    if (pid4 < 0)
    {
        perror("Erro ao criar aplicacao A4");
        exit(1);
    }
    else if (pid4 == 0)
    {
        // Processo filho para A4
        exit(0);
    }
    else
    {
        processos[3].pid = pid4;
    }

    pid5 = fork();
    if (pid5 < 0)
    {
        perror("Erro ao criar aplicacao A5");
        exit(1);
    }
    else if (pid5 == 0)
    {
        // Processo filho para A5
        exit(0);
    }
    else
    {
        processos[4].pid = pid5;
    }

    pid6 = fork();
    if (pid6 < 0)
    {
        perror("Erro ao criar aplicacao A6");
        exit(1);
    }
    else if (pid6 == 0)
    {
        // Processo filho para A6
        exit(0);
    }
    else
    {
        processos[5].pid = pid6;
    }

    pid7 = fork();
    if (pid7 < 0)
    {
        perror("Erro ao criar InterControllerSim");
        exit(1);
    }
    else if (pid7 == 0)
    {
        // Processo filho para InterControllerSim
        exit(0);
    }

    // 5. Teste no processo pai (KernelSim)
    sleep(1);
    printf("[KernelSim] Lendo memoria compartilhada apos teste: processos[0].pc = %d\n", processos[0].pc);

    // Aguardar o termino dos processos filhos do teste
    for (int i = 0; i < 7; i++)
    {
        wait(NULL);
    }

    // Liberar a memoria compartilhada
    munmap(processos, sizeof(Processo) * 6);
    printf("Teste de memoria compartilhada finalizado com sucesso!\n");

    return 0;
}