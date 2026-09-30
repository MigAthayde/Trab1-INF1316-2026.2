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

// VARIÁVEIS GLOBAIS!!!!!
Processo *processos;
int atual = -1;
//////////////////////////

int escolherProximoPronto(Processo *processos, int atual)
{
    int inicio = (atual == -1) ? 0 : (atual + 1) % 6;
    for (int i = 0; i < 6; i++)
    {
        int idx = (inicio + i) % 6;
        if (processos[idx].estado == PRONTO)
        {
            return idx;
        }
    }
    return -1;
}

void verificarTerminados(Processo *processos, int *atual)
{
    int p;
    while ((p = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        for (int i = 0; i < 6; i++)
        {
            if (processos[i].pid == p)
            {
                processos[i].estado = TERMINADO;
                printf("[Kernel] A%d terminou (pc=%d)\n", i + 1, processos[i].pc);
                if (*atual == i)
                {
                    *atual = -1;
                }
            }
        }
    }
}


int verificarTodosTerminaram(Processo *processos)
{
    for (int i = 0; i < 6; i++)
    {
        if (processos[i].estado != TERMINADO)
        {
            return 0;
        }
    }
    return 1;
}

// Essa função aqui vai ser usada como sigtstp handler
void tabelaProcessos(int sig)
{
    printf("\n[Kernel] Tabela de Processos (PCB):\n\n");
    printf("PID | PC | N | Estado | OpPendente | Dispositivo | AcessosLeitura | AcessosEscrita\n");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < 6; i++)
    {
        char dispositivo[20];
        if(processos[i].estado == BLOQUEADO)
        {
            snprintf(dispositivo, sizeof(dispositivo), "pipe%d", i/2 + 1);
        }
        else
        {
            snprintf(dispositivo, sizeof(dispositivo), "-");
        }
        printf("%3d | %2d | %d | %7s | %10s | %20s | %14d | %15d\n",
               processos[i].pid,
               processos[i].pc,
               processos[i].n,
               (processos[i].estado == PRONTO) ? "PRONTO" :
               (processos[i].estado == BLOQUEADO) ? "BLOQUEADO" :
               (processos[i].estado == EXECUTANDO) ? "EXECUTANDO" : "TERMINADO",
               (processos[i].opPendente == NENHUM) ? "NENHUM" :
               (processos[i].opPendente == LEITURA) ? "LEITURA" : "ESCRITA",
               dispositivo,
               processos[i].acessosLeitura,
               processos[i].acessosEscrita);
    }
    raise(SIGSTOP);
    for (int i = 0; i < 6; i++)
    {
        if (i != atual && processos[i].estado != TERMINADO)
        {
            kill(processos[i].pid, SIGSTOP);
        }
    }
}

