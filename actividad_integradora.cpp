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


    void compararYImprimir(const string &tName, const string &t, const string &mName, const string &m) {
        auto pos = KMP(t, m);
        if (!pos.empty())
            cout << tName << " vs " << mName << ": true " << pos[0] << "\n";
        else
            cout << tName << " vs " << mName << ": false\n";
    }

    int main() {
        // Leer y normalizar transmisiones
        string transmission1 = leerArchivo("transmission1.txt");
        string transmission2 = leerArchivo("transmission2.txt");

        // Para cada mcode, leer TODO el archivo, normalizar (unir en una sola secuencia) y buscar
        for (int i = 1; i <= 3; ++i) {
            string mcode = leerArchivo("mcode" + to_string(i) + ".txt");
            string mname = "mcode" + to_string(i) + ".txt";

            compararYImprimir("transmission1.txt", transmission1, mname, mcode);
            compararYImprimir("transmission2.txt", transmission2, mname, mcode);
        }
        return 0;
    }
