#pragma once




#include "spm.hpp"



class SPM_IPMI
{
  public:
    
    static void ServerPowerAction(SPM::server_actions spa, std::string bmc_addr, std::string usr, std::string pw); // [ ] Send power action to a server using the IPMI interface
    static void SetIPMI_User(std::string usr, std::string pw);                                              // [ ] Set user and password on IPMI in order to perform an action
};