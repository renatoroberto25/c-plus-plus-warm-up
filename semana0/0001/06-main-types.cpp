#include <iostream>
using namespace std;
//g++ 05-main.cpp -o 05-mai
int main()
{
    cout << "Escolha o tipo para ver o tamanho:\n";
    cout << "1 - int\n2 - char\n3 - float\n4 - double\n5 - todos\n";
    cout << "###############################################\n";
    int escolha;
    cin >> escolha;

    switch (escolha) {
        case 1:
            cout << "Size of int is: " << sizeof(int) << " bytes\n";
            break;
        case 2:
            cout << "Size of char is: " << sizeof(char) << " bytes\n";
            break;
        case 3:
            cout << "Size of float is: " << sizeof(float) << " bytes\n";
            break;
        case 4:
            cout << "Size of double is: " << sizeof(double) << " bytes\n";
            break;
        case 5:
            cout << "Size of int is: " << sizeof(int) << " bytes\n";
            cout << "Size of char is: " << sizeof(char) << " bytes\n";
            cout << "Size of float is: " << sizeof(float) << " bytes\n";
            cout << "Size of double is: " << sizeof(double) << " bytes\n";
            break;
        default:
            cout << "Opção inválida!\n";
    }
    return 0;
}