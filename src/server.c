#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//En este archivo crramos la conexion TCP, el servidor para que peuda recibir las solcitiuddes de los clientes desde el navegador
//Se crea el socket prinicpal,main en el pujerto 8080 CREamos el socket Stream para tcp a diferencia del UDP datagrama
int setup_server(int puerto) {
    int socket_servidor;
    struct sockaddr_in direccion_servidor; //declaramos server 
    int error_flag = 0; //por si acaso hay error

    //crear el socket. AF_INET es para ipv4 y SOCK_STREAM para TCP aqui nace el socket main
    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Fallo al crear el socket");
        exit(1);
    }



    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY; // escuchar en cualquier IP de la computadora
    direccion_servidor.sin_port = htons(puerto); // hay que convertir el puerto con htons 

    // hacer bind es el registro de nuestor servidor 
    if (bind(socket_servidor, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Fallo en el bind");
         close(socket_servidor);
        exit(1);
    }

    //Si fuera un restaurante esta seria la puerta para que entren las personas y hagan fila 
    //le puse 10 para que aguante 10 peticiones en cola maximo
    if (listen(socket_servidor, 10) < 0) {
        perror("Fallo en listen");
        close(socket_servidor);
        exit(1);
    }

    printf("Servidor configurado correctamente en el puerto %d\n", puerto); 
    //despues del socket principal daremos al recpecionista o tmbien llaamdo eppol para que pueda revisar los eventos por eso event-driven
    return socket_servidor;
}
