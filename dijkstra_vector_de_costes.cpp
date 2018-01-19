//solución del algoritmo de Dijkstra con una matriz de adyacencia
//PAULA MUÑOZ LAGO

#include <iostream>
#include <set>
#include <limits>
#include <string>
#include <fstream>
#include <list>
#include <ctime>
using namespace std;

const string file_name = "grafo.txt";
const string file_costes = "costes.txt";

int **inicializarMatriz(int &x);
int **inicializarMatriz_alearoria(int tamano);
void Dijkstra(int **mat, int n, int *coste_min, int *predecesor);
void print_graph(int **mat, int n, int *coste_min, int *predecesor, int i);
void write_costs(double time, int n);

int main() {
	//1. rellenar el grafo (la matriz)
	bool aleatoria = false;
	int x, veces = 1, tamano = 4;
	int **matriz = NULL;

	string op;

	do {
		cout << "Quiere cargarlo de un archivo o generar 200 matrices aleatorias? (archivo, aleatorias): ";
		cin >> op;

		if (op == "archivo") {
			matriz = inicializarMatriz(x);
		}
		else if (op == "aleatorias") {
			veces = 200;
			aleatoria = true;
		}
	} while (op != "archivo" && op != "aleatorias");

	for (int i = 0; i < veces; i++) {
		if (aleatoria) {
			matriz = inicializarMatriz_alearoria(tamano); //vamos generando grafos de tamaño desde 4 a 203
			x = tamano;
			tamano++;
		}
		if (matriz != NULL) {
			//2. Aplicar el algoritmo
			int *coste_min = new int[x];
			int *predecesor = new int[x];

			clock_t begin = clock();
			Dijkstra(matriz, x, coste_min, predecesor);
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

			//3. Imprimir los resultados
			print_graph(matriz, x, coste_min, predecesor, i);
			write_costs(elapsed_secs, x);
		}
		else cout << "Ha ocurrido un error" << endl;

		delete[] matriz;
	}
	return 0;
}

int **inicializarMatriz(int &n) {
	ifstream file;
	file.open(file_name);
	int x;
	file >> x;
	int **matriz = NULL;

	if (file.is_open()) {

			matriz = new int*[x];
			for (int j = 0; j < x; j++) {
				matriz[j] = new int[x];
				for (int i = 0; i < x; i++) {
					file >> matriz[j][i];
				}
			}

		n = x;
	}
	else {
		cout << "Ha de ser una matriz cuadrada, modifica el archivo " << file_name << endl;
		return false;
	}
	file.close();

	return matriz;
}

int **inicializarMatriz_alearoria(int tamano) {
	srand(time(NULL));
	int **mat = NULL;
	
	mat = new int*[tamano];
	for (int i = 0; i < tamano; i++) {
		mat[i] = new int[tamano];
		for (int j = 0; j < tamano; j++) {
			mat[i][j] = rand() % 50;
		}
	}
	return mat;
}

void Dijkstra(int **mat, int n, int *coste_min, int *predecesor) {
	set<int> candidatos;

	int minimo, elegido;

	coste_min[0] = 0;
	predecesor[0] = -1; //wrong value

	for (int i = 1; i < n; i++) { //para la primera fila de la matriz, insertamos como candidatos los conectados con el nodo 0, el inicial
		if (mat[0][i] != 0) {
			candidatos.insert(i);
			coste_min[i] = mat[0][i];
			predecesor[i] = 0;
		}else coste_min[i] = std::numeric_limits<int>::max(); // infinito (todavía no hemos encontrado camino mínimo)
	}

	for (int i = 0; i < n - 2; i++) { //vamos a encontrar el mínimo en coste min
		minimo = std::numeric_limits<int>::max();
		
		for (int j = 1; j <= n; j++) {
			if ((candidatos.find(j) != candidatos.cend()) && coste_min[j] < minimo) {
				minimo = coste_min[j];
				elegido = j;
			}
		}

		candidatos.erase(elegido);

		for (int j = 1; j < n; j++) { //actualizamos los vectores
			if (mat[elegido][j] != 0 && coste_min[elegido] + mat[elegido][j] < coste_min[j]) { //el coste minimo de llegar a j es a través del elegido
				coste_min[j] = coste_min[elegido] + mat[elegido][j]; //llegaremos a j a través del elegido
				predecesor[j] = elegido; //actualiamos su predecesor
				candidatos.insert(j); // y lo incluimos en candidatos ya que quizás este nodo esté conectado con otros a su vez, y también queremos acceder a ellos
			}
		}
	}
}

void print_graph(int **mat, int n, int *coste_min, int *predecesor, int i) { //imprimo el grafo
	cout << "--------- GRAPH " << i << "---------" << endl;
	int totalAristas = 0;

	cout << "Aristas:" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (mat[i][j] != 0) {
				totalAristas++;
				cout << i + 1 << " - " << j + 1 << endl;
			}
		}
	}
	cout << "Nodo \t coste \t predecesor" << endl;
	for (int i = 1; i < n; i++) {
		cout << i + 1 << "\t " << coste_min[i] << "\t " << predecesor[i] << endl;
	}

	cout << "Numero de aristas: " << totalAristas << endl;
	cout << "Numero de nodos: " << n << endl;
	cout << "------------------------" << endl;
}

void write_costs(double time, int n) { //ejecutaremos este algoritmo con grafos de distintos tamaños y los resultados los escribiremos en este archivo
										//para después cargarlos en gnuplot
	ofstream file;
	file.open(file_costes, std::ofstream::app);
	file <<  n<< "\t" << time << endl;
	file.close();
}