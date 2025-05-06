#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_SENSORES 6

const char *sensores[MAX_SENSORES] = {
    "temperatura", "umidade", "luminosidade", "ruido", "eco2", "etvoc"
};

typedef struct {
    char ano_mes[8];  // yyyy-mm
    double min[MAX_SENSORES];
    double max[MAX_SENSORES];
    double soma[MAX_SENSORES];
    int contagem[MAX_SENSORES];
} MesStats;

typedef struct {
    char *filename;
    char device[64];
} ThreadArgs;

typedef struct MesNode {
    MesStats stats;
    struct MesNode *next;
} MesNode;

MesNode* inserir_ou_buscar(MesNode **head, const char *ano_mes) {
    MesNode *current = *head;
    while (current) {
        if (strcmp(current->stats.ano_mes, ano_mes) == 0)
            return current;
        current = current->next;
    }

    MesNode *novo = (MesNode *)malloc(sizeof(MesNode));
    strcpy(novo->stats.ano_mes, ano_mes);
    for (int i = 0; i < MAX_SENSORES; i++) {
        novo->stats.min[i] = 1e9;
        novo->stats.max[i] = -1e9;
        novo->stats.soma[i] = 0;
        novo->stats.contagem[i] = 0;
    }
    novo->next = *head;
    *head = novo;
    return novo;
}

int tem_valores_validos(char **tokens) {
    for (int i = 4; i <= 9; i++) {
        if (tokens[i] == NULL || strlen(tokens[i]) == 0 || strcmp(tokens[i], "\n") == 0)
            return 0;
    }
    return 1;
}

void* processar_arquivo(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    FILE *f = fopen(args->filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return NULL;
    }

    char linha[MAX_LINE];
    fgets(linha, sizeof(linha), f); // pula cabeçalho

    MesNode *lista = NULL;

    while (fgets(linha, sizeof(linha), f)) {
        char *tokens[12] = {0};
        char *tok = strtok(linha, ",");
        for (int i = 0; tok && i < 12; i++) {
            tokens[i] = tok;
            tok = strtok(NULL, ",");
        }

        if (!tem_valores_validos(tokens)) continue;

        char *data_str = tokens[3];
        char ano_mes[8];
        strncpy(ano_mes, data_str, 7);
        ano_mes[7] = '\0';

        MesNode *mes = inserir_ou_buscar(&lista, ano_mes);

        for (int i = 0; i < MAX_SENSORES; i++) {
            double val = atof(tokens[4 + i]);
            if (val < mes->stats.min[i]) mes->stats.min[i] = val;
            if (val > mes->stats.max[i]) mes->stats.max[i] = val;
            mes->stats.soma[i] += val;
            mes->stats.contagem[i]++;
        }
    }
    fclose(f);

    // escreve resultado
    char out_nome[128];
    sprintf(out_nome, "saida_%s.csv", args->device);
    FILE *out = fopen(out_nome, "w");
    if (!out) {
        perror("Erro ao criar saída");
        return NULL;
    }

    fprintf(out, "device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo\n");

    for (MesNode *ptr = lista; ptr != NULL; ptr = ptr->next) {
        for (int i = 0; i < MAX_SENSORES; i++) {
            if (ptr->stats.contagem[i] == 0) continue;
            double media = ptr->stats.soma[i] / ptr->stats.contagem[i];
            fprintf(out, "%s;%s;%s;%.2f;%.2f;%.2f\n",
                args->device,
                ptr->stats.ano_mes,
                sensores[i],
                ptr->stats.max[i],
                media,
                ptr->stats.min[i]
            );
        }
    }

    fclose(out);

    // limpeza
    while (lista) {
        MesNode *tmp = lista;
        lista = lista->next;
        free(tmp);
    }

    return NULL;
}

void mesclarCSVs() {
    char nomeEntrada[100];
    const char *nomeSaida = "saida_sirrosteste_UCS_AMV-FINAL.csv";
    FILE *saida = fopen(nomeSaida, "w");

    if (!saida) {
        perror("Erro ao criar o arquivo de saída");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= 23; i++) {
        // Gera o nome do arquivo de entrada
        snprintf(nomeEntrada, sizeof(nomeEntrada), "saida_sirrosteste_UCS_AMV-%02d.csv", i);
        FILE *entrada = fopen(nomeEntrada, "r");

        if (!entrada) {
            fprintf(stderr, "Aviso: não foi possível abrir %s. Pulando...\n", nomeEntrada);
            continue;
        }

        char buffer[1024];
        int primeiraLinha = 1;

        while (fgets(buffer, sizeof(buffer), entrada)) {
            // Escreve cabeçalho apenas do primeiro arquivo
            if (i > 1 && primeiraLinha) {
                primeiraLinha = 0;
                continue; // pula o cabeçalho dos outros arquivos
            }
            fputs(buffer, saida);
            primeiraLinha = 0;
        }

        fclose(entrada);
    }

    fclose(saida);
    printf("Arquivo final criado com sucesso: %s\n", nomeSaida);
}

int main() {
    const int N = 23;
    pthread_t threads[N];
    ThreadArgs args[N];

    for (int i = 0; i < N; i++) {
        char *nome = (char *)malloc(64);
        sprintf(nome, "device_sirrosteste_UCS_AMV-%02d.csv", i + 1);
        args[i].filename = nome;
        sprintf(args[i].device, "sirrosteste_UCS_AMV-%02d", i + 1);
        pthread_create(&threads[i], NULL, processar_arquivo, &args[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        free(args[i].filename);
    }

    mesclarCSVs();

    return 0;
}