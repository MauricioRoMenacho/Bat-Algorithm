#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;


class Mochila {
private:
    int numObjetos;
    int capacidadMax;
    vector<int> valores;
    vector<int> pesos;

public:
   
    Mochila(int n, int cap, const vector<int>& vals, const vector<int>& ps) 
        : numObjetos(n), capacidadMax(cap), valores(vals), pesos(ps) {}

    // Getters
    int getNumObjetos() const { return numObjetos; }
    int getCapacidadMax() const { return capacidadMax; }
    int getValor(int i) const { return valores[i]; }
    int getPeso(int i) const { return pesos[i]; }

   
    double calcularFitness(const vector<int>& solucion) const {
        int pesoTotal = 0;
        int valorTotal = 0;

        for (int i = 0; i < numObjetos; i++) {
            pesoTotal += pesos[i] * solucion[i];
            valorTotal += valores[i] * solucion[i];
        }

       
        if (pesoTotal > capacidadMax)
            return 0.0;

        return valorTotal;
    }

    // Calcular peso total de una solución
    int calcularPesoTotal(const vector<int>& solucion) const {
        int pesoTotal = 0;
        for (int i = 0; i < numObjetos; i++) {
            pesoTotal += pesos[i] * solucion[i];
        }
        return pesoTotal;
    }

    
    int calcularValorTotal(const vector<int>& solucion) const {
        int valorTotal = 0;
        for (int i = 0; i < numObjetos; i++) {
            valorTotal += valores[i] * solucion[i];
        }
        return valorTotal;
    }

 
    bool esFactible(const vector<int>& solucion) const {
        return calcularPesoTotal(solucion) <= capacidadMax;
    }

   
    void mostrarInfo() const {
        cout << "=== INFORMACIÓN DE LA MOCHILA ===" << endl;
        cout << "Número de objetos: " << numObjetos << endl;
        cout << "Capacidad maxima: " << capacidadMax << endl;
        cout << "\nObjeto | Valor | Peso" << endl;
        cout << "-------|-------|------" << endl;
        for (int i = 0; i < numObjetos; i++) {
            cout << "   " << i + 1 << "   |  " << valores[i] << "    |  " << pesos[i] << endl;
        }
        cout << "==================================" << endl;
    }
};


class Murcielago {
private:
    vector<double> posicion;
    vector<double> velocidad;
    double frecuencia;
    double loudness;
    double pulseRate;
    double fitness;
    int dimension;

public:

    Murcielago(int dim) : dimension(dim) {
        posicion.resize(dim);
        velocidad.resize(dim);
        frecuencia = 0.0;
        loudness = 1.0;
        pulseRate = 0.5;
        fitness = 0.0;

        for (int i = 0; i < dim; i++) {
            posicion[i] = rand01();
            velocidad[i] = 0.0;
        }
    }


    static double rand01() {
        return (double)rand() / RAND_MAX;
    }

    static int binarizar(double valor) {
        return (rand01() < 1.0 / (1.0 + exp(-10 * (valor - 0.5)))) ? 1 : 0;
    }

    // Getters
    const vector<double>& getPosicion() const { return posicion; }
    double getFrecuencia() const { return frecuencia; }
    double getLoudness() const { return loudness; }
    double getPulseRate() const { return pulseRate; }
    double getFitness() const { return fitness; }

    // Setters
    void setFrecuencia(double f) { frecuencia = f; }
    void setLoudness(double l) { loudness = l; }
    void setPulseRate(double pr) { pulseRate = pr; }
    void setFitness(double f) { fitness = f; }
    void setPosicion(const vector<double>& pos) { posicion = pos; }

    // Actualizar velocidad
    void actualizarVelocidad(const vector<double>& mejorPosicion) {
        for (int i = 0; i < dimension; i++) {
            velocidad[i] += (posicion[i] - mejorPosicion[i]) * frecuencia;
        }
    }

    // Actualizar posición
    void actualizarPosicion() {
        for (int i = 0; i < dimension; i++) {
            posicion[i] += velocidad[i];

            // Limitar a [0, 1]
            if (posicion[i] < 0.0) posicion[i] = 0.0;
            if (posicion[i] > 1.0) posicion[i] = 1.0;
        }
    }

    
    void busquedaLocal(const vector<double>& mejorPosicion) {
        for (int i = 0; i < dimension; i++) {
            posicion[i] = mejorPosicion[i] + 0.001 * rand01();

       
            if (posicion[i] < 0.0) posicion[i] = 0.0;
            if (posicion[i] > 1.0) posicion[i] = 1.0;
        }
    }

    
    vector<int> obtenerSolucionBinaria() const {
        vector<int> solucion(dimension);
        for (int i = 0; i < dimension; i++) {
            solucion[i] = binarizar(posicion[i]);
        }
        return solucion;
    }

    
    void actualizarParametros(double alpha, double gamma, int iteracion) {
        loudness *= alpha;
        pulseRate = pulseRate * (1.0 - exp(-gamma * iteracion));
    }
};


class AlgoritmoMurcielago {
private:
    int numMurcielagos;
    int maxIteraciones;
    double freqMin;
    double freqMax;
    double alpha;
    double gamma;
    vector<Murcielago> poblacion;
    Mochila* problema;
    vector<double> mejorPosicion;
    double mejorFitness;

public:

