#include <iostream>
using namespace std;
// Função para calcular Juros Simples
float calculateSimpleInterest(float P, float T, float R) {
    // Fórmula: juros_simples = (P * T * R) / 100
    return (P * T * R) / 100.0;
}
int main() {
    float principal, time, rate;
    cout << "Digite o valor principal (P): ";
    cin >> principal;
    cout << "Digite o tempo (T): ";
    cin >> time;
    cout << "Digite a taxa de juros (R): ";
    cin >> rate;
    float interest = calculateSimpleInterest(principal, time, rate);
    cout << "O juros simples é: " << interest << endl;
    return 0;
}
