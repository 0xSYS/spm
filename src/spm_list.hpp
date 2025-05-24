#pragma once


#include <string>
#include <vector>


class SPMList
{
  public:
    typedef struct
    {
      int index; // Used only internally by the software
      std::string name;
      std::string macAddr;
      std::string broadcastIP;
      std::string defaultIP;
      std::string notes;
      bool restricred; 
    }computer;

    // Incomplete
    // General IPMI specs are required
    typedef struct
    {
      int index;
      std::string name;
      std::string IP;
      bool restricted;
    }server;

    static std::vector <computer> ReadComputerList();
    static server ReadServerList();

    static void WriteComputerList();
    static void WriteServerList();
};
