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

// definicoes da hashtable limitada
typedef struct {
    int chave;
    int valor;
} Par;

typedef struct {
    int tamanho;
    Par **tabela;
} tabelaHashLim;

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


unsigned h_div(unsigned x, unsigned i, unsigned B)
{
    return ((x % B) + i) % B;
}

unsigned h_mul(unsigned x, unsigned i, unsigned B)
{
    const double A = 0.6180;
    return  ((int) ((fmod(x * A, 1) * B) + i)) % B;
}

// definindo funcao hash e rehash
unsigned hash(tabelaHashLim* tabelaHash, int chave, unsigned i) {
    const int B = 150001;
    int h_mul_val = h_mul(chave, i, B);
    int h_div_val = h_div(chave, i, B);

    return (h_mul_val + i * h_div_val) % tabelaHash->tamanho;
}

unsigned rehash(tabelaHashLim* tabelaHash, int chave, unsigned i) {
    return (hash(tabelaHash, chave, i) + i) % tabelaHash->tamanho;
}

// funcoes da tabela
tabelaHashLim* criaTabela(int tamanho) {
    tabelaHashLim* tabelaHash = (tabelaHashLim*)malloc(sizeof(tabelaHashLim));
    tabelaHash->tamanho = tamanho;
    tabelaHash->tabela = (Par**)calloc(tamanho, sizeof(Par*));

    return tabelaHash;
}
void insere(tabelaHashLim* tabelaHash, int chave, int valor) {
    int i = 0;
    int valorHash = hash(tabelaHash, chave, i);

    while (tabelaHash->tabela[valorHash] != NULL)
        valorHash = rehash(tabelaHash, chave, i++);
    
    Par* par = (Par*)malloc(sizeof(Par));
    par->chave = chave;
    par->valor = valor;
    tabelaHash->tabela[valorHash] = par;
}

int busca(tabelaHashLim* tabelaHash, int chave) {
    int i = 0;
    int valorHash = hash(tabelaHash, chave, i);

    while (tabelaHash->tabela[valorHash] != NULL) {
        if (tabelaHash->tabela[valorHash]->chave == chave)
            return tabelaHash->tabela[valorHash]->valor;
    }
    valorHash = rehash(tabelaHash, chave, i++);

    return -1;
}

void excluiTabHash(tabelaHashLim* tabelaHash) {
    if (tabelaHash == NULL) 
        return;
    
    for (int i = 0; i < tabelaHash->tamanho; i++) 
        if (tabelaHash->tabela[i] != NULL)
            free(tabelaHash->tabela[i]);
    
    free(tabelaHash->tabela);
    free(tabelaHash);
}

int main(int argc, char const *argv[])
{
    const int N = 50000;
    const int M = 70000;
    const int B = 150001;

    unsigned colisoes = 0;
    unsigned encontrados = 0;

    string* insercoes = ler_strings("strings_entrada.txt", N);
    string* consultas = ler_strings("strings_busca.txt", M);


    // cria tabela hash com hash por hash duplo
    tabelaHashLim* tabelaHash = criaTabela(B);

    // inserção dos dados na tabela hash
    inicia_tempo();
    for (int i = 0; i < N; i++) {
        // inserir insercoes[i] na tabela hash
        insere(tabelaHash, converter(insercoes[i]), i);
    }
    double tempo_insercao = finaliza_tempo();

    int resultadoBusca;
    // busca dos dados na tabela hash
    inicia_tempo();
    for (int i = 0; i < M; i++) {
        // buscar consultas[i] na tabela hash
        resultadoBusca = busca(tabelaHash, converter(consultas[i]));
    }
    double tempo_busca = finaliza_tempo();

    excluiTabHash(tabelaHash);

    printf("Colisões na inserção: %d\n", colisoes);
    printf("Tempo de inserção   : %fs\n", tempo_insercao);
    printf("Tempo de busca      : %fs\n", tempo_busca);
    printf("Itens encontrados   : %d\n", encontrados);

    return 0;
}
