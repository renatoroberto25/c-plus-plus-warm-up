// C++ program to find area
// and perimeter of rectangle
#include<iostream>
#include <cmath>
using namespace std;
//g++ 12-main-retangle.cpp -o 12-main
// Utility function
// Função para calcular área do retângulo
int areaRectangle(int a, int b) {
    return a * b;
}
// Função para calcular perímetro do retângulo
int perimeterRectangle(int a, int b) {
    return 2 * (a + b);
}
// Função para calcular a hipotenusa
double hypotenuseRectangle(int a, int b) {
    return sqrt(a * a + b * b);
}

int main() {
    int a, b;
    cout << "Digite o comprimento do retângulo: ";
    cin >> a;
    cout << "Digite a largura do retângulo: ";
    cin >> b;
    cout << "Área = " << areaRectangle(a, b) << endl;
    cout << "Perímetro = " << perimeterRectangle(a, b) << endl;
    cout << "Hipotenusa = " << hypotenuseRectangle(a, b) << endl;
    return 0;
}