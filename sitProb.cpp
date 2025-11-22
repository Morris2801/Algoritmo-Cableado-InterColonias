#include <iostream>
#include <vector>
#include <queue>
#include <map>

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


void funcionSimilarAlViajero(){


	
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

	// Forma óptima de cablear colonias para que entre cuales quiera 2 colonias se puedan compartir info
	/* Solución: generar arbol MST
		Algoritmo: PRIM con min-heap y lista de adyacencia
		Complejidad: O(mlogn) con m = numAristas, n = numNodos
	*/
	pair<string,int> resultadoMST = mstPrim(matrizAdj, indLetra, numEntradas);
	cout << resultadoMST.first << endl; // este imprime camino
	cout << resultadoMST.second << endl; // este imprime el costo, se necesita?
	


	// Ruta más corta para visitar colonias exactamente 1 vez y volver a origen
	

	return 0;
}
