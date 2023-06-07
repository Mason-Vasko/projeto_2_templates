#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definição das variaveis que controlam a medição de tempo
clock_t _ini, _fim;

// Definição do tipo booleano
unsigned char typedef bool;
#define TRUE  1
#define FALSE 0

int* ler_inteiros(const char * arquivo, const int n)
{
    FILE* f = fopen(arquivo, "r");

    int* inteiros = (int *) malloc(sizeof(int) * n);

    for (int i = 0; !feof(f); i++)
        fscanf(f, "%d\n", &inteiros[i]);

    fclose(f);

    return inteiros;
}

void inicia_tempo()
{
    srand(time(NULL));
    _ini = clock();
}

double finaliza_tempo()
{
    _fim = clock();
    return ((double) (_fim - _ini)) / CLOCKS_PER_SEC;
}
void inverte(int* a, int* b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}
 
int particiona(int *vetor, int inicio, int fim){
    int pivo = vetor[fim];
    int i = (inicio - 1);
 
    for (int j = inicio; j <= fim - 1; j++) {
        if (vetor[j] < pivo) {
            i++;
            inverte(&vetor[i], &vetor[j]);
        }
    }
    inverte(&vetor[i + 1], &vetor[fim]);
    return (i + 1);
}
 
void QuickSort(int *vetor, int inicio, int fim){
    if (inicio < fim) {
        int pivo = particiona(vetor, inicio, fim);
        QuickSort(vetor, inicio, pivo - 1);
        QuickSort(vetor, pivo + 1, fim);
    }
}

int main(int argc, char const *argv[])
{
    const int N = 50000;
    const int index_size = 10000;
    unsigned encontrados = 0;

    int* entradas = ler_inteiros("inteiros_entrada.txt", N);
    int* consultas = ler_inteiros("inteiros_busca.txt", N);
    
    // ordenar entrada
    QuickSort(entradas,0,N-1);
    // criar tabela de indice

    // realizar consultas na tabela de indices 
    inicia_tempo();
    for (int i = 0; i < N; i++) {
        printf("%d\n",entradas[i]);
        // buscar o elemento consultas[i] na entrada
    }
    double tempo_busca = finaliza_tempo();

    printf("Tempo de busca    :\t%fs\n", tempo_busca);
    printf("Itens encontrados :\t%d\n", encontrados);

    return 0;
}
