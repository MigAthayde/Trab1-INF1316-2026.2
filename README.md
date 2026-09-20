# Trab1-INF1316-2026.2

# Simulador de Escalonamento Preemptivo e IPC (Pipes)

Este repositório contém a implementação do Primeiro Trabalho da disciplina de Sistemas de Computação (INF 1316). O projeto consiste em um simulador de um kernel de sistema operacional capaz de gerenciar o escalonamento preemptivo de processos e a comunicação inter-processos (IPC) utilizando sinais e pipes.

## 💻 Sobre o Projeto

O sistema foi desenvolvido em **C** para ambientes Unix/Linux, fazendo uso intensivo das primitivas `fork/exec`, sinais (`SIGSTOP`, `SIGCONT`) e mecanismos de IPC. 

O simulador é composto por três componentes principais que rodam concorrentemente:

1. **KernelSim**: Atua como o núcleo do sistema operacional. Ele gerencia o escalonamento Round-Robin de 6 processos de aplicação e lida com chamadas de sistema (syscalls) para leitura e escrita em pipes bidirecionais virtuais.
2. **InterController Sim**: Emula um controlador de interrupções. Ele gera eventos assíncronos (interrupções) baseados em tempo e probabilidade:
   * `IRQ0`: Fim da fatia de tempo (Time-slice).
   * `IRQ1`: Fim de uma operação de leitura em pipe.
   * `IRQ2`: Fim de uma operação de escrita em pipe.
3. **Processos de Aplicação (A1 a A6)**: Processos clientes que executam um loop de instruções (Controlados por um Program Counter - PC) e eventualmente solicitam comunicação com seus "processos vizinhos" (A1-A2, A3-A4, A5-A6) através das syscalls simuladas de `send()` e `recv()`.

## ⚙️ Principais Funcionalidades

* **Escalonamento por Time-sharing**: Chaveamento de contexto entre os processos através dos sinais `SIGSTOP` e `SIGCONT`.
* **Troca de Contexto**: Salvamento e restauração do estado dos processos (PC, variáveis locais, e parâmetros de syscalls) ao serem interrompidos.
* **Comunicação Bidirecional**: Simulação de pipes onde os processos trocam dados (seus PCs) com bloqueio temporário até a resolução via interrupção.
* **Monitoramento de Estado**: Ao enviar o sinal de parada (`Ctrl+Z` no processo simulador), o sistema exibe um relatório completo contendo:
  * Valor do PC e da variável recebida (N).
  * Estado atual do processo (Pronto, Bloqueado ou Executando).
  * Detalhes do bloqueio (operação e dispositivo).
  * Contagem de acessos aos dispositivos.

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C
* **APIs de SO:** POSIX (fork, exec, signals, pipes/shmem/fifo)

## 🚀 Como Executar

*(Adicione aqui as instruções específicas de compilação do seu projeto. Exemplo abaixo:)*

```bash
# Clone o repositório
git clone https://github.com/seu-usuario/nome-do-repo.git

# Entre na pasta
cd nome-do-repo

# Compile o código (exemplo genérico)
gcc -o simulador main.c

# Execute o simulador
./simulador
```
