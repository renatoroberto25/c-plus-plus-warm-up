// C++ program to swap two
// numbers using 3rd variable
#include <bits/stdc++.h>
using namespace std;
// Driver code
int main()
{
    int a = 50, b = 100;
    cout << "Antes de trocar a = " << a << " , b = " << b << endl;
    // pede um valor ao usuário
    cout << "Digite um valor inteiro para 'temp': \n";
    int temp;
    cin >> temp;
    // lógica alterada: a recebe temp, b recebe a, temp recebe b
    a = a + temp;
    b = b -  temp;
    cout << "Depois da troca , a = " << a << " , b = " << b << endl;
    return 0;
}