

#ifdef __linux__
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif


#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define _WINSOCK_DEPRECATED_NO_WARNINGS
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "Ws2_32.lib")
#endif


#ifdef _WIN32
  DWORD WINAPI recvdata(LPVOID);
  #define close closesocket      // Used to resemble the linux close socket function on windows


  
#else
  void* recvdata(void*);
#endif

#include <cstring>


#include "sckt_io.hpp"
#include "spm.hpp"
#include "dbg_log.hpp"
#include "config.hpp"






// static SPM spm;




void SPM_SocketIO::SndPowerAction(int actType, std::string target)
{
  /*
  Todo:
  Scan target and check what kind of target is passed (computer name automatic name or custom name / target IP)
  */
  int sckt = 0;
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);

  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);


    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid Address !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {
      if(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        if(actType == 1)
        {
          send(sckt, "pwroff", strlen("pwroff"), 0);
        }
        else if(actType == 2)
        {
          send(sckt, "fpwroff", strlen("fpwroff"), 0);
        }
        else if(actType == 3)
        {
          send(sckt, "rbt", strlen("rbt"), 0);
        }
        else if(actType == 4)
        {
          send(sckt, "frbt", strlen("frbt"), 0);
        }
        close(sckt);
      }
    }
  }
}


void SPM_SocketIO::SndCustomSettings(std::string target, ServerSettings ss)
{
  int sckt = 0;
  std::ostringstream serialSettings; // Used for constructing the serialised server settings packet 

  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if (inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {
      if (connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        // Constructing the settings packet
        serialSettings << "customSettings: feedback=" << ss.feedback          << " allowSysInfo="  << ss.alowSysInfo;
        serialSettings << " dbgLog="                  << ss.debugLog          << " port="          << ss.listenPort;
        serialSettings << " skipProcScan="            << ss.skipProcScan      << " stdoutCapture=" << ss.stdoutCapture;
        serialSettings << " terminateProcesses="      << ss.terminateProceses << " writeLogFiles=" << ss.writeLogFiles;

        // Sending the packet to the desired target
        send(sckt, serialSettings.str().c_str(), strlen(serialSettings.str().c_str()), 0);
        close(sckt);
      }
    }
  }
}

void SPM_SocketIO::SndResetSettings(std::string target)
{
  int sckt = 0;
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {

      if (connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        send(sckt, "RstSettings", strlen("RstSettings"), 0);
        // Todo: Get feddback check from server to spm client if this action executed successfully
        SPM_LOG(SPMDebug::Success, "Server settings reset");
        close(sckt);
      }
    }
  }
}


void SPM_SocketIO::SndClearLogs(std::string target)
{
  int sckt = 0;
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {

      if(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        send(sckt, "clrLogs", strlen("clrLogs"), 0);
        SPM_LOG(SPMDebug::Success, "Sent clear logs");
        close(sckt);
      }
    }
  }
}

// AddProtectedProc
void SPM_SocketIO::AddProtectedProc(std::string target, std::string proc)
{
  int sckt = 0;
  //SPMDebug dbg;
  struct sockaddr_in serv_addr;
  std::ostringstream packet;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
    }
    else
    {
      packet << "NewProtectedProc=" << proc;
      send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
      SPM_LOG(SPMDebug::Success, "Custom process successfully sent to ", target);
      close(sckt);
    }
  }
}

void SPM_SocketIO::RemoveProtectedProc(std::string target, std::string proc)
{
  int sckt = 0;
  struct sockaddr_in serv_addr;
  std::ostringstream packet;
  
  
  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
    }
    else
    {
      packet << "RemoveProtectedProc=" << proc;
      send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
      SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
      close(sckt);
    }
  }
}

void SPM_SocketIO::AddUnauthorizedProc(std::string target, std::string proc)
{
  int sckt = 0;
  struct sockaddr_in serv_addr;
  std::ostringstream packet;
  
  
  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
    }
    else
    {
      packet << "AddUnauthorizedProc=" << proc;
      send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
      SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
      close(sckt);
    }
  }
}

void SPM_SocketIO::RemoveUnauthorizedProc(std::string target, std::string proc)
{
  int sckt = 0;
  struct sockaddr_in serv_addr;
  std::ostringstream packet;
  
  
  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
    }
    else
    {
      packet << "RemoveUnauthorizedProc=" << proc;
      send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
      SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
      close(sckt);
    }
  }
}

void SPM_SocketIO::SndStopServer(std::string target)
{
  int sckt = 0;
  const char * packet = "stopServ";
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {

      if(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        send(sckt, packet, strlen(packet), 0);
        SPM_LOG(SPMDebug::Success, "Sent clear logs");
        close(sckt);
      }
    }
  }
}

void SPM_SocketIO::SndResumeServer(std::string target)
{
  int sckt = 0;
  const char * packet = "resumeServ";
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {

      if(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        send(sckt, packet, strlen(packet), 0);
        SPM_LOG(SPMDebug::Success, "Sent clear logs");
        close(sckt);
      }
    }
  }
}

void SPM_SocketIO::SndKillServer(std::string target)
{
  int sckt = 0;
  const char * packet = "killServ";
  struct sockaddr_in serv_addr;

  sckt = socket(AF_INET, SOCK_STREAM, 0);
  if(sckt < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
  }
  else
  {

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if(inet_pton(AF_INET, target.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid address / Address not supported !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", target, "' is invalid !!!");
#endif
    }
    else
    {

      if(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", target);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", target, "' has failed !!!");
#endif
      }
      else
      {
        send(sckt, packet, strlen(packet), 0);
        SPM_LOG(SPMDebug::Success, "Sent clear logs");
        close(sckt);
      }
    }
  }
  // No more SPM stuff from here
  // duh
}