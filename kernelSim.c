#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int pid1, pid2, pid3, pid4, pid5, pid6, pid7;
    printf("Inicializando kernelSim...\n");

    int fdA1_A2[2], fdA2_A1[2];
    pipe(fdA1_A2);
    pipe(fdA2_A1); // Pipe bidirecional entre A1 e A1 e vice versa

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Erro ao criar a aplicacao A1");
        exit(1);
    }
    else if ( pid1 == 0) // Entrando no processo filho
    {
        // Processo filho para A1, e por ai vai para os outros também!
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