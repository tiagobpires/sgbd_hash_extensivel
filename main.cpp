#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <memory>
#include <algorithm>

using namespace std;

int MAXIMO_REGISTROS = 3;

class Compra {
public:
    int pedido;
    double valor;
    int ano;

    Compra(int p, double v, int a) : pedido(p), valor(v), ano(a) {}

    void exibir() const {
        cout << "Pedido: " << pedido
                  << ", Valor: R$ " << valor
                  << ", Ano: " << ano << endl;
    }
};

class LeitorCSV {
private:
    string arquivo;

public:
    LeitorCSV(const string& nomeArquivo) : arquivo(nomeArquivo) {}

    vector<Compra> lerCompras() {
        ifstream file(arquivo);
        vector<Compra> compras;
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << arquivo << endl;
            return compras; 
        }

        string linha;

        while (getline(file, linha)) {
            istringstream ss(linha);
            string valorPedido, valorValor, valorAno;

            getline(ss, valorPedido, ',');
            getline(ss, valorValor, ',');
            getline(ss, valorAno, ',');

            int pedido = stoi(valorPedido);
            double valor = stod(valorValor);
            int ano = stoi(valorAno);

            compras.push_back(Compra(pedido, valor, ano));
        }

        file.close();
        return compras;
    }
};

class Bucket {
private:
    string filename; 

public:
    vector<Compra> registros;
    int profundidadeLocal;
    int maximoRegistros;

    Bucket(string filename, int profundidadeLocal, int maximoRegistros=3) : filename(filename), profundidadeLocal(profundidadeLocal), maximoRegistros(maximoRegistros) { 
        // Gerar arquivo do bucket
        ofstream file(filename); 
        file.close();
    }

    Bucket(string filename, vector<Compra> registros, int profundidadeLocal, int maximoRegistros=3) : filename(filename), profundidadeLocal(profundidadeLocal), maximoRegistros(maximoRegistros) {
        if (registros.size() > this->maximoRegistros) {
            cout << "Máximo de registros é " << this->maximoRegistros << endl;
            cout << "Nenhum registro foi adicionado" << endl;
        } else {
            this->registros = registros;
        }
    }

    bool cheio() const {
        if (this->registros.size() >= this->maximoRegistros) {
            cout << "Armazenamento cheio. Máximo de registros é " << this->maximoRegistros << endl;
            return true;
        }
        return false;
    }

    void carregar() {
        LeitorCSV leitor(filename);
        this->registros = leitor.lerCompras();
    }

    void salvar() {
        ofstream file(filename);
        for (const auto& compra : this->registros) {
            file << compra.pedido << "," << compra.valor << "," << compra.ano << endl;
        }
        file.close();
    }

    void desalocarMemoriaRegistros() {
        this->registros = vector<Compra>();
    }

    bool adicionarRegistro(Compra compra) {
        if (!cheio()) {
            registros.push_back(compra);
            salvar(); 
            return true;
        }
        return false;
    }

    void imprimir() {
        for (Compra compra : this->registros) {
            compra.exibir();
        }
    }
};

class Diretorio {
private:
    vector<shared_ptr<Bucket>> buckets;

public:
    int profundidadeGlobal;

    Diretorio(int profundidade) : profundidadeGlobal(profundidade) {
        int tamanhoInicial = pow(2, profundidadeGlobal);
        this->buckets.resize(tamanhoInicial);

        for (int i = 0; i < tamanhoInicial; ++i) {
            string nomeArquivo = "bucket_" + to_string(i) + ".csv";
            this->buckets[i] = make_shared<Bucket>(nomeArquivo, profundidade, MAXIMO_REGISTROS);
        }
    }

    int funcaoHash(int ano, int profundidade) const {
        return ano & ((1 << profundidade) - 1);
    }

    int inserirRegistro(const Compra& compra, vector<pair<int,int>> &duplicacoesDiretorio) {
        int indice = funcaoHash(compra.ano, this->profundidadeGlobal);
        shared_ptr<Bucket> bucket = this->buckets[indice];

        bucket->carregar();
        if (!bucket->adicionarRegistro(compra)) {
            if (bucket->profundidadeLocal == profundidadeGlobal) {
                duplicarDiretorio();
                duplicacoesDiretorio.push_back(make_pair(profundidadeGlobal, bucket->profundidadeLocal));
            }
            dividirBucket(indice);
            return inserirRegistro(compra, duplicacoesDiretorio); // Tenta inserir compra novamente
        } else {
            bucket->salvar();
        }
        return bucket->profundidadeLocal;
    }

    void duplicarDiretorio() {
        int tamanhoAtual = buckets.size();
        profundidadeGlobal++;
        buckets.resize(tamanhoAtual * 2);

        for (int i = 0; i < tamanhoAtual; i++) {
            // Compartilha o mesmo bucket inicialmente
            buckets[tamanhoAtual + i] = buckets[i]; 
        }
    }

