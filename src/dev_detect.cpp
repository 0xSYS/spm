
#ifdef __linux__
  #include <ifaddrs.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
  #include <winsock2.h>
  #include <iphlpapi.h>

  #pragma comment(lib, "iphlpapi.lib")
  #pragma comment(lib, "ws2_32.lib")
#endif
#include <string>
#include <vector>






#include "dev_detect.hpp"
#include "dbg_log.hpp"





std::vector<std::string> SPMDetect::CreateIP_Table()
{
  std::vector<std::string> ipTable;
  
#ifdef __linux__
  std::vector<arpDev> arp_devices;
  arp_devices = SPMDetect::GetArpTable();
  
  for(const auto& dev : arp_devices)
  {
    ipTable.push_back(dev.ip);
  }
#endif

#if defined(_WIN32) || defined(_WIN64)
  DWORD size = 0; 
  GetIpNetTable(nullptr, &size, false);
  PMIB_IPNETTABLE ipNetTable = (PMIB_IPNETTABLE)malloc(size);
  
  DWORD res = GetIpNetTable(ipNetTable, &size, false);
  if(res == NO_ERROR)
  {
    for(DWORD i = 0; i < ipNetTable->dwNumEntries; i++)
    {
      in_addr ipAddr;
      ipAddr.S_un.S_addr = ipNetTable->table[i].dwAddr;
      std::cout << "IP: " << inet_ntoa(ipAddr) << std::endl;
    }
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to get IP table!!!\nWindows Error: ", SPMUtils::GetWinApiErr(res));
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to scan IP table !!!\nWindows Error: ", SPMUtils::GetWinApiErr(res));
#endif
  }
  free(ipNetTable);
#endif
  
  return ipTable;
}

#ifdef __linux__
std::vector<SPMDetect::arpDev> SPMDetect::GetArpTable()
{
  /*
  In a much simpler and practical usage you can just do cat "/proc/net/arp"
  The only difference is that it stores all this into a structured array 
  */
  std::vector<SPMDetect::arpDev> out_table;
  std::ifstream arp_table("/proc/net/arp");
  
  if(!arp_table.is_open())
  {
    SPM_LOG(SPMDebug::Err, "Failed to open arp table !!!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to get available network devices.\nCouldn't read arp table.");
#endif
  }
  else
  {
    // Skip the text header of the table
    std::string header_line;
    std::getline(arp_table, header_line);
    
    while(std::getline(arp_table, header_line))
    {
      std::istringstream line(header_line);
      std::string ip, hw_type, flags, mac, mask, device;
      
      // Simply just assign the contents of the table into each string
      if(!(line >> ip >> hw_type >> flags >> mac >> mask >> device))
        continue;
        
      // Put devices into the vector
      out_table.push_back({ip, hw_type, flags, mac, device});
    }
  }
  return out_table;
}
#endif