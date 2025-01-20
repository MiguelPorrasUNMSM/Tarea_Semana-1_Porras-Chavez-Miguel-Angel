#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

enum Palo { corazones, treboles, picas, diamantes, comodines };

struct Carta {
    int valor; // 1 a 13 para cartas regulares, 14 para comodines
    Palo palo;
};

// Función para generar un mazo de cartas
vector<Carta> generarMazo(int numCartas) {
    vector<Carta> mazo;
    for (int i = 0; i < numCartas / 54; ++i) {
        for (int palo = corazones; palo <= diamantes; ++palo) {
            for (int valor = 1; valor <= 13; ++valor) {
                mazo.push_back({valor, static_cast<Palo>(palo)});
            }
        }
        // Añadir comodines
        mazo.push_back({14, comodines});
        mazo.push_back({14, comodines});
    }
    return mazo;
}

// Función de comparación para ordenar
bool compararCartas(const Carta& a, const Carta& b) {
    if (a.palo == b.palo) {
        return a.valor < b.valor;
    }
    return a.palo < b.palo;
}

int main() {
    auto mazo = generarMazo(4000000);

    // Barajar el mazo
    shuffle(mazo.begin(), mazo.end(), mt19937{random_device{}()});

    // Medir el tiempo de ordenación
    auto inicio = chrono::high_resolution_clock::now();
    sort(mazo.begin(), mazo.end(), compararCartas);
    auto fin = chrono::high_resolution_clock::now();

    cout << "Tiempo para ordenación secuencial: "
         << chrono::duration_cast<chrono::milliseconds>(fin - inicio).count()
         << " ms\n";

    return 0;
}