int main(void)
{
    int pid1, pid2, pid3, pid4, pid5, pid6, pid7;

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

    processos = (Processo *)shmat(shmid, NULL, 0);
    if (processos ==  (void *)-1)
    {
        perror("Erro ao anexar memoria compartilhada com shmat");
        exit(1);
    }
    signal(SIGTSTP, tabelaProcessos);
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
    // Estruturas da Etapa 7:
    // buffer[i] guarda o ultimo PC enviado pelo processo i para o seu parceiro (i ^ 1)
    int buffer[6] = {0};

    // Filas FIFO para processos bloqueados em chamadas de sistema
    int filaLeitura[6];
    int tamLeitura = 0;

    int filaEscrita[6];
    int tamEscrita = 0;

    int msg;

    while (1)
    {
        if (read(0, &msg, sizeof(int)) <= 0)
        {
            perror("[Kernel] Erro na leitura do pipe de eventos");
            exit(1);
        }

        // 1. Atualiza quem ja terminou
        verificarTerminados(processos, &atual);

        // 2. Condicao de saida: todos os 6 processos terminaram?
        if (verificarTodosTerminaram(processos))
        {
            printf("[Kernel] Todos os processos terminaram. Encerrando kernelSim...\n");
            break;
        }

        if (msg == 0) // IRQ0: Fim da fatia de tempo (Time-slice)
        {
            printf("[Kernel] IRQ0 recebido\n");

            // Se tem alguem executando, interrompe e coloca como PRONTO
            if (atual != -1)
            {
                kill(processos[atual].pid, SIGSTOP);
                if (processos[atual].estado != TERMINADO)
                {
                    processos[atual].estado = PRONTO;
                }
            }

            // Seleciona o proximo processo PRONTO (Round-Robin)
            int proximo = escolherProximoPronto(processos, atual);
            if (proximo != -1)
            {
                atual = proximo;
                processos[atual].estado = EXECUTANDO;
                printf("[Kernel] A%d executando (pc=%d)\n", atual + 1, processos[atual].pc);
                kill(processos[atual].pid, SIGCONT);
            }
            else
            {
                atual = -1; // Nenhum processo pronto no momento
            }
        }
        else if (msg == 1) // IRQ1: Fim de operacao de leitura em pipe
        {
            printf("[Kernel] IRQ1 recebido\n");
            if (tamLeitura > 0)
            {
                // Tira o primeiro processo da fila FIFO de leitura
                int j = filaLeitura[0];
                for (int k = 0; k < tamLeitura - 1; k++)
                {
                    filaLeitura[k] = filaLeitura[k + 1];
                }
                tamLeitura--;

                // Entrega o dado do parceiro (j ^ 1)
                int parceiro = j ^ 1;
                processos[j].n = buffer[parceiro];
                buffer[parceiro] = 0; // Consumido (evita releitura; se parceiro nao escreveu, e 0 como no NO_WAIT)

                processos[j].opPendente = NENHUM;
                processos[j].estado = PRONTO;
                printf("[Kernel] A%d desbloqueado da LEITURA (recebeu N=%d de A%d, agora PRONTO)\n", 
                       j + 1, processos[j].n, parceiro + 1);
            }
        }
        else if (msg == 2) // IRQ2: Fim de operacao de escrita em pipe
        {
            printf("[Kernel] IRQ2 recebido\n");
            if (tamEscrita > 0)
            {
                // Tira o primeiro processo da fila FIFO de escrita
                int j = filaEscrita[0];
                for (int k = 0; k < tamEscrita - 1; k++)
                {
                    filaEscrita[k] = filaEscrita[k + 1];
                }
                tamEscrita--;

                // Grava o PC no buffer do canal de j
                buffer[j] = processos[j].pc;
                processos[j].opPendente = NENHUM;
                processos[j].estado = PRONTO;
                printf("[Kernel] A%d desbloqueado da ESCRITA (escreveu PC=%d no buffer, agora PRONTO)\n", 
                       j + 1, buffer[j]);
            }
        }
        else if (msg >= 10) // Syscall vinda de uma aplicacao (msg = 10 + i)
        {
            int i = msg - 10;
            if (i >= 0 && i < 6)
            {
                // Bloqueia imediatamente o processo requisitante
                kill(processos[i].pid, SIGSTOP);
                processos[i].estado = BLOQUEADO;

                if (processos[i].opPendente == LEITURA)
                {
                    processos[i].acessosLeitura++;
                    printf("[Kernel] Syscall LEITURA de A%d (BLOQUEADO na fila)\n", i + 1);
                    filaLeitura[tamLeitura++] = i;
                }
                else if (processos[i].opPendente == ESCRITA)
                {
                    processos[i].acessosEscrita++;
                    printf("[Kernel] Syscall ESCRITA de A%d (BLOQUEADO na fila, valor pc=%d)\n", i + 1, processos[i].pc);
                    filaEscrita[tamEscrita++] = i;
                }

                // Se o processo que chamou syscall era o que estava rodando na CPU, escalona o proximo
                if (i == atual)
                {
                    atual = -1;
                    int proximo = escolherProximoPronto(processos, atual);
                    if (proximo != -1)
                    {
                        atual = proximo;
                        processos[atual].estado = EXECUTANDO;
                        printf("[Kernel] A%d executando (pc=%d)\n", atual + 1, processos[atual].pc);
                        kill(processos[atual].pid, SIGCONT);
                    }
                }
            }
        }
    }

    kill(pid7, SIGKILL);
    waitpid(pid7, NULL, 0);
    // Liberar a memoria compartilhada
    shmdt(processos);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}