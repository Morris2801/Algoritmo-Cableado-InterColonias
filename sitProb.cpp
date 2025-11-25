#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <cstring>
#include <climits>

using namespace std;

pair<string,int> mstPrim(vector<vector<int>> & matriz, map<int, char> letraIndice, int numNodos){
	vector<bool> enMST(numNodos, false);
	priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> minHeap;

	string caminoMST = "";
	int distancia = 0;
	enMST[0] = true; // Empezar desde el nodo 0
	for (int colonia = 0; colonia < numNodos; colonia++){
		if(colonia != 0 && matriz[0][colonia] != 0){
			minHeap.push({matriz[0][colonia], {0, colonia}}); // Costo, (origen, destino)
		}
	}
	int aristasAgregadas =0;
	while (!minHeap.empty() && aristasAgregadas<numNodos -1){
		pair<int, pair<int, int>> actual = minHeap.top();
		minHeap.pop();
		int costo = actual.first;
		int origen = actual.second.first;
		int destino = actual.second.second;

		if(enMST[destino])continue; // colonia ya en MST
		enMST[destino] = true;
		aristasAgregadas++;
		distancia += costo;
		char letraOrigen = letraIndice[origen]; // para ponerle letras en lugar de índices
		char letraDestino = letraIndice[destino]; // lo mismo
		string concatTemp = string("(") + letraOrigen + ',' + letraDestino +")";
		caminoMST += concatTemp;

		if (aristasAgregadas < numNodos -1){
			caminoMST +=" ";
		} // separar entre (x,x)_(x,x)

		for(int colonia = 0; colonia < numNodos; colonia++){
			if(!enMST[colonia] && matriz[destino][colonia] !=0){
				minHeap.push({matriz[destino][colonia], {destino, colonia}}); // pega otro "nodo":  Costo, (origen, destino)
			}
		}
	}
	return {caminoMST, distancia};
}

//Variables globales para TSP con Branch & Bound
vector<int> caminoF;
vector<bool> visitado;
int N;
int pesoMin;

//Aqui almaceno los caminos temporales encontrados
void copia(vector<int>& caminoActual){
    caminoF = caminoActual;
    caminoF.push_back(caminoActual[0]); // Cerrar el ciclo
}

//Con esta funcion encuentro el costo minimo del borde (arista)
int firstMin(vector<vector<int>>& adj, int i)
{
    int min = INT_MAX;
    for (int k=0; k<N; k++)
        if (adj[i][k]<min && i != k)
            min = adj[i][k];
    return min;
}

//Con esta segunda funcion, encuentro el segundo costo minimo
int secondMin(vector<vector<int>>& adj, int i)
{
    int first = INT_MAX, second = INT_MAX;
    for (int j=0; j<N; j++){
        if (i == j || adj[i][j] == 0)
            continue;

        if (adj[i][j] <= first){
            second = first;
            first = adj[i][j];
        }
        else if (adj[i][j] <= second &&
                 adj[i][j] != first)
            second = adj[i][j];
    }
    return second;
}

//Funcion recursiva que toma como argumentos:
    //cotaActual: cota inferior del nodo raiz
    //pesoActual: almacena el peso del camino hasta ahora
    //nivel: nivel actual
    //caminoActual: Almacena la solucion que luego sera copiada a caminoF
void TSPRec(vector<vector<int>>& adj, int cotaActual, int pesoActual, int nivel, vector<int>& caminoActual)
{
    // Caso base: Se cubrieron todos los nodos
    if (nivel==N){
        //Verificar si hay camino de regreso desde ultimo vertice a primero
        if (adj[caminoActual[nivel-1]][caminoActual[0]] != 0){

            //resultadoActual tiene el peso total de la solucion
            int resultadoActual = pesoActual + adj[caminoActual[nivel-1]][caminoActual[0]];

            //Actualizar el resultado si se encontro uno mejor al que ya se tenia
            if (resultadoActual < pesoMin)
            {
                copia(caminoActual);
                pesoMin = resultadoActual;
            }
        }
        return;
    }

    //Si todavia no se llega al ultimo nivel, se itera en el resto para hacer la busqueda en un arbol
    for(int i=0; i<N; i++){
        //Si no ha sido visitado, se toma el siguiente nodo
        if (adj[caminoActual[nivel-1]][i] != 0 && !visitado[i]){
            int temp = cotaActual;
            pesoActual += adj[caminoActual[nivel-1]][i];

            //Si es nivel uno, se hace un calculo diferente
            if (nivel==1)
              cotaActual -= ((firstMin(adj, caminoActual[nivel-1]) + firstMin(adj, i))/2);
            else
            //Para todos los demás niveles, se hace el calculo habitual
              cotaActual -= ((secondMin(adj, caminoActual[nivel-1]) + firstMin(adj, i))/2);

            // Si la cota inferior actual<pesoMin, se sigue explorando
            if (cotaActual + pesoActual < pesoMin){
                caminoActual[nivel] = i;
                visitado[i] = true;

                //Se llama a la funcion recursiva para el siguiente nivel
                TSPRec(adj, cotaActual, pesoActual, nivel+1,
                       caminoActual);
            }

            //Si no, se poda el nodo, restableciendo el pesoActual y cotaActual
            pesoActual -= adj[caminoActual[nivel-1]][i];
            cotaActual = temp;

            // También restablecer el arreglo visitado
            fill(visitado.begin(), visitado.end(), false);
            for (int j=0; j<=nivel-1; j++)
                visitado[caminoActual[j]] = true;
        }
    }
}

