#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "VisualizadorServer.h"
#include <string>
#include <map>
#include <functional>

class HttpServer {
private:
    VisualizadorServer visualizador;
    int puerto;
    
public:
    HttpServer(int puerto = 8080);
    ~HttpServer();
    void iniciar();
    void detener();
    
private:
    std::string procesarRequest(const std::string& metodo, const std::string& path, const std::string& body);
    std::string servirArchivo(const std::string& archivo);
    std::string getCORSHeaders();
};

#endif // HTTP_SERVER_H
