#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char* argv []){

   if (argc!=2)
   {
   	   err(1, "Wrong arg count!");
   }

   int find_sort[2];
   int sort_head[2];

   if (pipe(find_sort)==-1) {err(1, "Could not pipe!");}
   if (pipe(sort_head)==-1) {err(1, "Could not pipe!");}

   pid_t find_pid = fork();
   
   if (find_pid<0)
   {err(1, "Could not fork find!");}

   if (find_pid==0)
   {
   	   close(find_sort[0]);
       close(sort_head[0]);
       close(sort_head[1]);

   	   dup2(find_sort[1], 1);
   	   close(find_sort[1]);

   	   execlp("find", "find", argv[1], "-printf", "%f %T@\n", (char*)NULL);
   	   err(1, "exec(find)");
   }
 
   pid_t sort_pid = fork();

   if (sort_pid<0) { err(1, "Could not fork sort!"); } 

   if (sort_pid==0)
   {
   	   close(find_sort[1]);
       close(sort_head[0]);

   	   dup2(find_sort[0], 0);
   	   dup2(sort_head[1], 1);
   	   close(find_sort[0]);
   	   close(sort_head[1]);

   	   execlp("sort", "sort", "-r", "-t", " ", "-k", "2", "-n", (char*)NULL);
   	   err(1, "exec(sort)");
   }
  
   //Should this one happen before the checks for children's pids, or is this way better?

   close(find_sort[0]);
   close(find_sort[1]);

   close(sort_head[1]);
   
   dup2(sort_head[0], 0);
   close(sort_head[0]);
    

   for (int i=1; i<=2; i++)
   {
   	   int status;
   	   wait(&status);

   	   if (WIFEXITED(status))
	   {
	   	   if (WEXITSTATUS(status)!=0)
		   {
		   	   warnx("Child exited with status %d", WEXITSTATUS(status));
		   }
	   }
	   else
	   {
	   	   warnx("Child has been killed!");
	   }
   }

   execlp("head", "head", "-n", "1", (char*)NULL);  //Do not need redirection here since head prints on standart output on default 

   return 0;
}
