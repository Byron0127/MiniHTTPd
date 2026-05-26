#include "http.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void handle_client(int socket_cliente) {
    char buffer[4096]; // arreglo temporal para guardar el texto que manda el navegador
    
    // este es el mesero aqui escuchamos lo que quiere pedir el cliente
    int bytes_leidos = recv(socket_cliente, buffer, sizeof(buffer) - 1, 0); //
    
    if (bytes_leidos <= 0) {
        return; // si no leyo nada o hubo error, salimos sin hacer nada
    }
    
    buffer[bytes_leidos] = '\0'; // le ponemos fin de string para poder usar sscanf despues
    
    char metodo[16], ruta[256], version[16];
    
    // extraemos la primera linea de la peticion HTTP (ej. "GET /index.html HTTP/1.1")
    if (sscanf(buffer, "%15s %255s %15s", metodo, ruta, version) != 3) {
        printf("Peticion mal formada, respondiendo error 400\n");
        char *respuesta_error = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<h1>400 Peticion Invalida</h1>";
        send(socket_cliente, respuesta_error, strlen(respuesta_error), 0);
        return;
    }

    printf("El navegador nos pide: %s %s\n", metodo, ruta);
    
    // verificamos que el metodo sea GET, porque nuestro servidor web es simple y no soporta POST ni otras cosas
    if (strcmp(metodo, "GET") != 0) {
        printf("Me mandaron un metodo que no es GET, respondiendo error 405\n");
        char *respuesta_error = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n<h1>405 Metodo No Permitido</h1>";
        send(socket_cliente, respuesta_error, strlen(respuesta_error), 0);
        return;
    }

    // si paso todas las validaciones, vamos a buscar y enviarle el archivo que pidieron
    serve_file(socket_cliente, ruta);
}
