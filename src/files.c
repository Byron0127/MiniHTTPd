#include "files.h"
#include "mime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/socket.h>

//en este archivo lo que se desarolla es enviar los archivos de www por los sockets de regreso al cliente cuando lo solicitan  
// otra cosa importante es la seguridad del proyecyo con el realpath paara evitar que algiien robe los secretos de mi entorno  y bloquearlo con error 403 Forbbiden            

int serve_file(int socket_cliente, const char *ruta) { //dos parametros recibidos el socket del cliente es sdecir la coenxion con el navegador juento a la ruta que pide el cliente
    char ruta_local[1024]; // GUARDAMOS EN ESTE ARREGLO LA RUTA del archivo pedido
    
    // si pide la raiz de la pag, le damos el index.html de www
    if (strcmp(ruta, "/") == 0) {
        snprintf(ruta_local, sizeof(ruta_local), "www/index.html");
    } else {
        snprintf(ruta_local, sizeof(ruta_local), "www%s", ruta);
    }
    
    char ruta_resuelta[PATH_MAX]; // para guardar la ruta real del archivo en mi entorno local
    
    // ESTO ES PARA SEGURIDAD (evitar el Directory Traversal que vimos)
    // realpath me da la ruta de verdad para que no me hackeen poniendo "../../../" en la url es decir convierte una ruta relativa en una ruta real absoluta
    if (realpath(ruta_local, ruta_resuelta) == NULL) {
        char *resp = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>Error 404: Archivo no encontrado</h1>";
        send(socket_cliente, resp, strlen(resp), 0);
        return -1;
    }

    // obtner el directorio actual
    char dir_actual[PATH_MAX];
    if (getcwd(dir_actual, sizeof(dir_actual)) == NULL) {
        printf("Error interno: no se pudo obtener el directorio actual\n");
        char *resp = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<h1>500 Error Interno del Servidor</h1>";
        send(socket_cliente, resp, strlen(resp), 0);
        return -1;
    }
    
    char dir_base_www[PATH_MAX + 10]; 
    snprintf(dir_base_www, sizeof(dir_base_www), "%s/www", dir_actual);
    
    // revisar que la ruta si este dentro de la carpeta www/
    // aqui evitamos el ataque conocido como Directory traversal que consiste en evadir la carpeta www
    if (strncmp(ruta_resuelta, dir_base_www, strlen(dir_base_www)) != 0) {
        printf("Ataque evitado, se intento salir de la carpeta www\n");
        char *resp = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n<h1>403 Acceso Prohibido</h1>";
        send(socket_cliente, resp, strlen(resp), 0); //ENVIO EL ERROR 403 FORBIDDEN
        return -1;
    }

    // leer en binario por si son imagenes
    FILE *archivo = fopen(ruta_resuelta, "rb"); //aqui ya abro el archivo para leerlo
    if (archivo == NULL) { //si no se puede abrir el archivo
        char *resp = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>Error 404: Archivo no encontrado</h1>";
        send(socket_cliente, resp, strlen(resp), 0); //ENVIO EL ERROR 404 NOT FOUND
        return -1;
    }

    // ver que tipo de archivo es con la otra funcion que hice
    const char *tipo_mime = get_mime_type(ruta_resuelta); //obtenemos el tipo de archivo con la funcion get_mime_type

    char encabezado[512];
    // armar el encabezado de HTTP seria como el mesero entregando la orden para el cliente luego de su pedido o peticion desde el browser
    snprintf(encabezado, sizeof(encabezado),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n\r\n", tipo_mime); 
             
    send(socket_cliente, encabezado, strlen(encabezado), 0); //ENVIO EL ENCABEZADO

    // mandar el archivo por pedazos (buffer)
    char buffer[1024]; // creamos un buffer para enviar el archivo por pedazos
    size_t bytes_leidos; // numero de bytes leidos
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), archivo)) > 0) { // mientras haya bytes leidos 
        send(socket_cliente, buffer, bytes_leidos, 0); //envio el archivo
    }

    fclose(archivo); // cierro el archivo 
    return 0; // retorno 0
}
