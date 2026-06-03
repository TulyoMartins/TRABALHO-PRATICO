# TRABALHO-PRATICO
**Nome: Marcos tulyo oliveira martins
**Matrícula:** 20250045394

## Problema Escolhido
**P1 - Multiplicação de matrizes grandes**
A implementação realiza a multiplicação de duas matrizes ($A \times B = C$) alocadas como vetores unidimensionais por eficiência de cache. A paralelização foi feita fatiando as matrizes horizontalmente; ou seja, o laço mais externo foi dividido entre as threads, cada uma calculando e preenchendo as linhas de `C` correspondentes à sua fatia.

## Como Compilar e Executar

**Via CMake (Recomendado - Terminal Linux/WSL):**
``bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/meu_exe 4


// resultados//

./build/meu_exe 1
Iniciando medições para N = 1000, Threads = 1
Tempo Sequencial (Media): 0.9210 s
Tempo Paralelo (Media):   1.3274 s
Verificacao:              OK
Speedup obtido:           0.69x


./build/meu_exe 2
Iniciando medições para N = 1000, Threads = 2
Tempo Sequencial (Media): 0.9054 s
Tempo Paralelo (Media):   0.7925 s
Verificacao:              OK
Speedup obtido:           1.14x


./build/meu_exe 8
Iniciando medições para N = 1000, Threads = 8
Tempo Sequencial (Media): 0.9353 s
Tempo Paralelo (Media):   0.7898 s
Verificacao:              OK
Speedup obtido:           1.18x    . como incrementrar dna readm

## Ambiente de Teste
SO: Ubuntu 22.04 LTS (WSL2) no Windows 11

Processador (CPU): 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz

Núcleos Físicos: 4 Cores

Núcleos Lógicos: 8 Threads de Hardware (CPUs)

Compilador: GCC (Ubuntu 11.x ou superior)

Flags de Compilação: -O2 -Wall -Wextra -pthread -g (Configuradas via CMakeLists.txt)

Notas de I/O de Disco: Os testes foram executados sobre o diretório mapeado /mnt/c/, introduzindo uma camada adicional de overhead no carregamento inicial devido à tradução do sistema de arquivos inter-operacional entre o WSL2 e o Windows NT File System (NTFS).

## Resultados de Escalabilidade
 Os tempos reportados abaixo desconsideram a etapa de alocação de memória e inicialização das estruturas (setup), calculando a média estrita de 5 execuções após o descarte da primeira execução (aquecimento).
Threads 1 (seq), 1 (par), 2, 4, 8
Tempo Médio (s)  0.9205s, 1.3274 s, 0.7925 s, 0.6201 s, 0.7898 s
Speedup-Obtido  1.00x, 0.69x, 1.14x, 1.48x, 1.18x
Eficiência 100% , 69%, 57%, 37.%, 14.7%

Os dados experimentais coletados no processador Intel i5-1135G7 demonstram de forma clara que o ganho de desempenho em ambientes concorrentes não é linear em relação ao número de threads alocadas, sofrendo o impacto de múltiplos fatores de arquitetura de computadores:Overhead de Criação de Threads: Ao executar o algoritmo paralelo com apenas $T=1$, observou-se uma perda severa de desempenho (speedup de $0.69x$, sendo mais lento que o sequencial). Isso comprova empiricamente o custo computacional associado às chamadas de sistema pthread_create e pthread_join. O tempo gasto pelo Kernel do Sistema Operacional para instanciar e gerenciar as estruturas de dados da thread supera o beneficio do algoritmo paralelo em cargas de trabalho de curta duração.Escalabilidade Ideal nos Núcleos Físicos: O ponto máximo de desempenho do programa ocorreu com $T=4$ threads, alcançando um tempo de 0.6201 s e speedup de 1.48x. Como a CPU possui exatamente 4 núcleos físicos reais, este cenário permitiu que cada thread fosse agendada em um núcleo independente, maximizando o paralelismo real de computação sem concorrência interna por recursos de execução.Saturação por Hiper-Threading e Barramento (Memory Bound): Ao elevar a concorrência para $T=8$ threads, o tempo de execução voltou a subir para 0.7898 s (reduzindo o speedup para 1.18x). Dois fatores justificam esse comportamento: em primeiro lugar, os 8 núcleos lógicos da CPU são baseados em compartilhamento de recursos físicos (2 threads por núcleo); logo, threads irmãs passaram a disputar as mesmas unidades lógicas e memória cache. Em segundo lugar, a multiplicação de matrizes grandes realiza leituras massivas na memória RAM; com 8 threads ativas, ocorre a saturação da largura de banda do barramento de memória (gargalo de Memory Bound), forçando os núcleos a entrarem em estado de espera (stalls) por dados.

