#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>

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
    int processoAtual = 0;
    int tentativas = 0;
    int p; // Essa aqui vai ser usada para pegar o pid do processo que terminou no loop do escalonador

    printf("Inicializando kernelSim...\n");
    int fd[2];
    if(pipe(fd) == -1)
    {
        perror("Erro ao criar pipe");
        exit(1);
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(Processo) * 6, IPC_CREAT | 0666);
    if(shmid < 0)
    {
        perror("Erro ao criar memoria compartilhada com shmget");
        exit(1);
    }

    Processo *processos = (Processo *)shmat(shmid, NULL, 0);
    if (processos ==  (void *)-1)
    {
        perror("Erro ao anexar memoria compartilhada com shmat");
        exit(1);
    }
    char shmid_str[20];
    sprintf(shmid_str, "%d", shmid);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "0", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A1");
        exit(1);
    }
    else
    {
        processos[0].pid = pid1;
        kill(pid1, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "1", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A2");
        exit(1);
    }
    else
    {
        processos[1].pid = pid2;
        kill(pid2, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "2", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A3");
        exit(1);
    }
    else
    {
        processos[2].pid = pid3;
        kill(pid3, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "3", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A4");
        exit(1);
    }
    else
    {
        processos[3].pid = pid4;
        kill(pid4, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "4", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A5");
        exit(1);
    }
    else
    {
        processos[4].pid = pid5;
        kill(pid5, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./app", "app", "5", shmid_str, NULL);
        perror("Erro ao executar a aplicacao A6");
        exit(1);
    }
    else
    {
        processos[5].pid = pid6;
        kill(pid6, SIGSTOP);
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
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execl("./interController", "interController", NULL);
        perror("Erro ao executar o InterControllerSim");
        exit(1);
    }

    // Criou todos os filhos!! 
    close(fd[1]);
    dup2(fd[0], 0);
    close(fd[0]);
    // Loop escalonador!
    while(1)
    {
        while(processos[processoAtual].estado != PRONTO && tentativas < 6)
        {
            processoAtual = (processoAtual + 1) % 6;
            tentativas++;
        }
        if (tentativas == 6)
        {
            printf("[Kernel] Todos os processos terminaram. Encerrando kernelSim...\n");
            break;
        }
        tentativas = 0;

        kill(processos[processoAtual].pid, SIGCONT);
        printf("[Kernel] A%d executando (pc=%d)\n", processoAtual + 1, processos[processoAtual].pc);
        processos[processoAtual].estado = EXECUTANDO;
        usleep(500000);
        kill(processos[processoAtual].pid, SIGSTOP);
        while((p = waitpid(-1, NULL, WNOHANG)) > 0)
        {
            for (int i = 0; i < 6; i++)
            {
                if(processos[i].pid == p)
                {
                    processos[i].estado = TERMINADO;
                    printf("[Kernel] A%d terminou (pc=%d)\n", i + 1, processos[i].pc);

                }
            }
        }
        if (processos[processoAtual].estado != TERMINADO)
        {
            processos[processoAtual].estado = PRONTO;
        }
        processoAtual = (processoAtual + 1) % 6;
    }

    kill(pid7, SIGKILL);
    waitpid(pid7, NULL, 0);
    // Liberar a memoria compartilhada
    shmdt(processos);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}