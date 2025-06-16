#include <stdlib.h>
#include <stdio.h>
#include <cJSON/cJSON.h>



#include "config.h"
#include "server.h"
#include "dbg.h"






config default_conf;
config current_conf;


config ReadConfig()
{
  config res_conf;
  FILE * in_file = fopen(dir_path, "r");
  
  if(!in_file)
  {
    Log(Err, "Failed to open file for reading");
  }
  else
  {
    fseek(in_file, 0, SEEK_END);
    long len = ftell(in_file);
    rewind(in_file);
    
    char *in_conf_str = (char *)malloc(len + 1);
    fread(in_conf_str, 1, len, in_file);
    in_conf_str[len] = '\0';
    fclose(in_file);
    
    
    cJSON *root = cJSON_Parse(in_conf_str);
    
    if(!root)
    {
      Log(Err, "Error before %s", cJSON_GetErrorPtr());
      free(in_conf_str);
    }
    else
    {
      const cJSON *allow_sys_info = cJSON_GetObjectItemCaseSensitive(root, "allowSystemInfo");
      const cJSON *dbg_log        = cJSON_GetObjectItemCaseSensitive(root, "debugLog");
      const cJSON *write_log_file = cJSON_GetObjectItemCaseSensitive(root, "writeLogFile");
      const cJSON *skip_proc_scan = cJSON_GetObjectItemCaseSensitive(root, "skipProcessScanning");
      const cJSON *term_proc      = cJSON_GetObjectItemCaseSensitive(root, "terminateProcs");
      const cJSON *stdout_capt    = cJSON_GetObjectItemCaseSensitive(root, "stdoutCapture");
      const cJSON *sock_resp      = cJSON_GetObjectItemCaseSensitive(root, "replies");
      const cJSON *port           = cJSON_GetObjectItemCaseSensitive(root, "port");
      
      if(cJSON_IsBool(allow_sys_info))
      {
        res_conf.allow_sys_info = allow_sys_info->valueint;
      }
      
      if(cJSON_IsBool(dbg_log))
      {
        res_conf.dbg_log = dbg_log->valueint;
      }
      
      if(cJSON_IsBool(write_log_file))
      {
        res_conf.write_log_file = write_log_file->valueint;
      }
      
      if(cJSON_IsBool(skip_proc_scan))
      {
        res_conf.skip_proc_scan = skip_proc_scan->valueint;
      }
      
      if(cJSON_IsBool(term_proc))
      {
        res_conf.terminate_processes = term_proc->valueint;
      }
      
      if(cJSON_IsBool(stdout_capt))
      {
        res_conf.stdout_capture = term_proc->valueint;
      }
      
      if(cJSON_IsBool(sock_resp))
      {
        res_conf.socket_response = sock_resp->valueint;
      }
      
      if(cJSON_IsNumber(port))
      {
        res_conf.port = port->valueint;
      }
      
      cJSON_Delete(root);
      free(in_conf_str);
    }
  }
	return res_conf;
}

void WriteConfig(config cfg)
{
  Log(Info, "Writing config");
	cJSON *root = cJSON_CreateObject();
	
	// Constructing a simple json structure
  cJSON_AddBoolToObject(root, "allowSystemInfo",     cfg.allow_sys_info);
  cJSON_AddBoolToObject(root, "debugLog",            cfg.dbg_log);
  cJSON_AddBoolToObject(root, "writeLogFile",        cfg.write_log_file);
  cJSON_AddBoolToObject(root, "skipProcessScanning", cfg.skip_proc_scan);
  cJSON_AddBoolToObject(root, "terminateProcs",      cfg.terminate_processes);
  cJSON_AddBoolToObject(root, "stdoutCapture",       cfg.stdout_capture);
  cJSON_AddBoolToObject(root, "replies",             cfg.socket_response);
  cJSON_AddNumberToObject(root, "port",              cfg.port);
  
  char *json_string = cJSON_Print(root);
  
  FILE *out_file = fopen(dir_path, "w");
  
  if(out_file)
  {
    fputs(json_string, out_file);
    fclose(out_file);
  }
  else
    Log(Err, "Failed to open %s for writing !!!", dir_path);
}

void PrintConfig(config cfg)
{
  printf("[BOOL] - allow_sys_info:      %d\n", cfg.allow_sys_info);
  printf("[BOOL] - dbg_log:             %d\n", cfg.dbg_log);
  printf("[BOOL] - write_log_file:      %d\n", cfg.write_log_file);
  printf("[BOOL] - skip_proc_scan:      %d\n", cfg.skip_proc_scan);
  printf("[BOOL] - terminate_processes: %d\n", cfg.terminate_processes);
  printf("[BOOL] - stdout_capture:      %d\n", cfg.stdout_capture);
  printf("[BOOL] - replies:             %d\n", cfg.socket_response);
  printf("[INT]  - port:                %d\n", cfg.port);
}