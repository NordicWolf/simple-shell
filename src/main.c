#include "../include/shell.h"

extern void* mem_ptr;           /* Apuntador a la memoria compartida */

int main(int argc, char *argv[])
{
  int child_status;             /* Estado del proceso hijo */
  pid_t child;

  /* Reserva un espacio de memoria compartida */
  if (create_sh_mem (&mem_ptr) < 0)
    exit(EXIT_FAILURE);

  while (1)
    {
      char** buff = NULL;       /* Apuntador a una lista de comandos */

      /* Lee un comando desde la entrada estándard */
      printf ("shell > ");
      if (get_command_line (&buff) < 0)
        break;

      /* Crea el proceso hijo */
      if ( (child = fork ()) < 0 )
        fprintf (stderr, "Imposible crear el proceso hijo\n");
      else
        if (child == 0)
          {
            /* Ejecuta la línea de comando */
            exec_command_line (buff);
            exit (EXIT_SUCCESS);
          }
        else
          {
            /* El proceso padre espera a que termine el proceso hijo */
            waitpid (child, &child_status, 0);
            /* Libera la memoria usada por los comandos */
            free (buff);
          }
    }

  printf ("Listo!\n");
  /* Libera el espacio de memoria compartida */
  return free_sh_mem(&mem_ptr);
}
