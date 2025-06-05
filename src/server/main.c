#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <cJSON/cJSON.h>


#include "dbg.h"
#include "server.h"
#include "proc_manager.h"
#include "power_funcs.h"
#include "sckt_io.h"
#include "config.h"








char *dir_path = NULL;




void TestingStuff()
{
  // sdfadsfd
  Log(Info, "Info Log");
  Log(Success, "Success Log");
  Log(Warn, "Warn Log");
  Log(Err, "Error Log");
}

void jsonTest1()
{
  // Create a JSON object
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "name", "Alice");
  cJSON_AddNumberToObject(root, "age", 30);

  // Print JSON to string
  char *json_str = cJSON_Print(root);

  // Write to file
  FILE *fp = fopen("output.json", "w");
  if (fp) {
      fputs(json_str, fp);
      fclose(fp);
  }

  // Clean up
  free(json_str);
  cJSON_Delete(root);
}

void jsonTest2()
{
  // Read file into buffer
  FILE *fp = fopen("output.json", "r");
  if (!fp) {
      perror("File opening failed");
      //return 1;
  }
  fseek(fp, 0, SEEK_END);
  long len = ftell(fp);
  rewind(fp);

  char *data = (char*)malloc(len + 1);
  fread(data, 1, len, fp);
  data[len] = '\0';
  fclose(fp);

  // Parse JSON
  cJSON *root = cJSON_Parse(data);
  if (!root) {
      printf("Error before: [%s]\n", cJSON_GetErrorPtr());
      free(data);
      //return 1;
  }

  // Access values
  const cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
  const cJSON *age = cJSON_GetObjectItemCaseSensitive(root, "age");
  if (cJSON_IsString(name) && (name->valuestring != NULL)) {
      printf("Name: %s\n", name->valuestring);
  }
  if (cJSON_IsNumber(age)) {
      printf("Age: %d\n", age->valueint);
  }

  // Clean up
  cJSON_Delete(root);
  free(data);
}


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
    // Do stuff here
  //TestingStuff();
  ServerSetup();
  StartScktReception();
  //jsonTest2();
    /*
    CheckRuningProc("helix");
    CheckRuningProc("zsh");
    CheckRuningProc("tmux");
    */
    // SysReboot();
    // SysPowerOff();
	return 0;
}
