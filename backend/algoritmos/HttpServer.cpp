#include "HttpServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define close closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

HttpServer::HttpServer(int puerto) : puerto(puerto) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

HttpServer::~HttpServer() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void HttpServer::iniciar() {
    std::cout << "Iniciando servidor HTTP en puerto " << puerto << std::endl;
    
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error creando socket" << std::endl;
        return;
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
        std::cerr << "Error configurando socket" << std::endl;
        return;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puerto);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Error bindeando socket" << std::endl;
        return;
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Error escuchando conexiones" << std::endl;
        return;
    }
    
    std::cout << "Servidor iniciado. Accede a http://localhost:" << puerto << std::endl;
    
    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Error aceptando conexión" << std::endl;
            continue;
        }
        
        // Handle request in separate thread
        std::thread([this, new_socket]() {
            char buffer[1024] = {0};
            read(new_socket, buffer, 1024);
            
            std::string request(buffer);
            std::stringstream ss(request);
            std::string metodo, path, version;
            ss >> metodo >> path >> version;
            
            // Find body
            std::string body;
            size_t bodyStart = request.find("\r\n\r\n");
            if (bodyStart != std::string::npos) {
                body = request.substr(bodyStart + 4);
            }
            
            std::string response = procesarRequest(metodo, path, body);
            
            send(new_socket, response.c_str(), response.length(), 0);
            close(new_socket);
        }).detach();
    }
}

std::string HttpServer::procesarRequest(const std::string& metodo, const std::string& path, const std::string& body) {
    std::string response;
    
    if (path == "/" || path == "/index.html") {
        response = servirArchivo("../frontend/index.html");
    } else if (path == "/style.css") {
        response = servirArchivo("../frontend/style.css");
    } else if (path == "/script.js") {
        response = servirArchivo("../frontend/script.js");
    } else if (path == "/api/grid") {
        if (metodo == "GET") {
            response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n" + visualizador.getGridJSON();
        } else if (metodo == "POST") {
            // Handle grid updates
            response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"ok\"}";
        }
    } else if (path == "/api/obstacle") {
        // Parse coordinates from body
        int x = 0, y = 0;
        if (body.find("x=") != std::string::npos && body.find("y=") != std::string::npos) {
            // Simple parsing - in real implementation use proper JSON parsing
            size_t xPos = body.find("x=");
            size_t yPos = body.find("y=");
            x = std::stoi(body.substr(xPos + 2, body.find("&", xPos) - xPos - 2));
            y = std::stoi(body.substr(yPos + 2));
        }
        visualizador.toggleObstacle(x, y);
        response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"ok\"}";
    } else if (path == "/api/start") {
        int x = 0, y = 0;
        if (body.find("x=") != std::string::npos && body.find("y=") != std::string::npos) {
            size_t xPos = body.find("x=");
            size_t yPos = body.find("y=");
            x = std::stoi(body.substr(xPos + 2, body.find("&", xPos) - xPos - 2));
            y = std::stoi(body.substr(yPos + 2));
        }
        visualizador.setStart(x, y);
        response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"ok\"}";
    } else if (path == "/api/goal") {
        int x = 0, y = 0;
        if (body.find("x=") != std::string::npos && body.find("y=") != std::string::npos) {
            size_t xPos = body.find("x=");
            size_t yPos = body.find("y=");
            x = std::stoi(body.substr(xPos + 2, body.find("&", xPos) - xPos - 2));
            y = std::stoi(body.substr(yPos + 2));
        }
        visualizador.setGoal(x, y);
        response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"ok\"}";
    } else if (path == "/api/algorithm") {
        if (metodo == "POST") {
            std::string algoritmo = "A*";
            if (body.find("algorithm=") != std::string::npos) {
                size_t pos = body.find("algorithm=");
                algoritmo = body.substr(pos + 10);
            }
            visualizador.ejecutarAlgoritmo(algoritmo);
            response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"started\"}";
        }
    } else if (path == "/api/clear") {
        visualizador.clearPath();
        response = "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + "\r\n{\"status\":\"ok\"}";
    } else {
        response = "HTTP/1.1 404 Not Found\r\n" + getCORSHeaders() + "\r\n{\"error\":\"Not found\"}";
    }
    
    return response;
}

std::string HttpServer::servirArchivo(const std::string& archivo) {
    std::ifstream file(archivo);
    if (!file.is_open()) {
        return "HTTP/1.1 404 Not Found\r\n" + getCORSHeaders() + "\r\nFile not found";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    std::string contentType = "text/html";
    if (archivo.find(".css") != std::string::npos) {
        contentType = "text/css";
    } else if (archivo.find(".js") != std::string::npos) {
        contentType = "application/javascript";
    }
    
    return "HTTP/1.1 200 OK\r\n" + getCORSHeaders() + 
           "Content-Type: " + contentType + "\r\n" +
           "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
}

std::string HttpServer::getCORSHeaders() {
    return "Access-Control-Allow-Origin: *\r\n"
           "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
           "Access-Control-Allow-Headers: Content-Type\r\n";
}
