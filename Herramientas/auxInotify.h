//-------auxNotify.h-------

#ifndef AUXINOTIFY_H_
#define AUXINOTIFY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/inotify.h>
#include <errno.h>
#include <commons/string.h>
#include "logging.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 64 )
#define BUF_LEN       ( 1024 * ( EVENT_SIZE ) )

int watch_descriptor;
char buffer[BUF_LEN];

int inicializar_estructuras_inotify();

int abrir_fd_inotify();
int watch_dir(int fd, const char *dirname, unsigned long watchFlags);
int read_events (int fd);
void event_handler(int length, void (*actualizar_config)(void));

#endif /* AUXINOTIFY_H_ */
