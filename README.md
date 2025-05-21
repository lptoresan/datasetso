# 📊 Processamento Paralelo de Arquivos CSV com Threads em C

## 🎯 Objetivo da Aplicação

Esta aplicação em **C** realiza o **processamento paralelo de 23 arquivos CSV**, cada um contendo dados de sensores ambientais como **temperatura**, **umidade**, **luminosidade**, entre outros.
Os arquivos .csv foram extraídos de um base de dados de diversos dispositivos, onde buscou todos "devices" com o nome sirroteste para separação, resultando em 23 arquivos de saída. Um para cada device. 

Utilizando **threads (pthread)**, o programa:

- Lê e processa os arquivos simultaneamente.
- Agrupa os dados por **ano-mês** e por **sensor**.
- Gera arquivos com **estatísticas mensais** (mínimo, máximo e média).
- Consolida todos os resultados em um **relatório final único**.

---

## ⚙️ Funcionamento Geral

1. **Leitura paralela** de arquivos CSV utilizando pthreads.
2. Para cada arquivo:
   - Extração de dados por **sensor** e **mês**.
   - Cálculo de **mínimo**, **máximo** e **média**.
   - Geração de um arquivo de saída com as estatísticas.
3. Consolidação final:
   - Um método une todos os arquivos CSV gerados em um **único relatório final**.

---

## 🧵 Arquitetura com Threads

- O programa cria **23 threads**, uma para cada arquivo.
- Cada thread executa a função `processar_arquivo`, que:
  - Lê os dados do CSV.
  - Armazena estatísticas em uma **lista encadeada** (`MesNode`).
  - Gera um arquivo de saída individual.
- A função `mesclarCSVs` junta todos os resultados em um **arquivo CSV final**.

---

## 📄 Explicação das Principais Funções

### `main()`

- Cria 23 threads, cada uma processando um arquivo:  
  `device_sirrosteste_UCS_AMV-XX.csv`
- Aguarda a finalização de todas as threads com `pthread_join`.
- Chama `mesclarCSVs()` para gerar o relatório final.

### `processar_arquivo(void *arg)`

- Abre o CSV recebido como argumento.
- Lê linha por linha (ignora o cabeçalho).
- Divide as linhas em colunas (tokens).
- Extrai a data para agrupar por `ano-mês`.
- Usa `inserir_ou_buscar` para manter/atualizar estatísticas.
- Atualiza valores mínimo, máximo, soma e contagem por sensor.
- Gera arquivo de saída com as estatísticas mensais.

### `inserir_ou_buscar()`

- Verifica se já existe uma entrada para o `ano-mês`.
- Se não, cria um novo nó com os dados inicializados.

### `tem_valores_validos()`

- Valida se os campos de sensores possuem valores numéricos válidos (`tokens[4]` a `tokens[9]`).

### `mesclarCSVs()`

- Abre os 23 arquivos individuais gerados por cada thread.
- Copia os dados para um único arquivo `saida_sirrosteste_UCS_AMV-FINAL.csv`.
- Mantém o cabeçalho apenas do primeiro arquivo.

---

## 🧾 Exemplo de Saída
device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo

sirrosteste_UCS_AMV-01;2023-01;temperatura;30.50;25.13;18.20

sirrosteste_UCS_AMV-01;2023-01;umidade;76.00;60.55;45.00
...

## 🛠️ Compilação e Execução

```bash
gcc -pthread -o trabalho trabalho.c
./trabalho.exe