    int ajustarIndice(int indice, int PL) {
        // Converter 'indice' para binário considerando apenas PL dígitos menos significativos
        int mascara = (1 << PL) - 1; 
        int bits = indice & mascara; 

        // Adicionar '1' à esquerda dos bits extraídos
        int novoIndice = (1 << PL) | bits; 

        return novoIndice;
    }

    void dividirBucket(int indice) {
        shared_ptr<Bucket> bucket = this->buckets[indice];
        vector<Compra> registrosAntigos = bucket->registros;
        bucket->desalocarMemoriaRegistros();

        int profundidadeLocal = bucket->profundidadeLocal;
        int novaProfundidadeLocal = profundidadeLocal + 1;
        bucket->profundidadeLocal++;

        int novoIndice = ajustarIndice(indice, profundidadeLocal);

        string novaFilename = "bucket_" + to_string(novoIndice) + ".csv";
        shared_ptr<Bucket> novoBucket = make_shared<Bucket>(novaFilename, novaProfundidadeLocal);
        this->buckets[novoIndice] = novoBucket;

        for (const Compra& compra : registrosAntigos) {
            int novoIndiceCompra = funcaoHash(compra.ano, novaProfundidadeLocal);
            if (novoIndiceCompra == indice) {
                bucket->adicionarRegistro(compra);
            } else {
                novoBucket->adicionarRegistro(compra);
            }
        }
        bucket->salvar();
        novoBucket->salvar();
    }

    pair<int, int> removerRegistros(int ano) {
        int indice = funcaoHash(ano, profundidadeGlobal);
        shared_ptr<Bucket> bucket = buckets[indice];

        bucket->carregar();
        int contagemInicial = bucket->registros.size();

        bucket->registros.erase(
            remove_if(bucket->registros.begin(), bucket->registros.end(), [ano](const Compra& compra) {
                return compra.ano == ano;
            }),
            bucket->registros.end()
        );

        int contagemFinal = bucket->registros.size();
        int tuplasRemovidas = contagemInicial - contagemFinal;

        bucket->salvar();

        return {tuplasRemovidas, bucket->profundidadeLocal};
    }

    int buscarPorAno(int ano) {
        int indice = funcaoHash(ano, profundidadeGlobal);
        shared_ptr<Bucket> bucket = buckets[indice];

        bucket->carregar(); 
        
        int quantidadeEncontrada = count_if(bucket->registros.begin(),  bucket->registros.end(), 
                                            [ano](const Compra& compra) {
                                                return compra.ano == ano;
                                            });

        return quantidadeEncontrada;
    }

    void imprimir() {
        cout << "Profundidade Global: " << profundidadeGlobal << endl;
        for (auto& bucket : this->buckets) {
            bucket->imprimir();
        }
    }
};

int main() {

    ifstream arquivoInstrucoes("in.txt");
    ofstream arquivoSaida("out.txt");
    if (!arquivoInstrucoes.is_open() || !arquivoSaida.is_open()) {
        cerr << "Erro ao abrir um dos arquivos!" << endl;
        return 1;
    }

    string linha;
    getline(arquivoInstrucoes, linha);
    // Primeira linha: PG/<profundidade_inicial>
    int profundidadeInicial = std::stoi(linha.substr(3));
    arquivoSaida << linha << endl;

    Diretorio diretorio(profundidadeInicial);  

    while (getline(arquivoInstrucoes, linha)) {
        stringstream ss(linha);
        int ano;
        string operacao = linha.substr(0, 3);

        if (operacao == "INC") {
            ano = stoi(linha.substr(4));
            
            string nomeArquivoCompras = "compras.csv";
            LeitorCSV leitorCompras(nomeArquivoCompras);
            vector<Compra> compras = leitorCompras.lerCompras();
            
            for (const auto& compra : compras) {
                if (compra.ano == ano) {
                    vector<pair<int, int>> duplicacoesDiretorio;
                    int profundidadeLocal = diretorio.inserirRegistro(compra, duplicacoesDiretorio);
                    
                    arquivoSaida << "INC:" << ano << "/" << diretorio.profundidadeGlobal << "," << profundidadeLocal << endl;

                    for (auto duplicaoDiretorio : duplicacoesDiretorio) {
                        arquivoSaida << "DUP_DIR:/" << duplicaoDiretorio.first << "," << duplicaoDiretorio.second << endl;
                    }
                }
            }

            compras = vector<Compra>();
        } else if (operacao == "REM") {
            ano = stoi(linha.substr(4));

            // <quantidade de tuplas removidas, profundidade local>
            pair<int, int> resultadoRemocao = diretorio.removerRegistros(ano);

            arquivoSaida << "REM:" << ano << "/" << resultadoRemocao.first << "," << diretorio.profundidadeGlobal << "," << resultadoRemocao.second << endl;
        } else if (operacao == "BUS") {
            ano = stoi(linha.substr(5));
            int quantidadeDeTuplas = diretorio.buscarPorAno(ano);
            arquivoSaida << "BUS:" << ano << "/" << quantidadeDeTuplas << endl;
        }
    }

    arquivoSaida << "P:/" << diretorio.profundidadeGlobal << endl;

    arquivoInstrucoes.close();
    arquivoSaida.close();

    return 0;
}