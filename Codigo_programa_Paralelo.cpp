#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

enum Palo { CORAZONES, TREBOLES, ESPADAS, DIAMANTES, COMODIN };

struct Carta {
    int valor;
    Palo palo;

    bool operator<(const Carta &otra) const {
        if (palo != otra.palo) return palo < otra.palo;
        return valor < otra.valor;
    }
};

vector<Carta> crearMazo(int cantidadCartas) {
    vector<Carta> mazo;
    for (int i = 0; i < cantidadCartas - 2; ++i) {
        mazo.push_back({(i % 13) + 1, static_cast<Palo>((i / (cantidadCartas / 4)) % 4)});
    }
    mazo.push_back({0, COMODIN});
    mazo.push_back({0, COMODIN});
    shuffle(mazo.begin(), mazo.end(), mt19937(random_device{}()));
    return mazo;
}

void fusionar(vector<Carta> &mazo, int izquierda, int medio, int derecha) {
    int n1 = medio - izquierda + 1;
    int n2 = derecha - medio;
    vector<Carta> izquierdaMazo(mazo.begin() + izquierda, mazo.begin() + medio + 1);
    vector<Carta> derechaMazo(mazo.begin() + medio + 1, mazo.begin() + derecha + 1);

    int i = 0, j = 0, k = izquierda;
    while (i < n1 && j < n2) {
        if (izquierdaMazo[i] < derechaMazo[j])
            mazo[k++] = izquierdaMazo[i++];
        else
            mazo[k++] = derechaMazo[j++];
    }
    while (i < n1) mazo[k++] = izquierdaMazo[i++];
    while (j < n2) mazo[k++] = derechaMazo[j++];
}

void mergesort(vector<Carta> &mazo, int izquierda, int derecha, int profundidad, int maxHilos) {
    if (izquierda < derecha) {
        int medio = izquierda + (derecha - izquierda) / 2;
        
        if (profundidad > 0 && maxHilos > 1) { // Solo crear hilos si hay recursos disponibles
            int nuevosHilos = maxHilos / 2;
            thread hiloIzq(mergesort, ref(mazo), izquierda, medio, profundidad - 1, nuevosHilos);
            thread hiloDer(mergesort, ref(mazo), medio + 1, derecha, profundidad - 1, nuevosHilos);
            
            hiloIzq.join();
            hiloDer.join();
        } else {
            mergesort(mazo, izquierda, medio, profundidad - 1, maxHilos);
            mergesort(mazo, medio + 1, derecha, profundidad - 1, maxHilos);
        }
        
        fusionar(mazo, izquierda, medio, derecha);
    }
}

void ordenarParalelo(vector<Carta> &mazo) {
    int maxHilos = min((int)thread::hardware_concurrency(), 8); // Limitar la cantidad de hilos
    mergesort(mazo, 0, mazo.size() - 1, maxHilos, maxHilos);
}

void mostrarMazo(const vector<Carta> &mazo) {
    for (const auto &carta : mazo) {
        cout << "(" << carta.valor << ", " << carta.palo << ") ";
    }
    cout << endl;
}

int main() {
    int cantidadCartas = 4000000;
    vector<Carta> mazo = crearMazo(cantidadCartas);

    auto inicio = chrono::high_resolution_clock::now();
    ordenarParalelo(mazo);
    auto fin = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duracion = fin - inicio;
    cout << "Tiempo de MergeSort Paralelo: " << duracion.count() << " segundos\n";
    return 0;
}
