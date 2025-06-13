#pragma once

#include <string>
#include <vector>




class SPMDetect
{

	public:
#ifdef __linux__
    typedef struct
    {
      std::string ip;
      std::string hw_type;
      std::string flags;
      std::string mac_addr;
      std::string device;
    }arpDev;
#endif
	  static std::vector<std::string> CreateIP_Table();
		static void SaveIP_table(const std::vector<std::string>& ip);
		static std::vector<std::string> GetIP_Table();
		static void RemoveIPTable();
	  //bool IsDeviceActive(std::string ip);
#ifdef __linux__
		static std::vector<arpDev> GetArpTable();
#endif
};
