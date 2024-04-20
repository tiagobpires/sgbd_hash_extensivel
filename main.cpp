#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <memory>

using namespace std;

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
        ofstream file(filename);
        file.close();
    }

    Bucket(string filename, vector<Compra> registros, int profundidadeLocal, int maximoRegistros=3) : filename(filename), profundidadeLocal(profundidadeLocal), maximoRegistros(maximoRegistros) {
        if (registros.size() > this->maximoRegistros) {
            cout << "Máximo de registros é " << this->maximoRegistros << endl;
        }
        this->registros = registros;
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

    bool adicionarRegistro(Compra compra) {
        if (!cheio()) {
            registros.push_back(compra);
            salvar(); 
            return true;
        }
        return false;
    }

    void imprimir() {
        for (Compra entry : this->registros) {
            cout << entry.pedido << endl;
        }
    }
};

class Diretorio {
private:
    int profundidadeGlobal;
    vector<shared_ptr<Bucket>> buckets;

public:
    Diretorio(int profundidade) : profundidadeGlobal(profundidade) {
        int tamanhoInicial = pow(2, profundidadeGlobal);
        this->buckets.resize(tamanhoInicial);

        for (int i = 0; i < tamanhoInicial; ++i) {
            string nomeArquivo = "bucket_" + to_string(i) + ".csv";
            this->buckets[i] = make_shared<Bucket>(nomeArquivo, 3, profundidadeGlobal);
        }
    }

    int funcaoHash(int ano, int profundidade) const {
        return ano & ((1 << profundidade) - 1);
    }

    void inserirRegistro(const Compra& compra) {
        int indice = funcaoHash(compra.ano, this->profundidadeGlobal);
        shared_ptr<Bucket> bucket = this->buckets[indice];

        bucket->carregar();
        if (!bucket->adicionarRegistro(compra)) {
            if (bucket->profundidadeLocal == profundidadeGlobal) {
                duplicarDiretorio();
            }
            dividirBucket(indice);
            inserirRegistro(compra); // Tenta inserir compra novamente
        }
        bucket->salvar();
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

    void dividirBucket(int indice) {
        shared_ptr<Bucket> bucket = this->buckets[indice];
        vector<Compra> registrosAntigos = bucket->registros;
        bucket->registros.clear();

        int profundidadeLocal = bucket->profundidadeLocal;
        int novaProfundidadeLocal = profundidadeLocal + 1;
        bucket->profundidadeLocal++;

        string novaFilename = "bucket_" + to_string(this->buckets.size()) + ".csv";
        shared_ptr<Bucket> novoBucket = make_shared<Bucket>(novaFilename, novaProfundidadeLocal);
        this->buckets.push_back(novoBucket);

        for (const Compra& compra : registrosAntigos) {
            int novoIndice = funcaoHash(compra.ano, novaProfundidadeLocal);
            if (novoIndice == indice) {
                bucket->adicionarRegistro(compra);
            } else {
                novoBucket->adicionarRegistro(compra);
            }
        }
        bucket->salvar();
        novoBucket->salvar();
    }

    void imprimir() {
        cout << "Profundidade Global: " << profundidadeGlobal << endl;
        for (auto& bucket : this->buckets) {
            bucket->imprimir();
        }
    }
};


int main() {
    string nomeArquivo = "compras.csv";
    LeitorCSV leitor(nomeArquivo);
    vector<Compra> compras = leitor.lerCompras();

    Diretorio diretorio(2);

    for (const auto& compra : compras) {
        diretorio.inserirRegistro(compra);
    }

    return 0;
}
