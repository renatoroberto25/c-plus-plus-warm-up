// C program to print ASCII Value of Character using
// implicit conversion with format specifier.
#include <iostream>
using namespace std;

int main() {
    char c ;
    cout << "Digite uma letra: ";
    cin >> c;

    // %d displays the integer value of
    // a character
    // %c displays the actual character
    cout << "****************************************\n";
    printf("O valor ascii de %c : %d \n", c, c);
    return 0;
}