

#include <vector>
#include <string>
#include <cstdint>



#include "spm.hpp"
#include "api_utils.h"
#include "wol.hpp"
#include "dev_detect.hpp"

//SPMWakeOnLan wol_exp;
//SPM spm_exp;
//SPMDetect spm_detct;






API_EXPORT void SPM_Init()
{
  SPM::Init(nullptr);
}

API_EXPORT void SPM_SafeExit()
{
  SPM::Terminate();
}

API_EXPORT bool SPM_ParseMac(std::string mac, std::vector<uint8_t> &mac_bytes)
{
  return SPMWakeOnLan::parse_mac_addr(mac, mac_bytes);
}

API_EXPORT void SPM_SndMagicPacket(std::string mac_addr, std::string broadcastIP, int port = 9)
{
  SPMWakeOnLan::SndMagicPack(mac_addr, broadcastIP, port);
}

API_EXPORT std::vector<std::string> SPM_DeviceDetect()
{
  return SPMDetect::CreateIP_Table();
}

API_EXPORT void SPM_Snd_Reboot(int mode, std::string ip)
{
  // Call c++
}

API_EXPORT void SPM_SndPowerOff(int mode, std::string ip)
{
  // Call c++
}
