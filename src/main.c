#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "server.h"
#include "http.h"

#define MAX_EVENTOS 10  //eventos definidos en el server.c
#define PUERTO 8080

//seccion prinicpal del programa, aqui corre el blucle infiito en while con eso nunca se apaga y sobre todo 
// epoll es parte de Linux, nos auida a manejar los eventos y avisar al sistema para que haya una concurrencia event-driven
//epoll seria como un recpecionista que vigila a todos los cloientes que se coencte a mi servidor uno por uno pero sabiendo que necesita cada cliente
// epoll es unico es decir no se clona, esta funcion nativa de linux es como un pulpo para ver los eventos en el puerto
int main() {

    printf("Arrancando el servidor web de Byron Ortiz...\n");
    int i_prueba = 0; // variable por si acaso para debuggear
    
    //configuramos el servidor en el puerto 8080 clave
    int socket_servidor = setup_server(PUERTO);

    //Aqui viene epoll quees como el recepcionista del restaurante en nuestorminiserver
    //AQUI LO usamos para manejar por eventos y ser concurrente no bloqueante bsaado en eventos a diferencia de una aruqitecura secuencial completamente secuencial

    int fd_epoll = epoll_create1(0); //creacion del recepcionistra con descriptod fd
    if (fd_epoll == -1) {
        perror("Error al crear epoll"); 
        exit(1);
    }

    struct epoll_event evento_principal;
    evento_principal.events = EPOLLIN; // EPOLLIN significa que hay datos para leer, es decir una nueva conexion para el server esperando
    evento_principal.data.fd = socket_servidor;

    // agregar el socket principal a epoll para que lo vigile con epoll_ctl contoalmos quue sockets vigila epoll
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, socket_servidor, &evento_principal) == -1) {
        perror("Error agregando a epoll_ctl");
        exit(1);
    }

    struct epoll_event lista_eventos[MAX_EVENTOS];// Cramos el arreglo para guardar los ecentos que ocullar y epoll wait lleen la lista

  printf("Todo listo, esperando conexiones en el puerto %d....\n", PUERTO);

    // bucle infinito
    while (1) { // Mientras el servidor esté prendido, sigue funcionando
        // os quedamos esperando aqui hasta que alguien se conecte o mande datos
        int num_eventos = epoll_wait(fd_epoll, lista_eventos, MAX_EVENTOS, -1); // el corazon del servidor con la linea de eppol_wait para esperar los eventos
        if (num_eventos == -1) { 
            perror("Error en epoll_wait");
            break;
        }

        // recorremos todos los eventos que sucedieron
        for (int i = 0; i < num_eventos; i++) { // recorremos los eventos que detecto epoll_wait
            if (lista_eventos[i].data.fd == socket_servidor) { // Si el evento es el socket principal, significa que hay una nueva conexion esperando
                // nueva conexion entrante
                struct sockaddr_in direccion_cliente;
                socklen_t tamanio_cliente = sizeof(direccion_cliente);
                int socket_cliente = accept(socket_servidor, (struct sockaddr *)&direccion_cliente, &tamanio_cliente); //aceptamos la conexion con el cliente 
                
                if (socket_cliente == -1) {
                    perror("Error aceptando al cliente");
                    continue; 
                }

                printf(">>> Se conecto un nuevo cliente! (socket id: %d)\n", socket_cliente); //puede aver multiples sockets al mismo cliente ya que en el navegador en las paginas siempre se solicigta muchos datos a pesar de en este caso ser contenidos estaticos en www   

                // meter al nuevo cliente en el epoll la sala de espera con el recpecionista epoll
                struct epoll_event evento_cliente;
                evento_cliente.events = EPOLLIN;
                evento_cliente.data.fd = socket_cliente;
                epoll_ctl(fd_epoll, EPOLL_CTL_ADD, socket_cliente, &evento_cliente);

            } else {
                // si no es el socket principal, el cliente ya nos mando su peticion HTTP en version 1.1
                int socket_cliente = lista_eventos[i].data.fd; // si no es el socket principal, el cliente ya nos mando su peticion HTTP
                
                  handle_client(socket_cliente); //mandamos a llamar a la funcion handle_client para que se encargue de manejar la peticion
                
                // despues de responderle con la pagina, lo quitamos y cerramos
                epoll_ctl(fd_epoll, EPOLL_CTL_DEL, socket_cliente, NULL);
                close(socket_cliente);
                printf("<<< Cliente desconectado \n");
            }
        }
    }

    close(socket_servidor);
    close(fd_epoll);
    return 0;
}
