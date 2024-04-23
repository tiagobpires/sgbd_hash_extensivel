## TODO

- [x] Leitura do Arquivo CSV: Implemente uma função para ler os dados do arquivo compras.csv. Use a biblioteca <fstream> para abrir e ler o arquivo linha por linha.

- [x] Estrutura de Dados: Crie uma estrutura ou classe para representar uma compra, contendo atributos como Pedido, valor e ano.

- [x] Implementação do Índice Hash Extensível

  - [x] Diretório: Crie uma estrutura para o diretório que mantém uma lista de ponteiros para os buckets.

  - [x] Bucket: Cada bucket pode conter até três registros de compras.

  - [x] Hash Function: Implemente uma função hash que considere o ano da compra e gere índices para o diretório.

- [x] Inserção: Adicione registros nos buckets apropriados. Se um bucket estiver cheio, implemente a lógica para dividir o bucket e duplicar o diretório, se necessário.

- [x] Remoção: Implemente a função para remover entradas. Ajuste o diretório e os buckets conforme necessário.

- [x] Busca: Implemente a busca por ano, retornando todas as compras realizadas em um determinado ano.

- [x] Leitura e Execução de Operações: Leitura do arquivo in.txt que contém operações como INC, REM, BUS=.

- [x] Gravação de Resultados: Após cada operação, escreva o resultado no arquivo out.txt, incluindo detalhes como profundidade global e local após inserções e remoções.

- [ ] Desalocar memória de `bucket->registros` após acessá-la
- [ ] Desalocar `compras` após acessá-las (todo incremento, acessar o arquivos "compras.csv", ler as compras e depois desalocar as memórias)
- [ ] Fazer passo a passo na mão para verificar solução (cada ano está no bucket que deveria estar?)
- [ ] Adicionar todas as compras de determinado ano na operação "INC"
