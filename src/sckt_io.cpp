

#ifdef __linux__
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <netinet/ip_icmp.h>
  #include <arpa/inet.h>
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
  #define close closesocket
#else
  void* recvdata(void*);
#endif

#include <cstring>
#include <chrono>
#include <thread>


#include "sckt_io.hpp"
#include "spm.hpp"
#include "dbg_log.hpp"
#include "config.hpp"












// Internal functions
unsigned short checksum(void *b, int len)
{
  unsigned short *buf = (unsigned short*)b;
  unsigned int sum=0;
  unsigned short result;

  for(sum = 0; len > 1; len -= 2)
    sum += *buf++;
  if(len == 1)
    sum += *(unsigned char*)buf;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

bool sockInit(int &s, std::string ip)
{
  
  struct sockaddr_in serv_addr;

  s = socket(AF_INET, SOCK_STREAM, 0);

  if(s < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
    
    return false;
#endif
  }
  else
  {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEFAULT_PORT);


    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
      SPM_LOG(SPMDebug::Err, "Invalid Address !!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", ip, "' is invalid !!!");
#endif
      return false;
    }
    else
    {
      if(connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
        SPM_LOG(SPMDebug::Err, "Failed to connect to ", ip);
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", ip, "' has failed !!!");
#endif
        return false;
      }
    }
    return true;
  }
}

bool SPM_SocketIO::ping(int count, int delay, std::string ip)
{
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if(sockfd < 0)
  {
    SPM_LOG(SPMDebug::Err, "socket() failed !!! | ", SPMUtils::getErr());
    return false;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  struct icmphdr icmp_hdr;
  icmp_hdr.type = ICMP_ECHO;
  icmp_hdr.code = 0;
  icmp_hdr.un.echo.id = getpid();

  int replies = 0;
  
  if(delay <= 0)
  {
    SPM_LOG(SPMDebug::Err, "Delay cannot be 0 or smaller than 0 !!! | Delay set to 1 sec.");
    delay = 1;
  }
  
  if(count == 0)
  {
    SPM_LOG(SPMDebug::Info, "Infinite pinging.");
    int seq = 0;
    while(true)
    {
      icmp_hdr.un.echo.sequence = seq;
      icmp_hdr.checksum = 0;
      icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
  
      std::this_thread::sleep_for(std::chrono::seconds(delay));
  
      auto start = std::chrono::high_resolution_clock::now();
  
      if(sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "sendto() failed !!! | ", SPMUtils::getErr());
        continue; // Don't return, try next ping
      }
  
      // Wait for reply
      char buf[1024];
      struct sockaddr_in r_addr;
      socklen_t len = sizeof(r_addr);
  
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(sockfd, &readfds);
  
      struct timeval timeout;
      timeout.tv_sec = 1; // 1 second timeout
      timeout.tv_usec = 0;
  
      int ret = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
      if(ret > 0 && FD_ISSET(sockfd, &readfds))
      {
        int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&r_addr, &len);
        if (r_addr.sin_addr.s_addr != addr.sin_addr.s_addr)
        {
          // Not from the host we are pinging, ignore
          continue;
        }
        if(bytes > 0)
        {
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double, std::milli> elapsed = end - start;
#ifdef ANSI_ESCAPES
          SPM_LOG(SPMDebug::Success, "seq: ", seq+1, " | Got reply from \033[38;5;214m", ip, "\033[0m in ", elapsed.count(), " ms");
#else
          SPM_LOG(SPMDebug::Success, "seq: ", seq+1, " | Got reply from ", ip, " in ", elapsed.count(), " ms");
#endif
          replies++;
        }
      }
      else
      {
        SPM_LOG(SPMDebug::Err, "Request timed out !! | seq: ", seq);
      }
      seq++;
    }
  }
  else
  {
    for(int j = 0; j < count; j++)
    {
      icmp_hdr.un.echo.sequence = j + 1;
      icmp_hdr.checksum = 0;
      icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

      std::this_thread::sleep_for(std::chrono::seconds(delay));

      auto start = std::chrono::high_resolution_clock::now();

      if(sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "sendto() failed !!! | ", SPMUtils::getErr());
        continue; // Don't return, try next ping
      }

      // Wait for reply
      char buf[1024];
      struct sockaddr_in r_addr;
      socklen_t len = sizeof(r_addr);

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(sockfd, &readfds);

      struct timeval timeout;
      timeout.tv_sec = 1; // 1 second timeout
      timeout.tv_usec = 0;

      int ret = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
      if(ret > 0 && FD_ISSET(sockfd, &readfds))
      {
        int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&r_addr, &len);
        if(r_addr.sin_addr.s_addr != addr.sin_addr.s_addr)
        {
          // Not from the host we are pinging, ignore
          continue;
        }
        if(bytes > 0)
        {
          auto end = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double, std::milli> elapsed = end - start;
#ifdef ANSI_ESCAPES
          SPM_LOG(SPMDebug::Success, "seq: ", j+1, " / ", count, " | Got reply from \033[38;5;214m", ip, "\033[0m in ", elapsed.count(), " ms");
#else
          SPM_LOG(SPMDebug::Success, "seq: ", j+1, " / ", count, " | Got reply from ", ip, " in ", elapsed.count(), " ms");
#endif
          replies++;
        }
      }
      else
      {
        SPM_LOG(SPMDebug::Err, "Request timed out !! | seq: ", j+1);
      }
    }
  }

  close(sockfd);
  
  return replies > 0; // Return true if at least one reply was received
}

