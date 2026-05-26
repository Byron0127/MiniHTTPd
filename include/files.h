#ifndef FILES_H
#define FILES_H
//cabecera que permite encontrar y enviar los archivos de www al cliente
int serve_file(int client_fd, const char *path);

#endif
