#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
using namespace std;

vector<int> computeFailure(const string &P) {
    int m = (int)P.size();
    vector<int> f(m, 0);
    for (int i = 1, j = 0; i < m; ++i) {
        while (j > 0 && P[i] != P[j]) j = f[j - 1];
        if (P[i] == P[j]) ++j;
        f[i] = j;
    }
    return f;
}

vector<int> KMP(const string &T, const string &P) {
    vector<int> res;
    if (P.empty() || T.empty() || P.size() > T.size()) return res;
    auto f = computeFailure(P);
    for (int i = 0, j = 0; i < (int)T.size(); ++i) {
        while (j > 0 && T[i] != P[j]) j = f[j - 1];
        if (T[i] == P[j]) ++j;
        if (j == (int)P.size()) {
            res.push_back(i - (int)P.size() + 1);
            j = f[j - 1];
        }
    }
    return res;
}

string leerArchivo(const string &ruta) {
    ifstream archivo(ruta);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo " << ruta << "\n";
        return "";
    }
    stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

// Quita saltos de línea y deja sólo 0-9 y A-F (en mayúsculas).
string normalizar(const string &s) {
    string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if (c == '\n' || c == '\r') continue;        // ignorar saltos de línea
        if (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
            out.push_back((char)toupper(c));
        }
        // Si quieres ser estricto y rechazar cualquier otro char, simplemente no lo agregues.
    }
    return out;
}

void compararYImprimir(const string &tName, const string &t, const string &mName, const string &m) {
    auto pos = KMP(t, m);
    if (!pos.empty())
        cout << tName << " vs " << mName << ": true " << pos[0] << "\n";
    else
        cout << tName << " vs " << mName << ": false\n";
}

int main() {
    // Leer y normalizar transmisiones
    string transmission1 = normalizar(leerArchivo("transmission1.txt"));
    string transmission2 = normalizar(leerArchivo("transmission2.txt"));

    // Para cada mcode, leer TODO el archivo, normalizar (unir en una sola secuencia) y buscar
    for (int i = 1; i <= 3; ++i) {
        string mcode = normalizar(leerArchivo("mcode" + to_string(i) + ".txt"));
        string mname = "mcode" + to_string(i) + ".txt";

        compararYImprimir("transmission1.txt", transmission1, mname, mcode);
        compararYImprimir("transmission2.txt", transmission2, mname, mcode);
    }
    return 0;
}
