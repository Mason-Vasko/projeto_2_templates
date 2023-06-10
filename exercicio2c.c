#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>

// Definição das variaveis que controlam a medição de tempo
clock_t _ini, _fim;

// Definição do tipo booleano
typedef unsigned char bool;
#define TRUE  1
#define FALSE 0

// Definição do tipo string
typedef char * string;

#define MAX_STRING_LEN 20

#define TAM_INICIAL 150001

// definicoes da hashtable ilimitada
// nó da lista linkada
typedef struct Node {
    int chave;
    int valor;
    struct Node* prox;
} Node;

typedef struct {
    Node** tabela;
} TabelaHashIlim;

unsigned converter(string s) {
   unsigned h = 0;
   for (int i = 0; s[i] != '\0'; i++) 
      h = h * 256 + s[i];
   return h;
}

string* ler_strings(const char * arquivo, const int n)
{
    FILE* f = fopen(arquivo, "r");
    
    string* strings = (string *) malloc(sizeof(string) * n);

    for (int i = 0; !feof(f); i++) {
        strings[i] = (string) malloc(sizeof(char) * MAX_STRING_LEN);
        fscanf(f, "%s\n", strings[i]);
    }

    fclose(f);

    return strings;
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

unsigned h_div(unsigned x, unsigned B)
{
    return x % B;
}

unsigned h_mul(unsigned x, unsigned B)
{
    const double A = 0.6180;
    return fmod(x * A, 1) * B;
}

TabelaHashIlim* criaTabela() {
    TabelaHashIlim* tabelaHash = (TabelaHashIlim*)malloc(sizeof(TabelaHashIlim));
    tabelaHash->tabela = (Node**)calloc(TAM_INICIAL, sizeof(Node*));
    return tabelaHash;
}

void excluiTabela(TabelaHashIlim* tabelaHash) {
    if (tabelaHash == NULL) return;

    for (int i = 0; i < TAM_INICIAL; i++) {
        Node* elemAtual = tabelaHash->tabela[i];

        while (elemAtual != NULL) {
            Node* proxElem = elemAtual->prox;
            free(elemAtual);
            elemAtual = proxElem;
        }
    }

    free(tabelaHash->tabela);
    free(tabelaHash);
}

int busca(TabelaHashIlim* tabelaHash, int chave, unsigned (*funcaoHash)(unsigned, unsigned)) {
    unsigned idx = funcaoHash(chave, TAM_INICIAL);
    Node* elemAtual = tabelaHash->tabela[idx];

    while (elemAtual != NULL) {
        if (elemAtual->chave == chave)
            return elemAtual->valor;
        
        elemAtual = elemAtual->prox;
    }

    return -1;
}

int insere(TabelaHashIlim* tabelaHash, int chave, int valor, unsigned (*funcaoHash)(unsigned, unsigned)){
    unsigned idx = funcaoHash(chave, TAM_INICIAL);
    Node* novoElem = (Node*)malloc(sizeof(Node));
    novoElem->chave = chave;
    novoElem->valor = valor;
    novoElem->prox = NULL;

    int colisoes = 0;
    // verifica colisoes
    if (tabelaHash->tabela[idx] == NULL) {
        tabelaHash->tabela[idx] = novoElem;
    } else {
        Node* elemAtual = tabelaHash->tabela[idx];
        
        while (elemAtual->prox != NULL) {
            elemAtual = elemAtual->prox;
        }
        
        elemAtual->prox = novoElem;
        colisoes++;
    }

    return colisoes;
}



int main(int argc, char const *argv[])
{
    const int N = 50000;
    const int M = 70000;
    const int B = 150001;

    unsigned colisoes_h_div = 0;
    unsigned colisoes_h_mul = 0;

    unsigned encontrados_h_div = 0;
    unsigned encontrados_h_mul = 0;

    string* insercoes = ler_strings("strings_entrada.txt", N);
    string* consultas = ler_strings("strings_busca.txt", M);
    

    // cria tabela hash com hash por divisão
    TabelaHashIlim* tabelaHash_div = criaTabela();

    // inserção dos dados na tabela hash com hash por divisão
    inicia_tempo();
    for (int i = 0; i < N; i++) {
        // inserir insercoes[i] na tabela hash
        colisoes_h_div += insere(tabelaHash_div, converter(insercoes[i]), i, h_div);
    }
    double tempo_insercao_h_div = finaliza_tempo();

    int resultadoBusca;
    // busca dos dados na tabela hash com hash por divisão
    inicia_tempo();
    for (int i = 0; i < M; i++) {
        // buscar consultas[i] na tabela hash
        resultadoBusca = busca(tabelaHash_div, converter(consultas[i]), h_div);
        if (resultadoBusca != -1)
            encontrados_h_div++;
    }
    double tempo_busca_h_div = finaliza_tempo();

    // destroi tabela hash com hash por divisão
    excluiTabela(tabelaHash_div);



    // cria tabela hash com hash por multiplicação
    TabelaHashIlim* tabelaHash_mul = criaTabela();

    // inserção dos dados na tabela hash com hash por multiplicação
    inicia_tempo();
    for (int i = 0; i < N; i++) {
        // inserir insercoes[i] na tabela hash
        colisoes_h_mul += insere(tabelaHash_mul, converter(insercoes[i]), i, h_mul);
    }
    double tempo_insercao_h_mul = finaliza_tempo();

    resultadoBusca = -1;
    // busca dos dados na tabela hash com hash por multiplicação
    inicia_tempo();
    for (int i = 0; i < M; i++) {
        // buscar consultas[i] na tabela hash
        resultadoBusca = busca(tabelaHash_mul, converter(consultas[i]), h_mul);
        if (resultadoBusca != -1)
            encontrados_h_mul++;
    }
    double tempo_busca_h_mul = finaliza_tempo();

    // destroi tabela hash com hash por multiplicação
    excluiTabela(tabelaHash_mul);



    printf("Hash por Divisão\n");
    printf("Colisões na inserção: %d\n", colisoes_h_div);
    printf("Tempo de inserção   : %fs\n", tempo_insercao_h_div);
    printf("Tempo de busca      : %fs\n", tempo_busca_h_div);
    printf("Itens encontrados   : %d\n", encontrados_h_div);
    printf("\n");
    printf("Hash por Multiplicação\n");
    printf("Colisões na inserção: %d\n", colisoes_h_mul);
    printf("Tempo de inserção   : %fs\n", tempo_insercao_h_mul);
    printf("Tempo de busca      : %fs\n", tempo_busca_h_mul);
    printf("Itens encontrados   : %d\n", encontrados_h_mul);

    return 0;
}
