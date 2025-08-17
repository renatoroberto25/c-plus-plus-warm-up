#include <iostream>
using namespace std;

// Função para converter Celsius para Fahrenheit
float Conversion(float celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

// Código principal
int main() {
    float n;
    cout << "Digite a temperatura em Celsius: ";
    cin >> n;

    float fahrenheit = Conversion(n);
    cout << "Temperatura em Fahrenheit: " << fahrenheit << "F" << endl;

    // Lógica para determinar a sensação térmica
    if (fahrenheit < 60) {
        cout << "Sensação: Está frio." << endl;
    } else if (fahrenheit >= 60 && fahrenheit < 80) {
        cout << "Sensação: Temperatura agradável." << endl;
    } else { // fahrenheit >= 80
        cout << "Sensação: Está quente." << endl;
    }

    return 0;
}