#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <ctime>

using namespace std;

// *************************************************************************

// Constantes
const string nomeArquivo = "arc/a28.txt";
const int tempo = 10;
const int nColunas = 3;
const int nColunasValidas = 2;

// *************************************************************************

// Numero de vertices
int numeroVertices() {
    int numeroVertices = 0;
    ifstream arquivo(nomeArquivo);
    if(arquivo.is_open()) {
        int linha;
        while(!arquivo.eof()) {
            arquivo >> linha;
            numeroVertices++;
        }
        arquivo.close();
    } else {
        cout << "Falha ao ler o arquivo." << endl;
        exit(1);
    }
    numeroVertices /= nColunas;
    return numeroVertices;
}

// *************************************************************************

// Alocacao e liberacao de matrizes
int **alocarMatriz(int numLinhas, int numColunas){
    int **mat;
    mat = (int **)malloc(sizeof(int *)*numLinhas);
    for(int i = 0; i < numLinhas; i++){
        mat[i] = (int *)malloc(sizeof(int)*numColunas);
    }
    return mat;
}
void liberarMatriz(int **mat, int numLinhas){
    int i;
    for(i = 0; i < numLinhas; i++) {
        free(mat[i]);
    }
    free(mat);
}

// *************************************************************************

// Matriz com os dados do arquivo
int **criarMatrizArquivo(int numeroVertices) {
    int **matArquivo = alocarMatriz(numeroVertices, nColunasValidas);
    ifstream arquivo(nomeArquivo);
    if(arquivo.is_open()) {
        int dado;
        int lin = 0;
        int col = -1;
        while (!arquivo.eof()) {
            arquivo >> dado;
            if(col >= 0) {  
                matArquivo[lin][col] = dado;
                col++;
                if(col == nColunasValidas) {
                    col = -1;
                    lin++;
                }
            } else {
                col++;
            }            
        }
        arquivo.close();
    } else {
        cout << "Falha ao ler o arquivo.";
    }
    return matArquivo;
}

// *************************************************************************

