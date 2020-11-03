#include "FUNCIONES.h"




int main(int argc, char* argv[]) {


    archivo = fopen("datos.txt", "r+"); 
    // con esto abro el archivo "datos.txt" en la cual hay varios lexemas aser reconocidos
    if (!archivo)
    {
    	// VERIFICO QUE EL ARCHIVO EXISTA 	
        printf("No se pudo abrir el archivo de lectura ");
        return EXIT_FAILURE;
    }

	archivoSalida = fopen("SalidaGenerada.txt", "wb+");

	Objetivo(); /*Este seria el axioma que inicia el programa*/

	fclose(archivo);

	fclose(archivoSalida);
	/*Se procede a cerrar el archivo*/

	return EXIT_SUCCESS;
}



