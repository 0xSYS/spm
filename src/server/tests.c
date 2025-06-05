#include <cJSON/cJSON.h>
#include <stdio.h>
#include <stdlib.h>


#include "dbg.h"



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

void TestingStuff()
{
  // sdfadsfd
  Log(Info, "Info Log");
  Log(Success, "Success Log");
  Log(Warn, "Warn Log");
  Log(Err, "Error Log");
}

void DevTests()
{
  // Doing stuff here
}