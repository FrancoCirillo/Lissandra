/*
 * auxInotify.c
 *
 *  Created on: 18 jun. 2019
 *      Author: utnso
 */

#include "auxInotify.h"

int inicializar_estructuras_inotify(char* ruta) {

	int fdInotify = abrir_fd_inotify();

	watch_descriptor = watch_dir(fdInotify, ruta , IN_MODIFY);

	return fdInotify;
}

/* Se crea una instancia de inotify, y se abre su fd*/
int abrir_fd_inotify() {
	int fd;

//  watched_items = 0;

	fd = inotify_init();

	if (fd < 0) {
		loggear_error(
				string_from_format("Error al inicializar inotify: %s\n", strerror(errno)));
	}
	return fd;

}

int watch_dir(int fd, const char *dirname, unsigned long watchFlags) {
	int wd;
	wd = inotify_add_watch(fd, dirname, watchFlags);
	if (wd < 0) {
		loggear_error(
				string_from_format("No se pudo agregar %s con los flags %lX al inotify watch: %s\n",dirname, watchFlags, strerror(errno)));
	} else {
		loggear_info(string_from_format("Vigilando la entrada %s WD=%d\n", dirname, wd));
	}
	return wd;
}



int read_events (int fd)
{
	int length = read(fd, buffer, BUF_LEN);
	if (length < 0) {
		string_from_format("Error al leer del fd %d: %s\n", fd, strerror(errno));
	}
	return length;
}


void event_handler(int length){

	int offset = 0;

	// Luego del read buffer es un array de n posiciones donde cada posición contiene
	// un eventos ( inotify_event ) junto con el nombre de este.
	while (offset < length) {

		// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
		// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
		// a sizeof( struct inotify_event ) + 24.
		struct inotify_event *event = (struct inotify_event *) &buffer[offset];

		// El campo "len" nos indica la longitud del tamaño del nombre
		if (event->len) {
			// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
			// sea un archivo o un directorio
			if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					loggear_warning(string_from_format("El directorio '%s' fue modificado", event->name));
				} else if(string_ends_with(event->name, ".config")){
					loggear_info(string_from_format("Archivo de congif '%s' modificado", event->name));
				}
			}
		}
		offset += sizeof (struct inotify_event) + event->len;
	}

//	inotify_rm_watch(fd, watch_descriptor);
//	close(fd);

}