    AlgoritmoMurcielago(int numMurc, int maxIter, Mochila* prob)
        : numMurcielagos(numMurc), maxIteraciones(maxIter), problema(prob) {
        
        freqMin = 0.0;
        freqMax = 2.0;
        alpha = 0.9;
        gamma = 0.9;
        mejorFitness = 0.0;

        // Inicializar población de murciélagos
        for (int i = 0; i < numMurcielagos; i++) {
            Murcielago murc(problema->getNumObjetos());
            poblacion.push_back(murc);
        }

        mejorPosicion.resize(problema->getNumObjetos(), 0.0);
    }


    void inicializar() {
        mejorFitness = 0.0;


        for (int i = 0; i < numMurcielagos; i++) {
            vector<int> solucion = poblacion[i].obtenerSolucionBinaria();
            double fitness = problema->calcularFitness(solucion);
            poblacion[i].setFitness(fitness);

            // Actualizar mejor solución
            if (fitness > mejorFitness) {
                mejorFitness = fitness;
                mejorPosicion = poblacion[i].getPosicion();
            }
        }
    }

    // Ejecutar el algoritmo
    void ejecutar() {
        cout << "\n===== PROGRESO DE ITERACIONES =====" << endl;

        for (int t = 0; t < maxIteraciones; t++) {
            for (int i = 0; i < numMurcielagos; i++) {
                // Generar nueva frecuencia
                double freq = freqMin + (freqMax - freqMin) * Murcielago::rand01();
                poblacion[i].setFrecuencia(freq);

                // Actualizar velocidad y posición
                poblacion[i].actualizarVelocidad(mejorPosicion);
                poblacion[i].actualizarPosicion();

                if (Murcielago::rand01() > poblacion[i].getPulseRate()) {
                    poblacion[i].busquedaLocal(mejorPosicion);
                }

                // Evaluar nueva solución
                vector<int> nuevaSolucion = poblacion[i].obtenerSolucionBinaria();
                double nuevoFitness = problema->calcularFitness(nuevaSolucion);

                // Aceptar nueva solución si es mejor
                if (nuevoFitness >= poblacion[i].getFitness() && 
                    Murcielago::rand01() < poblacion[i].getLoudness()) {
                    
                    poblacion[i].setFitness(nuevoFitness);
                    poblacion[i].actualizarParametros(alpha, gamma, t);
                }

                if (nuevoFitness > mejorFitness) {
                    mejorFitness = nuevoFitness;
                    mejorPosicion = poblacion[i].getPosicion();
                }
            }

            mostrarProgreso(t + 1);
        }
    }

    void mostrarProgreso(int iteracion) const {
        Murcielago temp(problema->getNumObjetos());
        temp.setPosicion(mejorPosicion);
        vector<int> solucion = temp.obtenerSolucionBinaria();
        int peso = problema->calcularPesoTotal(solucion);

        cout << "Iteracion " << iteracion 
             << " | Mejor valor = " << mejorFitness 
             << " | Peso = " << peso << endl;
    }

    // Obtener mejor solución encontrada
    vector<int> obtenerMejorSolucion() const {
        Murcielago temp(problema->getNumObjetos());
        temp.setPosicion(mejorPosicion);
        return temp.obtenerSolucionBinaria();
    }

    // Mostrar resultados finales
    void mostrarResultados() const {
        vector<int> mejorSolucion = obtenerMejorSolucion();
        int pesoTotal = problema->calcularPesoTotal(mejorSolucion);
        int valorTotal = problema->calcularValorTotal(mejorSolucion);

        cout << "\n===== RESULTADOS FINALES =====" << endl;
        cout << "Objetos seleccionados: ";
        for (int i = 0; i < mejorSolucion.size(); i++) {
            cout << mejorSolucion[i] << " ";
        }
        cout << "\nPeso total: " << pesoTotal;
        cout << "\nValor total: " << valorTotal;
        cout << "\nFitness (valor máximo): " << mejorFitness << endl;
        cout << "Factible: " << (problema->esFactible(mejorSolucion) ? "Si" : "No") << endl;
        cout << "===================================" << endl;
    }
};

// =============================================================================
// 🔹 PROGRAMA PRINCIPAL
// =============================================================================
int main() {
    srand(time(0));

    const int NUM_OBJETOS = 10;
    const int CAPACIDAD_MAX = 35;
    
    vector<int> valores = {10, 5, 15, 7, 6, 18, 3, 12, 8, 9};
    vector<int> pesos   = {2, 5, 7, 1, 4, 6, 3, 6, 3, 2};

    Mochila mochila(NUM_OBJETOS, CAPACIDAD_MAX, valores, pesos);
    mochila.mostrarInfo();

    // Configurar algoritmo
    const int NUM_MURCIELAGOS = 20;
    const int MAX_ITERACIONES = 100;

    
    AlgoritmoMurcielago algoritmo(NUM_MURCIELAGOS, MAX_ITERACIONES, &mochila);
    
    algoritmo.inicializar();
    algoritmo.ejecutar();
    
    // Mostrar resultados
    algoritmo.mostrarResultados();

    return 0;
}