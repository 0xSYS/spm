#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <cJSON/cJSON.h>



//#define DEV_TEST



#include "dbg.h"
#include "server.h"
#include "proc_manager.h"
#include "power_funcs.h"
#include "sckt_io.h"
#include "config.h"

#ifdef DEV_TEST
  #include "tests.h"
#endif








char *dir_path = NULL;




void ServerSetup()
{ 
  // Constructing the path to .spm (home/user/.spm/)
  char *home_dir = getenv("HOME");
  struct stat dir_stat;
  
  if(!home_dir)
  {
    Log(Err, "HOME unset");
  }
  else
  {
    size_t len = strlen(home_dir) + strlen("/.spm") + 1;
    dir_path = malloc(len);
    
    if(!dir_path)
    {
      perror("malloc");
    }
    else
    {
      strcpy(dir_path, home_dir);
      strcat(dir_path, "/.spm");
    }
  
    // Create the .spm directory
    // Check if .spm exists:
    if(stat(dir_path, &dir_stat) != 0)
    {
      Log(Info, "No %s directory.", dir_path);
      mkdir(dir_path, 0777);
    }
    else
    {
      Log(Info, ".spm found");
    }
    
    
    strcat(dir_path, "/server.json");
    // Check for server.json
    if(stat(dir_path, &dir_stat) != 0)
    {
      Log(Info, "No server config found.");
      // Create server config
      default_conf.allow_sys_info = true;
      default_conf.dbg_log = true;
      default_conf.write_log_file = false;
      default_conf.skip_proc_scan = false;
      default_conf.terminate_processes = true;
      default_conf.stdout_capture = false;
      default_conf.socket_response = true;
      default_conf.port = DEFAULT_PORT;
      
      WriteConfig(default_conf);
      
      current_conf = default_conf; // Apply default config to the current config
    }
    else
    {
      Log(Info, "Reading server config...");
      // Parse the server config
      current_conf = ReadConfig();
      
      PrintConfig(current_conf);
    }
  }
  
}



int main(int argc, char * argv[])
{
#ifdef DEV_TEST
  DevTests();
#else
  ServerSetup();
  StartScktReception();
#endif
	return 0;
}
