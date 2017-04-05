#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* Reserva un espacio de memoria compartida */
int create_sh_mem (void**);

/* Envía una señal de termianción al proceso padre */
int free_sh_mem (void**);

/* Recibe los comandos desde la entrada estándard */
int get_command_line (char***);

/* Ejecuta cada comando guardado entro del búfer */
int exec_command_line (char**);

/* Rompe una cadena dependiendo del separador pasado como argumento */
int break_string (char***, char*, char*);

/* Envía una señal de terminación al proceso actual */
void shell_exit ();
