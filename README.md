Objetivo da Aplicação

Esta aplicação em C realiza o processamento paralelo de 23 arquivos CSV, cada um contendo dados de sensores ambientais como temperatura, umidade, luminosidade, entre outros. Utilizando threads, o programa lê e processa os arquivos simultaneamente, agrupando os dados por mês (ano-mês) e sensor. Ao final, cada thread gera um arquivo de saída com estatísticas mensais, e o programa mescla todos esses arquivos em um único relatório final.

Funcionamento Geral:

1. Leitura paralela de arquivos CSV usando pthread.
2. Para cada arquivo:
   - Extrai dados por sensor e mês.
   - Calcula mínimo, máximo e média.
   - Gera um CSV com esses dados.
3. Ao final, um método une os CSVs gerados por cada thread em um único arquivo.
Arquitetura com Threads
• O programa usa 23 threads, uma para cada arquivo.
• Cada thread executa processar_arquivo, que:
  - Lê os dados do arquivo.
  - Armazena estatísticas em uma lista encadeada (MesNode).
  - Escreve um arquivo de saída.
• A função mesclarCSVs junta os 23 arquivos em um único arquivo final.

Explicação do Código

main()

• Cria 23 threads, cada uma responsável por um arquivo nomeado device_sirrosteste_UCS_AMV-XX.csv.
• Passa para cada thread o nome do arquivo e o identificador do dispositivo.
• Após a execução das threads, chama mesclarCSVs para juntar os resultados.

processar_arquivo(void *arg)

• Abre o arquivo CSV passado via arg.
• Lê linha por linha, ignorando a primeira (cabeçalho).
• Divide cada linha em tokens (colunas separadas por vírgula).
• Extrai o campo de data.
• Usa inserir_ou_buscar para manter ou criar uma entrada de estatísticas por mês.
• Para cada sensor atualiza os valores mínimo, máximo, soma e contagem.
• Ao fim do processamento, escreve os dados no arquivo saida.

inserir_ou_buscar()

• Verifica se já existe um nó para o mês (ano_mes) na lista.
• Se não, cria um novo nó com estatísticas inicializadas.

tem_valores_validos()

• Verifica se todos os campos de sensores (tokens[4] a tokens[9]) possuem dados válidos.
mesclarCSVs()
• Abre os 23 arquivos saida_sirrosteste_UCS_AMV-XX.csv.
• Copia os dados de todos para um único arquivo saida_sirrosteste_UCS_AMV-FINAL.csv.
• Mantém o cabeçalho apenas do primeiro arquivo.

Compilação e Execução

gcc -pthread -o trabalho trabalho.c

./trabalho.exe

Exemplo de Saída

device;ano-mes;sensor;valor_maximo;valor_medio;valor_minimo

sirrosteste_UCS_AMV-01;2023-01;temperatura;30.50;25.13;18.20

sirrosteste_UCS_AMV-01;2023-01;umidade;76.00;60.55;45.00

...


Limpeza de Recursos

- Todos os arquivos são fechados corretamente.
- A memória alocada dinamicamente (linhas, listas, nomes de arquivos) é liberada.
- Threads são sincronizadas com pthread_join.