// Peso entre dois vertices
int calcularPeso(int x1, int y1, int x2, int y2) {
    return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

// *************************************************************************

// Gera número aleatório de 1 até N
int nAleatorio(int n) {
    return (rand() % n)+1;
}

// *************************************************************************

// <------ Estrutura do Grafo ------>

class Adjacencia {
public:
    int v;
    int peso;

    Adjacencia(int v, int peso) {
        this->v = v;
        this->peso = peso;
    }

    bool operator < (const Adjacencia& a2) const {
		return (peso > a2.peso);
	}
};

class Aresta {
public:
    int v1;
    int v2;
    int peso;

    Aresta() {
    }
    Aresta(int v1, int v2, int peso) {
        this->v1 = v1;
        this->v2 = v2;
        this->peso = peso;
    }
};

class Grafo {
public:
    int nVertices;
    vector<Aresta> arestas;
    Aresta maior;

    Grafo(int nVertices, vector<Aresta> arestas) {
        this->nVertices = nVertices;
        this->arestas = arestas;
    }

    void exibirArestas() {
        cout << "Lista de Arestas: " << endl;
        for(Aresta x : arestas) {
            cout << x.v1 << " <--{ " << x.peso << " }--> " << x.v2 << endl;
        }
        cout << endl;
    }
    
    Grafo criarGrafoArquivo(int nVertices, int **matArquivo) {
        Aresta maior(0, 0, 0);
        vector<Aresta> arestas;
        for(int i = 0; i < nVertices; i++) {
            for(int j = i+1; j < nVertices; j++) {
                Aresta a(i+1, j+1, calcularPeso(matArquivo[i][0], matArquivo[i][1], matArquivo[j][0], matArquivo[j][1]));
                arestas.push_back(a);
                if(a.peso > maior.peso) {
                    maior = a;
                }
            }
        }
        Grafo g(nVertices, arestas);
        g.maior = maior;
        return g;
    }

     // <------ Lista de Adjacência ------>
    vector<Adjacencia> *criarListaAdj() {
        vector<Adjacencia> *adj = new vector<Adjacencia>[nVertices+1];
        Adjacencia a(0, 0);
        for(Aresta x : arestas) {
            a.peso = x.peso;
            a.v = x.v2;
            adj[x.v1].emplace_back(a);
            a.v = x.v1;
            adj[x.v2].emplace_back(a);
        }
        return adj;
    }
    void exibirListaAdj(vector<Adjacencia> *adj){
        cout << "Lista de Adjacencia: " << endl;
        for (int i = 1; i <= nVertices; i++) {
            cout << "[" << i << "] -> ";
            for(Adjacencia x : adj[i]) {
                cout << "[ " << x.v << " | (" << x.peso << ")" << " ] ";
            }
            cout << endl;
        }
        cout << endl;
    }

};

// *************************************************************************

// Classe Subgrafo

class Subgrafo {
public:
    vector<int> vertices;
    vector<Aresta> arestas;
    int peso;

    void exibirMaiorSubgrafo() {
        cout << nomeArquivo << endl;
        for(int v : vertices) {
            cout << v << " ";
        }
        cout << endl;
        cout << peso << endl;
    }

    // Heuristica Construtiva
    void criarSubgrafoInicial(Grafo g, vector<Adjacencia> *lAdj) {
        int v1 = g.maior.v1;
        int v2 = g.maior.v2;
        int v3, v4;

        arestas.push_back(g.maior);
        vertices.push_back(v1);
        vertices.push_back(v2);

        sort(lAdj[v1].begin(), lAdj[v1].end());
        sort(lAdj[v2].begin(), lAdj[v2].end());

        for(Adjacencia x : lAdj[v1]) {
            if(x.v != v2) {
                Aresta a(v1, x.v, x.peso);
                arestas.push_back(a);
                vertices.push_back(x.v);
                v3 = x.v;
                break;   
            }
        }
        for(Adjacencia x : lAdj[v2]) {
            if(x.v != v1 && x.v != v3) {
                Aresta b(v2, x.v, x.peso);
                arestas.push_back(b);
                vertices.push_back(x.v);
                v4 = x.v;
                break;   
            }
        }

        int v5 = nAleatorio(g.nVertices);
        while (v5 == v1 || v5 == v2 || v5 == v3 || v5 == v4) {
            v5 = nAleatorio(g.nVertices);
        }

        int indiceV3 = v3-1;
        int indiceV4 = v4-1;

        if(v3 > v5) {
            indiceV3--;
        }
        if(v4 > v5) {
            indiceV4--;
        }

        if(lAdj[v5].at(indiceV3).peso > lAdj[v5].at(indiceV4).peso) {
            Aresta c(lAdj[v5].at(indiceV3).v, v5, lAdj[v5].at(indiceV3).peso);
            arestas.push_back(c);
        } else {
            Aresta c(lAdj[v5].at(indiceV4).v, v5, lAdj[v5].at(indiceV4).peso);
            arestas.push_back(c);
        }
        vertices.push_back(v5);
        
        peso = 0;
        for(Aresta a : arestas) {
            peso += a.peso;
        }
    }

    // Lista de Adjacência do Subgrafo
    vector<Adjacencia> *criarListaAdj(int nVertices) {
        vector<Adjacencia> *adj = new vector<Adjacencia>[nVertices+1];
        Adjacencia a(0, 0);
        for(Aresta x : arestas) {
            a.peso = x.peso;
            a.v = x.v2;
            adj[x.v1].emplace_back(a);
            a.v = x.v1;
            adj[x.v2].emplace_back(a);
        }
        return adj;
    }
    
    // Busca Local
    Subgrafo buscaLocal(bool *verticesUtilizados, int *nVerticesUtilizados, Grafo g, vector<Adjacencia> *adj) {
        Subgrafo s = *this;

        if(nVerticesUtilizados[0] >= g.nVertices) {
            return s;
        }
        nVerticesUtilizados[0]++;

        // Define um novo vertice para adicionar, que não tenha sido adicionado
        int novoV = nAleatorio(g.nVertices);
        while (verticesUtilizados[novoV-1]) {
            novoV = nAleatorio(g.nVertices);
        }
        verticesUtilizados[novoV-1] = true;

        vector<Adjacencia> *adjSub = criarListaAdj(g.nVertices);

        for(int v : s.vertices) {
            int somaV = 0;
            int somaNovoV = 0;
            vector<Adjacencia> adjacenciasNovoV;
            for(Adjacencia a : adjSub[v]) {
                somaV += a.peso;
                int indice = a.v-1;
                if(a.v > novoV) {
                    indice--;
                }
                somaNovoV += adj[novoV].at(indice).peso;
                adjacenciasNovoV.push_back(adj[novoV].at(indice));
            }
            if(somaV < somaNovoV) {
                for(int j = 0; j < s.vertices.size(); j++) {
                    if(s.vertices.at(j) == v) {
                        s.vertices.at(j) = novoV;
                        break;
                    }
                }
                for(int i = 0; i < s.arestas.size(); i++) {
                    if(s.arestas.at(i).v1 == v) {
                        s.arestas.at(i).v1 = novoV;
                        for(Adjacencia d : adjacenciasNovoV) {
                            if(d.v == s.arestas.at(i).v2) {
                                s.peso -= s.arestas.at(i).peso;
                                s.peso += d.peso;
                                s.arestas.at(i).peso = d.peso;
                                break;
                            }
                        }
                    } else if(s.arestas.at(i).v2 == v) {
                        s.arestas.at(i).v2 = novoV;
                        for(Adjacencia d : adjacenciasNovoV) {
                            if(d.v == s.arestas.at(i).v1) {
                                s.peso -= s.arestas.at(i).peso;
                                s.peso += d.peso;
                                s.arestas.at(i).peso = d.peso;
                                break;
                            }
                        }
                    }
                }
            }
        }
        return s;
    }
};

// *************************************************************************

int main() {
    // Seed Random
    srand(1);

    // Dados para o tempo de execução
    time_t start, t;
    start = time(NULL);

    // ------ Leitura do Arquivo ------

    // Numero de vertices
    int nVertices = numeroVertices();

    // Criacao da matriz com os dados do arquivo
    int **matArquivo = criarMatrizArquivo(nVertices);

    // ------ Formação e representação do Grafo ------

    // Criacao do grafo do arquivo
    Grafo g = g.criarGrafoArquivo(nVertices, matArquivo);

    // Liberação da alocação
    liberarMatriz(matArquivo, nVertices);

    // Representação do Grafo em Lista de Adjacência
    vector<Adjacencia> *lAdj = g.criarListaAdj();

    // ------ Heurística Construtiva ------

    Subgrafo s;

    s.criarSubgrafoInicial(g, lAdj);

    // ------ Busca Local ------

    bool verticesUtilizados[nVertices] = {};
    for(int v : s.vertices) {
        verticesUtilizados[v-1] = true;
    }
    int nVerticesUtilizados[1] = {5};
    t = time(NULL) - start;

    // Aplicação da busca local até o tempo limite
    while (t < tempo) {
        s = s.buscaLocal(verticesUtilizados, nVerticesUtilizados, g, lAdj);
        t = time(NULL) - start;
    }

    // Exibe o maior subgrafo encontrado
    s.exibirMaiorSubgrafo();

    return 0;
}