/* Esta función resuelve TSP usando Branch & Bound
   Algoritmo: Branch & Bound con cotas de reducción (Little)
   Estructuras: vectores dinámicos, recursión con poda, priority implícito
   Complejidad: O(n!) peor caso, mucho mejor en práctica por la poda
*/
string TSP(vector<vector<int>>& adj, map<int, char>& indLetra){
    // Inicializar variables globales
    N = adj.size();
    caminoF.clear();
    visitado.assign(N, false);
    pesoMin = INT_MAX;
    
    vector<int> caminoActual(N, -1);

    //Calculo de la cota inferior inicial para todas las aristas: (suma de primer min+segundo min)/2 
    int cotaActual = 0;
    
    //Calculo de la cota inicial
    for (int i=0; i<N; i++){
        cotaActual += (firstMin(adj, i) + secondMin(adj, i));
    }

    //Redondeo la cota inferior a un entero
    cotaActual = (cotaActual&1)? cotaActual/2 + 1 : cotaActual/2;

    //El vertice en caminoActual es 0 por ser el primer vertice
    visitado[0] = true;
    caminoActual[0] = 0;

    //Llamada a TSPRec con pesoActual=0 y nivel=1
    TSPRec(adj, cotaActual, 0, 1, caminoActual);
    
    //String de salida para el problema
    string ruta = "";
    for (size_t i = 0; i < caminoF.size(); i++) {
        ruta += indLetra[caminoF[i]];
        if (i < caminoF.size() - 1) ruta += " ";
    }
    
    return ruta;
}


int main(){
	int numEntradas;
    cin >> numEntradas;
    vector<vector<int>> matrizAdj(numEntradas, vector<int>(numEntradas));
    vector<vector<pair<int, int>>> listaAdyacencia(numEntradas);

	// Mapeo para poner letras a nodos
	map<int, char> indLetra;
	map<char, int> letraIndice;
	for (int i = 0; i<numEntradas; i++){
		char letra = 'A' + i; //Ascii + ajuste offset
		indLetra[i] = letra;
		letraIndice[letra] = i;
	}

    // Leer matriz y construir lista de adyacencia
    for (int row = 0; row < numEntradas; row++) {
        for (int col = 0; col < numEntradas; col++) {
            cin >> matrizAdj[row][col]; // Kilometros entre colonias
        }
    }
	/*
    cout << "\n\nMatriz de adyacencia :" << endl;
    for (int i = 0; i < numEntradas; i++) {
        cout << matrizAdj[i][0];
        for (int j = 1; j < numEntradas; j++) {
            cout << " " << matrizAdj[i][j];
        }
        cout << endl;
    }
	*/

	// Forma optima de cablear colonias para que entre cuales quiera 2 colonias se puedan compartir info
	/* Solucion: generar arbol MST
		Algoritmo: PRIM con min-heap y lista de adyacencia
		Complejidad: O(mlogn) con m = numAristas, n = numNodos
	*/
	pair<string,int> resultadoMST = mstPrim(matrizAdj, indLetra, numEntradas);
	cout << "Forma de cableado optimo con PRIM: "<<resultadoMST.first << endl; // este imprime camino
	cout << "Costo: "<<resultadoMST.second << endl; // este imprime el costo, se necesita?
	
	// Ruta mas corta para visitar colonias exactamente 1 vez y volver a origen

	// 2.Ruta mas corta para visitar cada colonia exactamente una vez y regresar al origen (TSP)
	/* Solución: Traveling Salesman Problem (TSP)
		Algoritmo: Branch & Bound
		Estructuras de datos: vectores, recursion con poda
		Complejidad: O(n!) peor caso, pero gracias a la poda, con casos de prueba grandes, es mas eficiente que uno de fuerza bruta
	*/
	string rutaTSP = TSP(matrizAdj, indLetra);
	cout << "Ruta mas corta para visitar cada colonia una vez y regresar: "<<rutaTSP << endl;
	

	return 0;
}
