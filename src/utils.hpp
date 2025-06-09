#pragma once



#include <string>
#include "config.hpp"
#include "spm_list.hpp"




class SPMUtils
{
	public:
	 // static std::string homeDir; // Deprecated
	static std::string GetCurrentDate();
	static std::string GetCurrentTime(); // Todo
  static std::string GetHomeDir();
  static void makeDir(std::string d);
  static int removeDir(std::string d);
  static bool checkDir(std::string d);
  static bool checkFile(std::string f);
  static void printConfig(SPMConfig::cfgStruct c);
  static std::string getBuildConfig(); // Todo
  static void printDevArray(std::vector<SPMList::device> d);
  static char * getStdErr();
  static std::string genRandomHash(size_t len);
#if defined(_WIN32) || defined(_WIN64)
  static void SetWinTerm();
  static std::string GetWinApiErr(DWORD err_code);
#endif
	
};
