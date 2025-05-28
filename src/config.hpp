#pragma once



#include <vector>
#include <string>
#include <iostream>


// #include "spm.hpp"







class SPMConfig
{
  public:
	  typedef struct
	  {
			bool config_storage;                  // Enable / disable settings storage
	    bool pc_status_mpack;                 // Enable / disable pc status globally
      bool msgBox_log;                      // Enable / disable message boxes
      bool debug_log;                       // Enable / disable debug log to file (logs are stored in /home/user/.spm/logs)
      bool restrict_mode;                   // Enable / disable restriction mode
      bool restrict_timeout;                // Enable / disable restrict mode timeout
      bool power_opts_callbacks;            // Enable / disable socket callbacks when sending power events
      bool user_feedback;                   // Enable / disable user feedback trough message boxes
      int rescrict_time_span;               // Set ammount of time a rescricted session can last (in minutes)
      std::string restr_list_path;          // Set custom path for the encrypted pc list
      int usr_index;                        // Maps all users that can run in restricted mode with a number (usr1, usr2)
      int port;                             // Set custom port for poweroff / reboot packets
      int wolPort;                          // Set different port for wake on lan
      std::vector<std::string> restr_users; // Store all users that can enter restricted mode
	  }cfgStruct;
	static void Write(cfgStruct);
	static cfgStruct Read();
};
