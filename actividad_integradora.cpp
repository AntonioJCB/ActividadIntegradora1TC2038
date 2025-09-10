#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <memory>
#include <iomanip>
using namespace std;

string normalize(const string &s) {
    string res;
    for (char c : s)
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
            res += c;
    return res;
}

// Subsecuencias
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

//Busqueda de palindromos
pair<int,int> longestPalindrome(const string &s) {
int n = (int)s.size();
if (n == 0) return {0, 0};

int bestLen = 1;
int start = 0;

auto expand = [&](int l, int r) {
    while (l >= 0 && r < n && s[l] == s[r]) {
        l--;
        r++;
    }
    return pair<int,int>(l+1, r-1);
};

for (int i = 0; i < n; i++) {
    // palíndromos impares
    pair<int,int> p1 = expand(i, i);
    if (p1.second - p1.first + 1 > bestLen) {
        bestLen = p1.second - p1.first + 1;
        start = p1.first;
    }
    // palíndromos pares
    pair<int,int> p2 = expand(i, i+1);
    if (p2.second - p2.first + 1 > bestLen) {
        bestLen = p2.second - p2.first + 1;
        start = p2.first;
    }
}

return {start+1, start+bestLen}; 
}

// Huffman Coding

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r) : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Construir tabla de frecuencias
unordered_map<char,int> freqTable(const string &s) {
    unordered_map<char,int> freq;
    for (char c : s) {
        if (c != '\n' && c != '\r') // ignorar saltos de línea
            freq[c]++;
    }
    return freq;
}

// Construir árbol de Huffman
Node* buildHuffman(const unordered_map<char,int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto &p : freq) {
        pq.push(new Node(p.first, p.second));
    }
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a,b));
    }
    return pq.top();
}

// Construir códigos Huffman recursivamente
void buildCodes(Node* root, string code, unordered_map<char,string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = code;
    }
    buildCodes(root->left, code+"0", codes);
    buildCodes(root->right, code+"1", codes);
}

// Calcular longitud promedio esperada (∑ p_i * |code_i|)
double expectedLength(const unordered_map<char,int>& freq, const unordered_map<char,string>& codes, int total) {
    double avg = 0.0;
    for (auto &p : freq) {
        double prob = (double)p.second / total;
        avg += prob * codes.at(p.first).size();
    }
    return avg;
}

// Codificar un string y devolver longitud en bits
int encodeLength(const string &s, const unordered_map<char,string>& codes) {
    int len = 0;
    for (char c : s) {
        if (c == '\n' || c == '\r') continue;
        auto it = codes.find(c);
        if (it != codes.end()) {
            len += it->second.size();
        }
    }
    return len;
}

void analizarTransmision(const string& tName, const string& t, const vector<pair<string,string>>& mcodes) {
    // tabla de frecuencias
    auto freq = freqTable(t);
    if (freq.empty()) return;

    // árbol y códigos
    Node* root = buildHuffman(freq);
    unordered_map<char,string> codes;
    buildCodes(root, "", codes);

    // longitud promedio esperada
    double avg = expectedLength(freq, codes, (int)t.size());

    // longitud real de la transmisión
    int lenTrans = encodeLength(t, codes);

    cout << "\n--- " << tName << " ---\n";
    cout << "Promedio esperado: " << fixed << setprecision(3) << avg << " bits/símbolo\n";
    cout << "Longitud total codificada: " << lenTrans << " bits\n";

    // verificar cada mcode
    for (auto &m : mcodes) {
        int lenCode = encodeLength(m.second, codes);
        // criterio: sospechoso si supera 2x el promedio * longitud original
        double expected = avg * m.second.size();
        if (lenCode > expected * 2.0) {
            cout << m.first << ": sospechoso " << lenCode << "\n";
        } else {
            cout << m.first << ": no-sospechoso " << lenCode << "\n";
        }
    }
}

int main() {
    // Leer y normalizar transmisiones
    string transmission1 = normalize(leerArchivo("transmission1.txt"));
    string transmission2 = normalize(leerArchivo("transmission2.txt"));
    vector<string> transmissions = {transmission1, transmission2};
    vector<string> tnames = {"transmission1.txt", "transmission2.txt"};

    // Para cada mcode, leer TODO el archivo, normalizar (unir en una sola secuencia) y buscar
    vector<pair<string,string>> mcodes;
    for (int i = 1; i <= 3; ++i) {
        string mcode = normalize(leerArchivo("mcode" + to_string(i) + ".txt"));
        string mname = "mcode" + to_string(i) + ".txt";
        mcodes.push_back({mname, mcode});

        compararYImprimir("transmission1.txt", transmission1, mname, mcode);
        compararYImprimir("transmission2.txt", transmission2, mname, mcode);

        analizarTransmision("transmission1.txt", transmission1, mcodes);
        analizarTransmision("transmission2.txt", transmission2, mcodes);
    }

    pair<int,int> pal1 = longestPalindrome(transmission1);
    pair<int,int> pal2 = longestPalindrome(transmission2);

    cout <<"El palindromo mas largo de transmission1 sencuentra de "<< pal1.first << " a " << pal1.second << "\n";
    cout <<"El palindromo mas largo de transmission2 sencuentra de "<< pal2.first << " a " << pal2.second << "\n";
    return 0;
}
