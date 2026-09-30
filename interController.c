#include <stdlib.h>
#include <unistd.h>

#define P1 30
#define P2 20

int main(void)
{
    int irq;
    srand(getpid()); // Pra deixar aleatório

    while (1)
    {
        usleep(500000); // Time-slice de 500ms

        // IRQ0: Fim do time-slice
        irq = 0;
        write(1, &irq, sizeof(int));

        // IRQ1: Fim de operacao de leitura em pipe (probabilidade P1)
        if (rand() % 100 < P1)
        {
            irq = 1;
            write(1, &irq, sizeof(int));
        }

        // IRQ2: Fim de operacao de escrita em pipe (probabilidade P2)
        if (rand() % 100 < P2)
        {
            irq = 2;
            write(1, &irq, sizeof(int));
        }
    }

    return 0;
}
