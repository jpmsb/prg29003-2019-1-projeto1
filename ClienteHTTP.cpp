/* 
 * File:   Curl.cpp
 * Author: msobral
 * 
 * Created on 2 de Setembro de 2015, 10:57
 */

#include <pcre++.h>
#include <sstream>
#include "ClienteHTTP.h"

using namespace pcrepp;

int ClienteHTTP::count = 0;
Pcre expr_URL("^[a-zA-Z0-9]{2,6}://");

struct Status {
    string imt;
    int code;
};

ClienteHTTP::ClienteHTTP(const string & url) {
  init(url, false);
}

ClienteHTTP::ClienteHTTP(const string & url, bool debug) {
  init(url, debug);
}

ClienteHTTP::ClienteHTTP(const ClienteHTTP & outro) {
    baseUrl = outro.baseUrl;
    handle = curl_easy_duphandle(outro.handle);
}

void ClienteHTTP::init(const string & url, bool debug) {
  baseUrl = url;
  if (url.back() != '/') baseUrl += '/';

  this->debug = debug;
  if (not count) {
    curl_global_init(CURL_GLOBAL_ALL);
  }
  count ++;
  handle = curl_easy_init();
}

ClienteHTTP::~ClienteHTTP() {
  curl_easy_cleanup(handle);
  count--;
  if (not count) curl_global_cleanup();
}

size_t ClienteHTTP::get_header(void *ptr, size_t size, size_t nmemb,
                             void *arg) {
    static Pcre expr_imt("^ *([-a-zA-Z]+/[-a-zA-Z]+)");
    static Pcre expr_status("^ *HTTP/[012.]+ *([0-9]+)");
    
  Status * algo = static_cast<Status*>(arg);
  string header((char*)ptr, size*nmemb);
  
  //cout << "escrevendo " << size*nmemb << " caracteres" << endl;
  if (expr_status.search(header)) {
      algo->code = stoi(expr_status.get_match(0));
      return size*nmemb;
  }
  
  int pos = header.find(':');
  if (pos == string::npos) return size*nmemb;
  string hname = header.substr(0, pos);
  if (hname == "Content-Type") {
      if (expr_imt.search(header.substr(pos+1))) {
          string & imt = algo->imt;
          
          imt = expr_imt.get_match(0);
          while (isspace(imt.back())) imt.pop_back();
          while (isspace(imt.front())) imt.erase(0,1);
      }
  }
  return size*nmemb;
}

bool ClienteHTTP::link_local(const string& uri) {
    if (! expr_URL.search(uri)) return true;
    return (uri.find(baseUrl) != string::npos);
}

bool ClienteHTTP::obtemLinks(const string & uri, fila<string> & fila) {
  string url = uri;
  
  if (! is_url(url)) {
      if (url[0] == '/') url.erase(0, 1);
      url = baseUrl + url;
  }
  
  // link já visitado
  if (visitados.count(url)) {
      return true;
  }
  
  // obtém página, mas aceita somente texto html
  ostringstream result;
  try {
    if (obtem(url, result) != "text/html") return true;
  } catch (...) {
      // LINK QUEBRADO
      return false;
  }
  
  // marca link como visitado
  visitados.insert(url);
  
  if(! link_local(url)) return true;
  
  Pcre re("<(a|link)[^>]*href=\"?([-~0-9a-zA-Z_./,;@$%!()[\\]{<>+}:]+)\"?");
  string s = result.str();
  
  // lista os links encontrados
  int pos = 0;
  while (pos < s.size()) {
    if (re.search(s, pos)) {
      string algo = re.get_match(1);
      fila.enfileira(algo);
      
      pos = re.get_match_end() + 1;
    } else break;
  }
  
  return true;
}

bool ClienteHTTP::is_url(const string & link) {
  Pcre re("[a-zA-Z0-9]+://[-_.a-zA-Z0-9:@]+");
  
  return re.search(link);
}

string ClienteHTTP::extrai_uri(const string& link) {
  Pcre re("([a-zA-Z0-9]+://[-_.a-zA-Z0-9:@]+)?(/[-~0-9a-zA-Z_./,;@$%!()[\\]{<>+}:]+)");
  string uri = re.get_match(1);
  
  return uri;    
}

size_t ClienteHTTP::write_data(void *ptr, size_t size, size_t nmemb,
                             void *arg) {
  ostream * out = (ostream*)arg;
  char * data = (char*)ptr;
  
  //cout << "escrevendo " << siz1#Revis.C3.A3o_da_lista:_procurae*nmemb << " caracteres" << endl;
  ssize_t pos = out->tellp();
  out->write(data, size*nmemb);
  return out->tellp() - pos;
}

string ClienteHTTP::obtem(const string & URI, ostream & out) {
  string url = URI;
  Status algo;
  
  if (! is_url(url)) {
      if (url[0] == '/') url.erase(0, 1);
      url = baseUrl + url;
  }
  
  curl_easy_reset(handle);
  if (debug) curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ClienteHTTP::write_data);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&out);
  curl_easy_setopt(handle, CURLOPT_HEADERDATA, (void*)&algo);
  curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, ClienteHTTP::get_header);
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);

  if (curl_easy_perform(handle) != CURLE_OK) throw -1; 
  
  if (algo.code < 200 or algo.code >= 400) throw -1; 
  
  return algo.imt; // retornar o IMT correto ...
}