void SPM_SocketIO::SndPowerAction(int actType, std::string target)
{
  int sckt = 0;
  if(sockInit(sckt, target))
  {
    if(actType == 1)
    {
      send(sckt, "pwroff", strlen("pwroff"), 0);
      SPM_LOG(SPMDebug::Info, "Poweroff sent to ", target);
    }
    else if(actType == 2)
    {
      send(sckt, "fpwroff", strlen("fpwroff"), 0);
      SPM_LOG(SPMDebug::Warn, "Forced Poweroff sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
    }
    else if(actType == 3)
    {
      send(sckt, "rbt", strlen("rbt"), 0);
      SPM_LOG(SPMDebug::Info, "Reboot sent to ", target);
    }
    else if(actType == 4)
    {
      send(sckt, "frbt", strlen("frbt"), 0);
      SPM_LOG(SPMDebug::Warn, "Forced Reboot sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
    }
    else if(actType == 5)
    {
      send(sckt, "stby", strlen("stby"), 0);
      SPM_LOG(SPMDebug::Info, "Standby sent to ", target);
    }
    else if(actType == 6)
    {
      send(sckt, "fstby", strlen("fstby"), 0);
      SPM_LOG(SPMDebug::Info, "Forced Standby sent to ", target);
    }
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndCustomSettings(std::string target, ServerSettings ss)
{
  int sckt = 0;
  std::ostringstream serialSettings; // Used for constructing the serialised server settings packet

  if(sockInit(sckt, target))
  {
    // Constructing the settings packet
    serialSettings << "customSettings: scktResp=" << ss.feedback          << " allowSysInfo="  << ss.alowSysInfo;
    serialSettings << " dbgLog="                  << ss.debugLog          << " port="          << ss.listenPort;
    serialSettings << " skipProcScan="            << ss.skipProcScan      << " stdoutCapture=" << ss.stdoutCapture;
    serialSettings << " terminateProcesses="      << ss.terminateProceses << " writeLogFiles=" << ss.writeLogFiles;

    // Sending the packet to the desired target
    send(sckt, serialSettings.str().c_str(), strlen(serialSettings.str().c_str()), 0);
    close(sckt);
    
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndResetSettings(std::string target)
{
  int sckt = 0;
  const char * packet_str = "RstSettings";
  
  if(sockInit(sckt, target))
  {
    send(sckt, packet_str, strlen(packet_str), 0);
    // Todo: Get feddback check from server to spm client if this action executed successfully
    SPM_LOG(SPMDebug::Success, "Server settings reset");
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndClearLogs(std::string target)
{
  int sckt = 0;
  
  const char * packet_str = "clrLogs";
  
  if(sockInit(sckt, target))
  {
  
    send(sckt, "clrLogs", strlen("clrLogs"), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

// AddProtectedProc
void SPM_SocketIO::AddProtectedProc(std::string target, std::string proc)
{
  int sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "NewProtectedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "Custom process successfully sent to ", target);
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::RemoveProtectedProc(std::string target, std::string proc)
{
  int sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "RemoveProtectedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::AddUnauthorizedProc(std::string target, std::string proc)
{
  int sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "AddUnauthorizedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::RemoveUnauthorizedProc(std::string target, std::string proc)
{
  int sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "RemoveUnauthorizedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndStopServer(std::string target)
{
  int sckt = 0;
  const char * packet = "stopServ";

  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndResumeServer(std::string target)
{
  int sckt = 0;
  const char * packet = "resumeServ";
 
  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndKillServer(std::string target)
{
  int sckt = 0;
  const char * packet = "killServ";

  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    close(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
  
  // No more SPM stuff from here
  // duh
}