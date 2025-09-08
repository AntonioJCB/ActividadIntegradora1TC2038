#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

vector<int> computeFailure(const string &P) {
    int m = P.size();        // m = longitud del patrón
    vector<int> f(m, 0);     // f[i] = longitud del prefijo/sufijo más largo
    int j = 0;               // longitud del prefijo actual coincidente

    for (int i = 1; i < m; ++i) {
        // Retrocede hasta encontrar coincidencia o llegar a 0
        while (j > 0 && P[i] != P[j]) 
            j = f[j - 1];

        // Si hay coincidencia, extiende el prefijo
        if (P[i] == P[j]) 
            ++j;

        f[i] = j;  // guarda la longitud del prefijo más largo para i
    }
    return f;
}


vector<int> KMP(const string &T, const string &P) {
    int n = T.size(), m = P.size();
    auto f = computeFailure(P);   // obtener el arreglo de prefijos
    vector<int> res;              // aquí se guardan las posiciones encontradas
    int j = 0;                    // índice para el patrón

    for (int i = 0; i < n; ++i) { // recorrer el texto
        // Si hay desajuste, retrocede usando f[]
        while (j > 0 && T[i] != P[j])
            j = f[j - 1];

        // Si coincide el carácter, avanza en el patrón
        if (T[i] == P[j])
            ++j;

        // Si llegaste al final del patrón, encontraste una ocurrencia
        if (j == m) {
            res.push_back(i - m + 1); // posición de inicio en T
            j = f[j - 1];            // continuar buscando posibles solapamientos
        }
    }
    return res;
}

string leerArchivos(string searchfile) {
    ifstream archivo(searchfile);
    if (!archivo) {
        cout << "No se pudo abrir el archivo " << searchfile << "\n";
        return "";
    }

    stringstream buffer;
    buffer << archivo.rdbuf();  // leer todo el contenido
    return buffer.str();
}

int main() {

    string transmission1 = leerArchivos("transmission1.txt");
    string transmission2 = leerArchivos("transmission2.txt");

    for(int i = 1; i <= 3; i++) {
        string fileInfo = leerArchivos("mcode" + to_string(i) + ".txt");

        vector<int> pos1 = KMP(transmission1, fileInfo);
        if (!pos1.empty())
            cout << "transmission1.txt vs mcode" << i << ": true " << pos1[0] << endl;
        else
            cout << "transmission1.txt vs mcode" << i << ": false" << endl;

        vector<int> pos2 = KMP(transmission2, fileInfo);
        if (!pos2.empty())
            cout << "transmission2.txt vs mcode" << i << ": true " << pos2[0] << endl;
        else
            cout << "transmission2.txt vs mcode" << i << ": false" << endl;

    
    }
  


    return 0;
}
