#pragma once




#include "spm.hpp"



class SPM_IPMI
{
  public:
    enum power_action
    {
      Wake       = 0x01,
      Poweroff   = 0x00,
      HardReset  = 0x03,
      PowerCycle = 0x02
    };
    
    static void ServerPowerAction(power_action spa, std::string bmc_addr, std::string usr, std::string pw); // [ ] Send power action to a server using the IPMI interface
    static void SetIPMI_User(std::string usr, std::string pw);                                              // [ ] Set user and password on IPMI in order to perform an action
};