#include <iostream>
#include <cmath>
using namespace std;
//g++ 11-main-compound.cpp -o 11-main
int main() {
    double principal, rate, time, amount, compoundInterest;
    char tipoTaxa, tipoTempo;
    cout << "Digite o valor principal: ";
    cin >> principal;
    cout << "A taxa é anual (a) ou mensal (m)? ";
    cin >> tipoTaxa;

    cout << "Digite a taxa de juros (%): ";
    cin >> rate;

    cout << "O tempo é em anos (a) ou meses (m)? ";
    cin >> tipoTempo;

    cout << "Digite o tempo: ";
    cin >> time;

    // Ajusta taxa e tempo para o cálculo correto
    if (tipoTaxa == 'a' && tipoTempo == 'm') {
        rate = rate / 12;
        time = time / 12;
    } else if (tipoTaxa == 'm' && tipoTempo == 'a') {
        time = time * 12;
    }

    amount = principal * pow((1 + rate / 100), time);
    compoundInterest = amount - principal;

    cout << "Juros Compostos = " << compoundInterest << endl;

    return 0;
}