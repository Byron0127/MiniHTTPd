#include "mime.h"
#include <string.h>

// Mime permite  saber que tipo de contenido le voy a mandar al navegador
// para que sepa renderizarlo correctamente o si es una imagen un pdf html etc
const char* get_mime_type(const char *nombre_archivo) { // dos parametros recibidos nombre archivo y la ruta local 
    // buscar el punto de la extension en el nome del archivo para sacar al a extension 
    const char *extension = strrchr(nombre_archivo, '.');
    
    if (extension == NULL || extension == nombre_archivo) { // si no se encuentra el punto en el nombre del archivo

        return "text/plain"; // retornamos text/plain
    }
    
    // puro if para ver que tipo es 
    if (strcmp(extension, ".html") == 0) return "text/html"; // comparamos la extension con el string y retorno el tipo mime 
    if (strcmp(extension, ".css") == 0) return "text/css";   // comparamos la extension con el string y retorno el tipo mime 
    if (strcmp(extension, ".js") == 0) return "application/javascript"; // comparamos la extension con el string y retorno el tipo mime 
    if (strcmp(extension, ".png") == 0) return "image/png"; // comparamos la extension con el string y retorno el tipo mime 
    if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) { // comparamos la extension con el string y retorno el tipo mime 
        return "image/jpeg"; 
    }
    
    return "text/plain";
}
