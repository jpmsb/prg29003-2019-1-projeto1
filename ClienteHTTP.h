/* 
 * File:   Curl.h
 * Author: msobral
 *
 * Created on 2 de Setembro de 2015, 10:57
 */

#ifndef CURL_H
#define	CURL_H

#include <curl/curl.h>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <time.h>
#include <prglib.h>

using namespace std;
using prglib::fila;

// Um objeto da classe ClienteHTTP é responsável por fazer
// requisições para um sítio web específico
// Ele possibilita:
// - listar todos os links existentes em uma determinada página desse sítio
// - obter o conteúdo de uma página (ou recurso)

class ClienteHTTP {
 public:
  // construtores da classe
     
  // cria um objeto ClienteHTTP vinculado a um sítio web descrito pela "url"    
  ClienteHTTP(const string & url);

  // cria um objeto ClienteHTTP vinculado a um sítio web descrito pela "url"
  // modo debug ativado: todas as operações são mostradas na tela
  ClienteHTTP(const string & url, bool debug);

  // cria um objeto ClienteHTTP idêntico a outro a que já existe
  ClienteHTTP(const ClienteHTTP & outro);
  
  // destrutor: destrói um objeto ClienteHTTP
  ~ClienteHTTP();
  
  // Obtém uma listagem dos links existentes na página dado por "uri"
  // A listagem de links é inserida na fila "links"
  // Retorna "true" se o link for válido, e "false" se o link estiver quebrado
  bool obtemLinks(const string & uri, fila<string> & links);
  
  // Obtém o conteúdo da página dada por "uri"
  // O conteúdo da página é gravado na stream "out"
  // Retorna o tipo de conteúdo contido na página na forma
  // de Internet Media Type (ex: "text/html")
  string obtem(const string & uri, ostream & out);
  
  // Testa se "uri" corresponde a um link contido no sítio web 
  // associado a este objeto. Ex: se o sítio for "http://www.ifsc.edu.br",
  // as seguintes uri resultarão em:
  // /index.php --> true
  // http://www.ifsc.edu.br/ingresso --> true
  // http://www.google.com/analytics --> false
  bool link_local(const string & uri);
  
 private:
  set<string> visitados;
  static int count;
  CURL * handle;
  string baseUrl;
  bool debug;
     
  void init(const string & url, bool debug);
  static size_t write_data(void *ptr, size_t size, size_t nmemb, void *arg);
  static size_t get_header(void *ptr, size_t size, size_t nmemb, void *arg);
  string extrai_uri(const string & link);
  bool is_url(const string & link);
  
};


#endif	/* CURL_H */

