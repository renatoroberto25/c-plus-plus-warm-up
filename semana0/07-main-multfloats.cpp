// C++ program to multiply two
// floating point numbers
#include <iostream>
using namespace std;

// Creating a user-defined function
// called mul_floatnumbers that
// multiplies the numbers passed to
// it as an input. It gives you the
// product of these numbers.
float mul_floatnumbers(float a, float b) { return a * b; }

// Driver code
int main()
{
    float A, B, product;

    // Solicita ao usuário os valores
    cout << "Digite o valor de A: ";
    cin >> A;
    cout << "Digite o valor de B: ";
    cin >> B;
    cout << "###############################################\n";
    // Chama a função
    product = mul_floatnumbers(A, B);

    // Imprime o resultado
    cout << "Produto: " << product << endl;

    return 0;
}