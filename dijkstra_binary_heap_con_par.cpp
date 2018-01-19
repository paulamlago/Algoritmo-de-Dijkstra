#include <iostream>
#include <fstream>
#include <set>
#include <ctime>
#include <list>
#include<string>
#include <map>
using namespace std;

class par {
	int elem;
	int prioridad;

public:
	par() { elem = 0, prioridad = 0; }
	par(int x, int y) { elem = x, prioridad = y; }
	int getElem() { return elem; }
	int getPrio() { return prioridad; }
	void changePrio(int p) { prioridad = p; }
	void changeElem(int e) { elem = e; }
};


class Monticulo_Williams_Minimos {
	par *v; //vector en el que guardamos los valores de los nodos, primera posicion: 0
	int *posiciones;
	int ultimo;
	int max_size; //ultima posicion: tamañoMaximo-1
	int size;

public:

	Monticulo_Williams_Minimos(int); //el parametro es el tamaño máximo 
	void insert(par);
	void flotar(int);
	void hundir(int j, int k);
	void intercambiar(int, int); //para flotar y hundir: necesario a la hora de insertar, borrar, decrecer clave...
	par  elem_minimo(); //como es un montículo de mínimos, es el primer elemento del vector
	void borra_min();
	void decrecer_clave(int); //!!
	void modificar(int valor, int destino);
	int getSize() { return size; }
	par getValue(int i) { return v[i]; }
	//las siguientes funciones devuelven la posicion en la que se encuentran los hijos y el padre, no el elemento
	int hijo_iz(int padre) { return padre * 2 + 1; }
	int hijo_dr(int padre) { return padre * 2 + 2; }
	int padre(int nodo) { return (nodo - 1) / 2; }
};

Monticulo_Williams_Minimos::Monticulo_Williams_Minimos(int t) {
	max_size = t;
	size = 0;
	v = new par[t];
	posiciones = new int[t];
}

void Monticulo_Williams_Minimos::insert(par element) {
	//si se puede insertar, es decir, no hemos llegado a la capacidad máxima: insertamos
	if (size != max_size) {
		v[size] = element;
		posiciones[size] = element.getElem();
		size++;
		//si no se trata del primer elemento que insertamos y el padre del elemento es 
		//mayor que este nuevo elemento habremos violado la condicion de montículo de minimos
		//para solucionarlo usamos la funcion intercambiar
		flotar(size - 1);
	}
	else cout << "Ya no se pueden insertar más elementos!" << endl;
}

void Monticulo_Williams_Minimos::flotar(int i) {
	if (i != 0) {
		for (; v[padre(i)].getElem() > v[i].getElem(); i = padre(i)) {
			intercambiar(padre(size - 1), i);
		}
	}
}

void Monticulo_Williams_Minimos::hundir(int j, int k) {
	bool fin = false;
	int m;

	while (2 * j <= size && !fin) {

		if (2 * j + 1 <= k && v[2 * j + 1].getElem() < v[2 * j].getElem()) m = 2 * j + 1;
		if (2 * j + 1 > k || v[2 * j + 1].getElem() >= v[2 * j].getElem()) m = 2 * j;

		if (v[j].getElem() > v[m].getElem()) { 
			intercambiar(j, m); 
			j = m; 
		}
		else fin = true;
	}
}

void Monticulo_Williams_Minimos::intercambiar(int x, int y) {
	par aux = v[x];
	v[x] = v[y];
	v[y] = aux;
}

par Monticulo_Williams_Minimos::elem_minimo() {
	if (size > 0) return v[0];
	else cout << "elem_minimo: No hay elementos en el monticulo!" << endl;
	return par(-1, -1); 
}

void Monticulo_Williams_Minimos::borra_min() {
	//el tamaño del monticulo es mayor que 0 siempre
	if (size == 1) {
		size = 0;
	}
	else {
		v[0] = v[size - 1]; //convertimos en raiz la hoja más a la derecha del ultimo nivel
		size--;
		//hundimos la raíz (la intercambiamos por el menor de sus hijos)
		//mientras que el elemento no sea una hoja o sea mayor que alguno de sus hijos los intercambiamos
		hundir(1, size);
	}
}

void Monticulo_Williams_Minimos::decrecer_clave(int i) { //siendo i el indice de la clave que queremos decrementar (-1)
	if (i <= size) {
		v[i].changeElem(v[i].getElem() - 1);
		flotar(i);
	}
	else cout << "decrecer_clave:Ese elemento no existe en el monticulo.";
}

void Monticulo_Williams_Minimos::modificar(int pos, int prio) { //lo utilizamos cuando T es de tipo par
	if (pos > size) { //si no está en el montículo lo metemos
		insert(par(pos, prio));
	}
	else v[pos].changePrio(prio);
}

const string file_name = "grafo.txt";
const string file_costes = "costes_mont.txt";

typedef struct info_arista{
	int destino;
	int valor;
};

void write_costs(double time, int n);
int **inicializarMatriz(int &n);
int **inicializarMatriz_alearoria(int tamano);
list<info_arista> copia_lista(int **v, int fila, int n);
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

		for (int i = 0; i < x; i++) {
			delete[] matriz[i];
		}
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
	
	par minimo;
	Monticulo_Williams_Minimos *M = new Monticulo_Williams_Minimos(n); //conjunto de posiciones 
	list<info_arista> l;
	info_arista ar;

	int elegido;

	coste_min[0] = 0;
	predecesor[0] = -1; //valor erroneo

	//rellenamos el resto de costes_minimos a infinito ya que todaía no hemos llegado a ellos
	for (int i = 1; i < n; i++) { //para la primera fila de la matriz, insertamos como candidatos los conectados con el nodo 0, el inicial
		if (mat[0][i] != 0) {
			coste_min[i] = mat[0][i];
			predecesor[i] = 1;
		}
		else coste_min[i] = std::numeric_limits<int>::max(); // infinito (todavía no hemos encontrado camino mínimo)
	}
	
	l = copia_lista(mat, 0, n);

	int i;
	while (!l.empty()) {
		i = l.front().destino;
		coste_min[i] = l.front().valor;
		M->insert(par(i, coste_min[i]));
		l.pop_front();
	}

	for (int i = 1; i < n - 1; i++) {
		minimo = M->elem_minimo();
		M->borra_min();
		elegido = minimo.getElem();
		l = copia_lista(mat, elegido, n);
		int j, coste;
		while (!l.empty()) {
			j = l.front().destino;
			coste = coste_min[elegido] + l.front().valor;
			if (coste < coste_min[j]) {
				coste_min[j] = coste;
				predecesor[j] = elegido + 1;
				M->modificar(j, coste);
			}

			l.pop_front();
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
	file << n << "\t" << time << endl;
	file.close();
}

list<info_arista> copia_lista(int **m, int fila, int n) {
	list<info_arista> l;
	info_arista ar;
	for (int i = fila + 1; i < n; i++) {
		if (m[fila][i] != 0) {
			ar.destino = i;
			ar.valor = m[fila][i];
			l.push_back(ar);
		}
	}
	return l;
}