#pragma once



#include <string>




class SPMUtils
{
	public:
	 // static std::string homeDir; // Deprecated
	static std::string GetCurrentDate();
  static std::string GetHomeDir();
  static void makeDir(std::string d);
  static bool checkDir(std::string d);
  static bool checkFile(std::string f);
  static bool ping(const std::string & ip);
#if defined(_WIN32) || defined(_WIN64)
  void SetWinTerm();
#endif
	
};
