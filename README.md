## TODO

[X] Leitura do Arquivo CSV: Implemente uma função para ler os dados do arquivo compras.csv. Use a biblioteca <fstream> para abrir e ler o arquivo linha por linha.

[X] Estrutura de Dados: Crie uma estrutura ou classe para representar uma compra, contendo atributos como Pedido, valor e ano.

[ ] Implementação do Índice Hash Extensível

    - [ ] Diretório: Crie uma estrutura para o diretório que mantém uma lista de ponteiros para os buckets.

    - [ ] Bucket: Cada bucket pode conter até três registros de compras.

    - [ ] Hash Function: Implemente uma função hash que considere o ano da compra e gere índices para o diretório.

[ ] Inserção: Adicione registros nos buckets apropriados. Se um bucket estiver cheio, implemente a lógica para dividir o bucket e duplicar o diretório, se necessário.

[ ] Remoção: Implemente a função para remover entradas. Ajuste o diretório e os buckets conforme necessário.

[ ] Busca: Implemente a busca por ano, retornando todas as compras realizadas em um determinado ano.

[ ] Leitura e Execução de Operações: Leitura do arquivo in.txt que contém operações como INC, REM, BUS=.

[ ] Gravação de Resultados: Após cada operação, escreva o resultado no arquivo out.txt, incluindo detalhes como profundidade global e local após inserções e remoções.
