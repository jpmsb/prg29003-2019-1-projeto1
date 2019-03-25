#include <cstdlib>
#include <iostream>
#include "prglib.h"
#include "ClienteHTTP.h"

using namespace std;
using prglib::fila;
using prglib::pilha;

#define DEFAULT_URL "https://wiki.sj.ifsc.edu.br/"

int main(int argc, char** argv) {
    string url; 
    
    if (argc > 1) url = argv[1];
    else url = DEFAULT_URL;
   
    // Cria um objeto ClienteHTTP para o sítio web dado pelo parâmetro URL 
    ClienteHTTP web(url);
 
    // Cria uma fila
    fila<string> links(10000);
    fila<string> sublinks(10000);

    // Enfileira o link inicial do sítio web ("/")
    links.enfileira("/");

    // Enquanto fila não vazia
    while (! links.vazia()){
        // Desenfileira um link
        string link = links.desenfileira();
        // Se link for valido
        if (web.obtemLinks(link, sublinks));
    }
    
    return 0;
}

