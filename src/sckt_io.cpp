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

bool sockInit(SPM_SOCKET &s, std::string ip)
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
    serv_addr.sin_port = htons(globalConf.port);


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

#if defined(_WIN32) || defined(_WIN64)
  // Todo rn
  WSADATA wsaData;
  
  if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    SPM_LOG(SPMDebug::Err, "WSAStartup() Failed !!!");
    return false;
  }
  else
  {
    s = socket(AF_INET, SOCK_STREAM, 0);
    
    if(s == INVALID_SOCKET)
    {
      SPM_LOG(SPMDebug::Err, "Failed to create socket !!!");
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to create socket !!!");
#endif
      WSACleanup();
      return false;
    }
    else
    {
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(DEFAULT_PORT);

      // inet_pton is available in ws2tcpip.h on Windows
      if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Invalid Address !!");
#ifndef NO_MSGBOX
        SPMDebug::MsgBoxLog(SPMDebug::Err, "Address '", ip, "' is invalid !!!");
#endif
        closesocket(s);
        WSACleanup();
        return false;
      }
      else
      {
        if(connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        {
          SPM_LOG(SPMDebug::Err, "Failed to connect to ", ip);
#ifndef NO_MSGBOX
          SPMDebug::MsgBoxLog(SPMDebug::Err, "Connection to'", ip, "' has failed !!!");
#endif
          closesocket(s);
          WSACleanup();
          return false;
        }
      }
      WSACleanup();
      return true;
    }
  }
#endif
}


void CloseSPM_Socket(SPM_SOCKET s)
{
#if defined(_WIN32) || defined(_WIN64)
  closesocket(s);
  WSACleanup();
#endif

#ifdef __linux__
  close(s);
#endif
}

bool SPM_SocketIO::ping(int count, int delay, std::string ip)
{
  SPM_SOCKET sockfd;
  int replies = 0;
  
#ifdef __linux__
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if(sockfd < 0)
  {
    SPM_LOG(SPMDebug::Err, "socket() failed !!! | ", SPMUtils::getStdErr());
    return false;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  struct icmphdr icmp_hdr;
  icmp_hdr.type = ICMP_ECHO;
  icmp_hdr.code = 0;
  icmp_hdr.un.echo.id = getpid();
  
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
        SPM_LOG(SPMDebug::Err, "sendto() failed !!! | ", SPMUtils::getStdErr());
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
        SPM_LOG(SPMDebug::Err, "sendto() failed !!! | ", SPMUtils::getStdErr());
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
#endif

#if defined(_WIN32) || defined(_WIN64)
  struct icmphdr
  {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union
    {
      struct
      {
        uint16_t id;
        uint16_t sequence;
      }echo;
      
      uint32_t gateway;
      struct
      {
        uint16_t __unused;
        uint16_t mtu;
      }frag;
    }un;
  };
  
  
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
  {
    SPM_LOG(SPMDebug::Err, "WSAStartup failed!");
    return false;
  }
  else
  {

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd == INVALID_SOCKET)
    {
      SPM_LOG(SPMDebug::Err, "socket() failed !!! | ", WSAGetLastError());
      WSACleanup();
      return false;
    }
    else
    {
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(ip.c_str());

      icmphdr icmp_hdr;
      icmp_hdr.type = 8; // ICMP_ECHO
      icmp_hdr.code = 0;
      icmp_hdr.un.echo.id = (uint16_t)GetCurrentProcessId();

      //int replies = 0;

      if(delay <= 0)
      {
        SPM_LOG(SPMDebug::Err, "Delay cannot be 0 or smaller than 0 !!! | Delay set to 1 sec.");
        delay = 1;
      }

      auto checksum = [](void* b, int len) -> uint16_t
      {
        uint16_t* buf = (uint16_t*)b;
        uint32_t sum = 0;
        for (; len > 1; len -= 2)
        sum += *buf++;
        if (len == 1)
        sum += *(uint8_t*)buf;
        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        return(uint16_t)(~sum);
      };

      int seq = 0;
      int max_count = (count == 0) ? INT_MAX : count;
      for(int j = 0; j < max_count; ++j)
      {
        icmp_hdr.un.echo.sequence = seq++;
        icmp_hdr.checksum = 0;
        icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
        
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        auto start = std::chrono::high_resolution_clock::now();
        
        int sent = sendto(sockfd, (const char*)&icmp_hdr, sizeof(icmp_hdr), 0, (sockaddr*)&addr, sizeof(addr));
        if(sent == SOCKET_ERROR)
        {
          SPM_LOG(SPMDebug::Err, "sendto() failed !!! | ", WSAGetLastError());
          continue;
        }
        
        char buf[1024];
        sockaddr_in r_addr;
        int len = sizeof(r_addr);
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ret = select(0, &readfds, NULL, NULL, &timeout);
        if(ret > 0 && FD_ISSET(sockfd, &readfds))
        {
          int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, (sockaddr*)&r_addr, &len);
          if (r_addr.sin_addr.s_addr != addr.sin_addr.s_addr)
            continue;
          if(bytes > 0)
          {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            SPM_LOG(SPMDebug::Success, "seq: ", seq, " | Got reply from ", ip, " in ", elapsed.count(), " ms");
            replies++;
          }
        }
        else
        {
          SPM_LOG(SPMDebug::Err, "Request timed out !! | seq: ", seq);
        }
        if (count != 0 && j + 1 >= count)
          break;
      }
    }
  }

  closesocket(sockfd);
  WSACleanup();
  return replies > 0;
#endif
}


