#include <iostream>
#include <chrono>
#include <thread>





#include "../spm.hpp"
#include "../config.hpp"
#include "../wol.hpp"
#include "../dev_detect.hpp"
#include "../utils.hpp"
#include "../dbg_log.hpp"
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
	std::cout << "IP Table: \n";
	for(const auto& ips : dummy)
	{
	  std::cout << ips << "\n";
	}
}

void Test7()
{
	std::vector<SPMList::device> someList = SPMList::Read(0);

	for(int i = 0; i < someList.size(); i++)
	{
		std::cout << someList[i].name << " " << someList[i].broadcast_ip << " " << someList[i].broadcast_ip << " " << someList[i].hw_addr << "\n";

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
	SPM_SocketIO::SndPowerAction(SPM_SocketIO::Reboot, "192.168.1.102");
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

void Test11()
{
  std::cout << "Test11() -> default Env check testing\n";
  
  SPM::CheckEnv(1);
  SPM::CheckEnv(2);
}

void Test12()
{
  std::cout << " Test12() -> CreateNewEnv()\n";
  
  SPM::envInfo e1;
  e1.name = "Idk Some name here";
  e1.description = "Some simple struct testing lol";
  
  SPM::envInfo e2;
  e2.name = "Another env ghgg";
  e2.description = "Things seems to be working just fine.";
  
  SPM::CreateNewEnv(nullptr, 1);
  SPM::CreateNewEnv(nullptr, 2);
  SPM::CreateNewEnv(&e1, 3);
  SPM::CreateNewEnv(&e2, 4);
}

void Test13()
{
  std::cout << "Test13() -> RemoveEnv()\n";
  
  SPM::RemoveEnv(1);
  SPM::RemoveEnv(2);
}

void Test14()
{
  std::vector<SPMList::device> some_list;
  some_list.push_back({"Pentium Dual Core", "default", "1c:6f:65:c2:e8:2f", "192.168.1.255", "192.168.1.102", "Just a simple demo"});
  some_list.push_back({"Core Duo E8400", "default", "6c:f0:49:a1:d9:e6", "192.168.1.255", "192.168.1.33", "The computher that dosen't have an SSD anymore lmao"});
  some_list.push_back({"NotExistingServer", "ipmi", "the:bmc:addr", "192.168.1.255", "192.168.1.54", "This is how a server would be stored"});
  
  SPMList::Write(0, some_list);
}

void Test15()
{
  std::vector<SPMList::device> parsed_list;
  
  parsed_list = SPMList::Read(0);
  
  SPMUtils::printDevArray(parsed_list);
}

void Test16()
{
  SPM_SocketIO::ping(50, 1, "192.168.1.102");
}

void Test17()
{
  SPMWakeOnLan::SndMagicPack("1c:6f:65:c2:e8:2f", "192.168.1.255");
  SPM_SocketIO::ping(50, 1, "192.168.1.102");
}


void SomeFn(int &n)
{
  n += 5;
}


void Test18()
{
  int num = 5;
  
  SomeFn(num);
  
  std::cout << "Modified stuff: " << num << "\n";
  
  
}

void Test19()
{
  SPM_SocketIO::ServerSettings mySettings;
  mySettings.alow_sys_info = false;
  mySettings.debug_log = false;
  mySettings.write_log_files = false;
  mySettings.stdout_capture = false;
  mySettings.terminate_proceses = false;
  mySettings.listen_port = 5200;
  mySettings.skip_proc_scan = false;
  mySettings.socket_response = false;
  SPM_SocketIO::SndCustomSettings("192.168.1.102", mySettings);
}

void Test20()
{
  SPM_SocketIO::SndKillServer("192.168.1.102");
}

#ifdef __linux__
void Test21()
{
  std::vector<SPMDetect::arpDev> test;
  test = SPMDetect::GetArpTable();
  
  for(const auto& dev : test)
  {
    std::cout << " | ip: " << dev.ip << " | hw_type: " << dev.hw_type << " | flags: " << dev.flags << " | mac_addr: " << dev.mac_addr << " | device: " << dev.device << "\n";
  }
}
#endif


int main(int argc, char * argv[])
{
	std::cout << "- - - - SPM BACKEND TESTS - - - - \n\n\n\n";
	// Test1(); // Linux Pass
	// Test2(); // Linux, Windows Pass
	// Test3(); // 
	// Test4(); // All pass
	// Test5();
  // Test6();
	// Test7();
	// Test8();
  Test9();
  // Test10();
  // Test11();
  // Test12();
  // Test13();
  // Test14();
  // Test15();
  // Test16();
  // Test17();
  // Test18();
  // Test19();
  // Test20();
#ifdef __linux__
    //Test21();
#endif
	return 0;
}