#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>



#include "dbg_log.hpp"
#include "spm_list.hpp"
#include "utils.hpp"




std::vector <SPMList::device> SPMList::Read(int env_index)
{

  std::vector<device> inList;

	// Get the path to the given storage environment
	std::ostringstream file_path;
#ifdef __linux__
	file_path << SPMUtils::GetHomeDir() << "/.spm/data/env_" << env_index << "/";
#endif

#if defined(_WIN32) || defined(_WIN64)
	file_path << SPMUtils::GetHomeDir() << "\\.spm\\data\\env_" << env_index << "\\";
#endif

  if(SPMUtils::checkDir(file_path.str()))
  {
    file_path << "devices.sls";
    if(SPMUtils::checkFile(file_path.str()))
    {
      // Read an array of json objects
      std::ifstream in_file(file_path.str());
      
      if(!in_file)
      {
        SPM_LOG(SPMDebug::Err, "Failed to open devices.sls from environment ", env_index, " !!!");
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to open devices.sls from environment ", env_index , " !!!");
#endif
      }
      else
      {
        std::vector<uint8_t> msgpack_data((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        nlohmann::json j = nlohmann::json::from_msgpack(msgpack_data);
        for (const auto& dev : j)
        {
          inList.push_back(
            {
              dev["name"],
              dev["interfaceType"],
              dev["hwAddress"],
              dev["broadcastAddr"],
              dev["ip"],
              dev["description"]
            }
          );
        }
      }
    }
    else
    {
      SPM_LOG(SPMDebug::Err, "Missing devices.sls from environment ", env_index);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Missing devices.sls from environment ", env_index);
#endif
    }
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Evnironment ", env_index, " does not exists !!!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Environment ", env_index, " does not exists!!!");
#endif
  }


	
  return inList;
}


void SPMList::Write(int env_index, std::vector<device> l)
{
  
  // Get the path to the given storage environment
	std::ostringstream file_path;
	nlohmann::json json_out = nlohmann::json::array();
#ifdef __linux__
  file_path << SPMUtils::GetHomeDir() << "/.spm/data/env_" << env_index << "/";
#endif
 
#if defined(_WIN32) || defined(_WIN64)
	file_path << SPMUtils::GetHomeDir() << "\\.spm\\data\\env_" << env_index << "\\";
#endif


  if(SPMUtils::checkDir(file_path.str()))
  {
    file_path << "devices.sls";
    // Do the list writing
    int dev_index = 0;
    for(const auto& dev : l)
    {
      dev_index++;
      json_out.push_back({
        { "index", dev_index },
        { "name", dev.name },
        { "interfaceType", dev.interface_type },
        { "hwAddress", dev.hw_addr },
        { "broadcastAddr", dev.broadcast_ip },
        { "ip", dev.os_ip },
        { "description", dev.notes }
      });
    }
    
    
    std::vector<std::uint8_t> msgpack_dat = nlohmann::json::to_msgpack(json_out);
    std::ofstream out_devls(file_path.str(), std::ios::binary);
    
    if(!out_devls.is_open())
    {
      SPM_LOG(SPMDebug::Err, "Failed to write device list on environment ", env_index);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to write device list on environment ", env_index);
#endif
    }
    else
    {
      out_devls.write(reinterpret_cast<const char*>(msgpack_dat.data()), msgpack_dat.size());
      out_devls.close();
      SPM_LOG(SPMDebug::Success, "Device list on environment ", env_index, " successfully written!");
    }
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Evnironment ", env_index, " does not exists !!!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Environment ", env_index, " does not exists!!!");
#endif
  }
}
