#include <sstream>
#include <vector>
// bullshit #include <inifile-cpp/inicpp.h>
#include <nlohmann/json.hpp>



#include "dbg_log.hpp"
#include "spm_list.hpp"
#include "utils.hpp"
//#include "globals.hpp"


SPMUtils temp;


std::vector <SPMList::device> SPMList::ReadDevList()
{
  device in;

  std::vector<device> inList;

	//ini::IniFile ini_list;
	std::ostringstream list_path;
#ifdef __linux__
	list_path << SPMUtils::GetHomeDir() << "/.spm/lists/main_list.sls"; // The path to the list of computers
#endif

#if defined(_WIN32) || defined(_WIN64)
	list_path << SPMUtils::GetHomeDir() << "\\.spm\\lists\\main_list.sls"; // The path to the list of computers
#endif


	if(!temp.checkFile(list_path.str()))
	{
		SPM_LOG(SPMDebug::Err, "'.spm/lists/main_list.sls' not found !");
	}
	else
	{

	//ini_list.load(list_path.str());

//in.index = ini_list["main"]["pcCount"].as<int>();

	std::ostringstream incrementalPcName;

	for(int i = 0; i < 3; i++)
	{
		incrementalPcName.str("");
		incrementalPcName.clear();
	  incrementalPcName << "PC_" << i;
		//in.macAddr     = ini_list[incrementalPcName.str()]["MacAddress"].as<std::string>();
		//in.broadcastIP = ini_list[incrementalPcName.str()]["BroadcastIP"].as<std::string>();
		//in.name        = ini_list[incrementalPcName.str()]["Name"].as<std::string>();
		//in.defaultIP   = ini_list[incrementalPcName.str()]["IP"].as<std::string>();
		//in.notes       = ini_list[incrementalPcName.str()]["Notes"].as<std::string>();
		//std::cout << "for " << incrementalPcName.str() << " " << in.defaultIP << " " << in.macAddr << "\n";
		inList.push_back(in);
	}
	}
  return inList;


}


void SPMList::WriteDevList()
{
  //ini::IniFile out_list;
	//ini::IniFile temp_read;
	std::ostringstream list_path;
#ifdef __linux__
	list_path << SPMUtils::GetHomeDir() << "/.spm/lists/main_list.sls";
#endif

#if defined(_WIN32) || defined(_WIN64)
  list_path << SPMUtils::GetHomeDir() << "\\.spm\\lists\\main_list.sls"; // The path to the list of computers
#endif

  // Temporary reading of pc list to ensure that each new section is ordered by number. (This helps to prevent the same section names to be written)
	//MainList.index = temp_read["main"]["count"].as<int>();

	//if(MainList.index != MainList.index+1)
	//  MainList.index++;

	//out_list["main"]["count"] = MainList.index; // Store the number of computers entered to be used latter on for reading all assigned computers.
  //std::ostringstream incrementalPcName;
  //incrementalPcName << "PC_" << MainList.index;
  //out_list[incrementalPcName.str()]["MacAddress"]  = MainList.macAddr;
  //out_list[incrementalPcName.str()]["BroadcastIP"] = MainList.broadcastIP;
  //out_list[incrementalPcName.str()]["Name"]        = MainList.name;
  //out_list[incrementalPcName.str()]["IP"]          = MainList.defaultIP;
  //out_list[incrementalPcName.str()]["Notes"]       = MainList.notes;

	//out_list.save(list_path.str());
}
