#pragma once


#include <string>
#include <vector>


class SPMList
{
  public:
    typedef struct
    {
      std::string name;
      std::string interface_type;
      std::string mac_addr;
      std::string broadcast_ip;
      std::string bmc_ip_addr; // IMPI specific
      std::string os_ip;
      std::string notes;
      bool restricred;
    }device;

    static std::vector <device> ReadDevList();
    static void WriteDevList();
};
