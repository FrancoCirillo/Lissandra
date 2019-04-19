

#include "fileSystem.h"
#include <sys/stat.h>
FILE* abrirArchivo(char* nombre) {
		return fopen(nombre, "a");
	}
void escribirStringEnArchivo(FILE* archivo, char* bytes) {
	fwrite(bytes, strlen(bytes)+1,1, archivo);
	fflush(archivo);
}
FILE* crearTabla (char* nombre){
	mkdir("table/tableA", S_IRWXU);
	FILE*f = abrirArchivo("Metadata");

}
int main() {
	FILE*f = abrirArchivo("banana.bin");
	escribirStringEnArchivo(f, "lalala");
	escribirStringEnArchivo(f, "lololo");
	mkdir("tables", S_IRWXU);
	FILE* meta = crearTabla("tableA");

}
