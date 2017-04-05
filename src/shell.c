#include "../include/shell.h"

long regsize;                     /* Tamaño del objeto de memoria compartida */
void* mem_ptr;                    /* Apuntador a la memoria compartida */
const char* fname = "shmem_obj";  /* Nombre del objeto de memoria compartida */

/* Reserva un espacio de memoria compartida */
int create_sh_mem (void** ptr)
/* {{{ */
{
  int fd;
  shm_unlink(fname);              /* Será removido después  */
  /* Crea o abre un nuevo objeto de memoria compartida */
  fd = shm_open(fname, O_CREAT | O_TRUNC | O_RDWR, 666);
  if (fd < 0)
    return -1;
  /* Establece el tamaño de memoria */
  regsize = sysconf(_SC_PAGE_SIZE);
  if (ftruncate(fd, regsize) < 0)
    return -1;
  /* Mapea el archivo de memoria compartida a la memoria principal */
  *ptr = mmap(0, regsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (*ptr == MAP_FAILED)
    return -1;
  /* Cierra el archivo del objeto de memoria */
  close(fd);
  return 0;
}
/* }}} */

/* Envía una señal de terminación al proceso padre */
int free_sh_mem (void** ptr)
/* {{{ */
{
  /* Se libera el recurso de memoria */
  if (munmap(*ptr, regsize) != 0)
    return -1;
  if (shm_unlink(fname) != 0)
    return -1;
  /* Envía la señal de terminación  */
  /* if (kill (getppid(), SIGTERM) != 0) */
  /*   return -1; */

  return 0;
}
/* }}} */

/* Recibe los comandos desde la entrada estándard */
int get_command_line (char*** buff)
/* {{{ */
{
  char* input = NULL;             /* Apuntador a la cadena del comando de entrada */
  size_t size;                    /* Tamaño del búfer para la cadena de entrada */

  if (getline (&input, &size, stdin) < 1)
    return -1;

  /* Compara la cadena de entrada con el comando de salida */
  if (strncmp (input, "exit", 4) == 0)
    return -1;

  /* Separa la cadena de comandos por cada pipe '|' */
  if (break_string (buff, input, "|") < 0)
    return -1;

  return 0;
}
/* }}} */

/* Ejecuta cada comando 'cmd' guardado dentro del búfer 'buff' */
int exec_command_line (char** buff)
/* {{{ */
{
  int child_status;               /* Estado del proceso hijo */
  int pipefd [2];                 /* Descriptores del pipe */
  char** ptr;                     /* Apuntador al comando actual dentro del búfer */
  char** cmd;                     /* Apuntador al comando actual con sus argumentos */
  pid_t child;

  /* Inicializa los descriptores del pipe */
  if (pipe (pipefd) == -1)
    exit (EXIT_FAILURE);

  ptr = buff;
  while (*ptr != NULL)
    {
      /* Ejecuta el programa dentro de otro proceso */
      if ( (child = fork ()) < 0 )
        return EXIT_FAILURE;
      if (child == 0)
        {
          /* {{{ */
          /* Rompe la cadena por espacios o tabuladores */
          /* {{{ */
          cmd = NULL;
          if (break_string (&cmd, *ptr, " \t\n") < 0)
            return EXIT_FAILURE;
          /* }}} */
          /* Para el comando de salida se manda una señal de terminación */
          /* {{{ */
          if (strncmp (*cmd, "exit", 4) == 0)
            {
              free (cmd);
              shell_exit ();
              exit (EXIT_SUCCESS);
            }
          /* }}} */
          /* Ejecuta el comando actual */
          /* {{{ */
          close (pipefd [0]);
          dup2 (pipefd [1], STDOUT_FILENO);
          close (pipefd [1]);
          execvp (*cmd, cmd);

          fprintf (stderr, "Comando desconocido\n");
          exit (127);
          /* }}} */
          /* }}} */
        }
      else
        {
          /* {{{ */
          /* El proceso padre espera a que terminen los procesos hijo */
          /* {{{ */
          waitpid (child, &child_status, 0);
          close (pipefd [1]);
          dup2 (pipefd [0], STDIN_FILENO);
          /* close (pipefd [0]); */
          /* }}} */
          /* Avanza al siguiente comando dentro del búfer */
          ptr ++;
          /* }}} */
        }
    }
  char buffer;
  while (read (pipefd [0], &buffer, 1) > 0)
    write (STDOUT_FILENO, &buffer, 1);

  return 0;
}
/* }}} */

/* Rompe una cadena 'str' dependiendo del separador 'sep' */
int break_string (char*** buff, char* str, char* sep)
/* {{{ */
{
  char* token;                    /* Guarda el token actual */
  char** ptr;                     /* Apuntador al token actual dentro del búfer */
  size_t len;                     /* Tamaño del token actual */

  /* Toma el primer token */
  token = strtok (str, sep);
  len = strlen (token);

  /* Copia el token dentro del búfer */
  *buff = (char**) malloc (sizeof (char*));
  **buff = (char*) malloc (len);
  strncpy (**buff, token, len);

  /* Toma y guarda los siguientes tokens */
  int i = 1;
  while ( (token = strtok (NULL, sep)) != NULL )
    {
      ptr = (char**) realloc (*buff, ++i * sizeof (char*));
      if (ptr == NULL)
        return -1;
      *buff = ptr;
      ptr += (i-1);

      /* Guarda el siguiente token dentro del búfer */
      len = strlen (token);
      *ptr = (char*) malloc (len);
      strncpy (*ptr, token, len);
    }

  /* Termina con un apuntador a NULL */
  ptr = (char**) realloc (*buff, ++i * sizeof (char*));
  if (ptr == NULL)
    return -1;
  *buff = ptr;
  ptr += (i-1);
  *ptr = NULL;

  return 0;
}
/* }}} */

/* Envía una señal de terminación al proceso actual */
void shell_exit ()
/* {{{ */
{
  if (kill (getppid(), SIGTERM) != 0)
    exit (EXIT_FAILURE);
}
/* }}} */
