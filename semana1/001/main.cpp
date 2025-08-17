// vamos fazer os includes ncessários
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
// função para encontrar os elementos no vector que são menores
// que seus vizinhos adjacentes
vector<int> verificar(vector<int> valores) {
  // vamos inicializar um vector vazio para guardar os elementos
  // que passarem na condição
  vector<int> temp;
  // vamos percorrer o vector recebido como argumento
  for(int i = 1; i < valores.size() - 1; i++) {
    // o elemento atual é menor que o elemento anterior e posterior a ele?
    if(valores[i] < valores[i - 1] && valores[i] < valores[i + 1]) {
      // vamos adicionar no vector temporário
      temp.push_back(valores[i]);
    }
  }  
  // retornamos o vector temporário
  return temp;
}
// função principal do programa
int main(int argc, char *argv[]) {
  // vamos criar o vector que guardará os valores informados
  // pelo usuário
  vector<int> numeros;
  int valor;
  // vamos ler 10 números inteiros
  for (int i = 0; i < 10; i++) {
    cout << "Informe o " << (i + 1) << ".o valor: ";
    cin >> valor;
    numeros.push_back(valor);
  }
  // vamos mostrar o conteúdo do vector original
  cout << "\nConteúdo do vector: ";
  for (int n: numeros) {
    cout << n << "  ";
  }
  // agora vamos testar os números que são menores que seus
  // vizinhos adjascentes
  vector<int> resultado = verificar(numeros);
  cout << "\n\nOs elementos menores que seus vizinhos adjascentes são:\n\n";   
  for (int n: resultado) {
    cout << n << "  ";
  }
  cout << "\n\n";
  return EXIT_SUCCESS;
}