std::string SPM_SocketIO::GetServerReplyStr(int s)
{
  std::string out;
  int recv_bytes;
  
  recv_bytes = recv(s, repl_buf, sizeof(repl_buf), 0);
  
  if(recv_bytes > 0)
  {
    // Convert c string to std::string
    out = repl_buf;
  }
  else if(recv_bytes == 0)
  {
    SPM_LOG(SPMDebug::Warn, "No server reply !!! 0 bytes received");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "An error has occured while getting server reply !!!");
  }
  
  
  return out;
}

void SPM_SocketIO::SndPowerAction(SPM::server_actions serv_act, std::string target)
{
  SPM_SOCKET sckt = 0;
  std::string received_msg;
  int recv_byte;
  if(sockInit(sckt, target))
  {
    if(serv_act == SPM::Shutdown)
    {
      send(sckt, "pwroff", strlen("pwroff"), 0);
      SPM_LOG(SPMDebug::Info, "Poweroff sent to ", target);
      
      // Try to get erver reply
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceShutdown)
    {
      send(sckt, "fpwroff", strlen("fpwroff"), 0);
      SPM_LOG(SPMDebug::Warn, "Forced Poweroff sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Restart)
    {
      send(sckt, "rbt", strlen("rbt"), 0);
      SPM_LOG(SPMDebug::Info, "Reboot sent to ", target);
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceRestart)
    {
      send(sckt, "frbt", strlen("frbt"), 0);
      SPM_LOG(SPMDebug::Warn, "Forced Reboot sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Standby)
    {
      send(sckt, "stby", strlen("stby"), 0);
      SPM_LOG(SPMDebug::Info, "Standby sent to ", target);
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceStandby)
    {
      send(sckt, "fstby", strlen("fstby"), 0);
      SPM_LOG(SPMDebug::Info, "Forced Standby sent to ", target);
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Hibernate)
    {
      send(sckt, "hiber", strlen("hiber"), 0);
      SPM_LOG(SPMDebug::Info, "Hibernate sent to ", target);
      
      received_msg = GetServerReplyStr(sckt);
      received_msg.push_back('\n');
    }
    CloseSPM_Socket(sckt);
    
#ifdef ANSI_ESCAPES
    SPM_LOG(SPMDebug::Info, "Server replied from: \033[38;5;214m", target, "\033[0m with message: ", received_msg);
#else
    SPM_LOG(SPMDebug::Info, "Server replied from: ", target, " with message: ", received_msg);
#endif
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    close(sckt);
  }
}

void SPM_SocketIO::SndCustomSettings(std::string target, ServerSettings ss)
{
  SPM_SOCKET sckt = 0;
  std::ostringstream serialSettings; // Used for constructing the serialised server settings packet

  if(sockInit(sckt, target))
  {
    // Constructing the settings packet
    serialSettings << "customSettings: scktResp=" << ss.socket_response    << " allowSysInfo="  << ss.alow_sys_info;
    serialSettings << " dbgLog="                  << ss.debug_log          << " port="          << ss.listen_port;
    serialSettings << " skipProcScan="            << ss.skip_proc_scan     << " stdoutCapture=" << ss.stdout_capture;
    serialSettings << " terminateProcesses="      << ss.terminate_proceses << " writeLogFiles=" << ss.write_log_files;

    // Sending the packet to the desired target
    send(sckt, serialSettings.str().c_str(), strlen(serialSettings.str().c_str()), 0);
    SPM_LOG(SPMDebug::Info, "Sent custom settings");
    CloseSPM_Socket(sckt);
    
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::SndResetSettings(std::string target)
{
  SPM_SOCKET sckt = 0;
  const char * packet_str = "RstSettings";
  
  if(sockInit(sckt, target))
  {
    send(sckt, packet_str, strlen(packet_str), 0);
    // Todo: Get feddback check from server to spm client if this action executed successfully
    SPM_LOG(SPMDebug::Success, "Server settings reset");
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::SndClearLogs(std::string target)
{
  SPM_SOCKET sckt = 0;
  
  const char * packet_str = "clrLogs";
  
  if(sockInit(sckt, target))
  {
  
    send(sckt, "clrLogs", strlen("clrLogs"), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

// AddProtectedProc
void SPM_SocketIO::AddProtectedProc(std::string target, std::string proc)
{
  SPM_SOCKET sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "NewProtectedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "Protected process was successfuly sent to ", target);
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::RemoveProtectedProc(std::string target, std::string proc)
{
  SPM_SOCKET sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "RemoveProtectedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New protected process was successfully removed from ", target);
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::AddUnauthorizedProc(std::string target, std::string proc)
{
  SPM_SOCKET sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "AddUnauthorizedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::RemoveUnauthorizedProc(std::string target, std::string proc)
{
  SPM_SOCKET sckt = 0;
  std::ostringstream packet;
  
  if(sockInit(sckt, target))
  {
    packet << "RemoveUnauthorizedProc=" << proc;
    send(sckt, packet.str().c_str(), strlen(packet.str().c_str()), 0);
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully removed from ", target);
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::SndStopServer(std::string target)
{
  SPM_SOCKET sckt = 0;
  const char * packet = "stopServ";

  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent stop server");
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::SndResumeServer(std::string target)
{
  SPM_SOCKET sckt = 0;
  const char * packet = "resumeServ";
 
  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
}

void SPM_SocketIO::SndKillServer(std::string target)
{
  SPM_SOCKET sckt = 0;
  const char * packet = "killServ";

  if(sockInit(sckt, target))
  {
    send(sckt, packet, strlen(packet), 0);
    SPM_LOG(SPMDebug::Success, "Sent kill server");
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }
  
  // No more SPM stuff from here
  // duh
}