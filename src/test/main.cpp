#include <iostream>





#include "../spm.hpp"
#include "../config.hpp"
#include "../wol.hpp"
#include "../dev_detect.hpp"
#include "../utils.hpp"
#include "../dbg_log.hpp"
#include "../globals.hpp"
#include "../spm_list.hpp"
#include "../sckt_io.hpp"






void Test1()
{
	std::cout << "Test1() -> SPM::Init(nullptr)\n";
	SPM::Init(nullptr);
}

void Test2()
{
	std::cout << "Test2() -> SPMWakeOnLan::SndMagicPack()\n";
	SPMWakeOnLan::SndMagicPack("1c:6f:65:c2:e8:2f", "192.168.1.255");
	//wol.SndMagicPack("6c:f0:49:a1:d9:e6", "192.168.1.255");
	// 1c:6f:65:c2:e8:2f //g41MT-S2P
	// 6c:f0:49:a1:d9:e6 // g41M-ES2L
}

void Test3()
{
	std::vector<uint8_t> byte_array;
	std::cout << "Test3() -> SPMWakeOnLan::parse_mac_addr()\n";
	if(SPMWakeOnLan::parse_mac_addr("1c:6f:65:c2:e8:2f", byte_array))
	{
		SPM_LOG(SPMDebug::Success, "Mac address parsed successfully");
	}
	else
	{
		SPM_LOG(SPMDebug::Err, "Failed to parse mac address");
	}
}

void Test4()
{
	std::cout << "Test4() -> Log testing\n";
	SPM_LOG(SPMDebug::Info, "Info Test");
	SPM_LOG(SPMDebug::Success, "Success Test");
	SPM_LOG(SPMDebug::Warn, "Warn Test");
	SPM_LOG(SPMDebug::Err, "Err Test");
}

void Test5()
{
	SPMDebug::MsgBoxLog(SPMDebug::Info, " * ", "Arg 1 ", "Arg 2 ", 5);
	SPMDebug::MsgBoxLog(SPMDebug::Success, " * ", "Success test " , "And another stringy string ", 48);
	SPMDebug::MsgBoxLog(SPMDebug::Warn, " * ", "Warning test ", "yet anoter text and a number idk ", 152);
	SPMDebug::MsgBoxLog(SPMDebug::Err, " * ", "This is some error ", " things work ok ig ", 196);
}

void Test6()
{
	std::vector<std::string> dummy = SPMDetect::CreateIP_Table();
}

void Test7()
{
	std::vector<SPMList::computer> someList = SPMList::ReadComputerList();

	for(int i = 0; i < someList.size(); i++)
	{
		std::cout << someList[i].name << " " << someList[i].broadcastIP << " " << someList[i].broadcastIP << " " << someList[i].macAddr << "\n";

	}
}

void Test8()
{
	for(int i = 0; i < 5; i++)
	{
		SPM_LOG(SPMDebug::Info, "Updating stuff");
	}
}

void Test9()
{
	std::cout << "Power actions test\n";
	SPM_SocketIO::SndPowerAction(SPM_SocketIO::Poweroff, "192.168.1.102");
	//SPM_SocketIO::SndPowerAction(SPM_SocketIO::Poweroff, "192.168.1.34");
}

void Test10()
{
	std::string file = "/home/0xsys/.spm/config.json";

	if(!SPMUtils::checkFile(file))
	{
		SPM_LOG(SPMDebug::Err, "File not Found");
	}
	else
	{
		SPM_LOG(SPMDebug::Success, "File exists");
	}
}


int main(int argc, char * argv[])
{
	std::cout << "- - - - SPM BACKEND TESTS - - - - \n\n\n\n";
	Test1(); // Linux Pass
	//Test2(); // Linux, Windows Pass
	// Test3(); // 
	// Test4(); // All pass
	// Test5();
  // Test6();
	// Test7();
	// Test8();
  //Test9();
  // Test10();
	return 0;
}