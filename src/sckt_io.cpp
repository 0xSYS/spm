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

bool SPM_SocketIO::sockInit(SPM_SOCKET &s)
{
#ifdef __linux__
  int broadcast_enable = 1;
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if(s < 0)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket !!!");
    perror("socket");
    return false;
    
    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
    {
      SPM_LOG(SPMDebug::Err, "Failed to set socket options !!!");
      perror("setsockopt (SO_BROADCAST) failed");
      CloseSPM_Socket(s);
      return false;
    }
  }
  else
  {
    return true;
  }
#endif

#if defined(_WIN32) || defined(_WIN64)
  WSADATA wsaData;
  int result;
  
  result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0)
  {
    printf("WSAStartup failed: %d\n", result);
    return false;
  }
  
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd == INVALID_SOCKET)
  {
    SPM_LOG(SPMDebug::Err, "Failed to create socket | Last WSA Err: ", WSAGetLastError());
    WSACleanup();
    return false;
  }
  else
  {
    WSAGetLastError();
  }
#endif
}

void SPM_SocketIO::addressSetup(struct sockaddr_in *addr, std::string ip, int port)
{
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  
  SPM_LOG(SPMDebug::Info, "Setup addres on port: ", port);
  
  int ret = inet_pton(AF_INET, ip.c_str(), &addr->sin_addr);
  if(ret <= 0)
  {
    // ret == 0: Not in presentation format
    // ret == -1: Invalid address family
    //throw std::runtime_error("Invalid IP address: " + ip);
    SPM_LOG(SPMDebug::Err, "Invalid IP address: ", ip);
  }
}


void SPM_SocketIO::CloseSPM_Socket(SPM_SOCKET s)
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
    SPM_LOG(SPMDebug::Info, "Infinite pinging: ", ESC_ORANGE3, ip, ESC_RST);
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
          SPM_LOG(SPMDebug::Success, "seq: ", seq+1, " | Got reply from ", ESC_ORANGE3, ip, ESC_RST, " in ", elapsed.count(), " ms");
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
          SPM_LOG(SPMDebug::Success, "seq: ", j+1, " / ", count, " | Got reply from ", ESC_ORANGE3, ip, ESC_RST, " in ", elapsed.count(), " ms");
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

std::string SPM_SocketIO::GetServerReplyStr(int s, struct sockaddr_in serv_addr)
{
  std::string out;
  int recv_bytes;
  
  socklen_t addrlen = sizeof(serv_addr);
  
  recv_bytes = recvfrom(s, repl_buf, sizeof(repl_buf)-1, 0, (struct sockaddr*)&serv_addr, &addrlen);
  
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
  std::string received_msg;
  int recv_byte;
  struct sockaddr_in addr;
  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    if(serv_act == SPM::Shutdown)
    {
      sendto(main_socket, "pwroff", strlen("pwroff"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Info, "Poweroff sent to ", target);
      
      // Try to get erver reply
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceShutdown)
    {
      sendto(main_socket, "fpwroff", strlen("fpwroff"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Warn, "Forced Poweroff sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Restart)
    {
      sendto(main_socket, "rbt", strlen("rbt"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Info, "Reboot sent to ", target);
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceRestart)
    {
      sendto(main_socket, "frbt", strlen("frbt"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Warn, "Forced Reboot sent to ", target, " THIS CAN CAUSE SYSTEM CORRUPTION IF NOT CAREFULLY HANDELED !!!");
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Standby)
    {
      sendto(main_socket, "stby", strlen("stby"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Info, "Standby sent to ", target);
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::ForceStandby)
    {
      sendto(main_socket, "fstby", strlen("fstby"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Info, "Forced Standby sent to ", target);
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }
    else if(serv_act == SPM::Hibernate)
    {
      sendto(main_socket, "hiber", strlen("hiber"), 0, (struct sockaddr*)&addr, sizeof(addr));
      SPM_LOG(SPMDebug::Info, "Hibernate sent to ", target);
      
      received_msg = GetServerReplyStr(main_socket, addr);
      received_msg.push_back('\n');
    }

    SPM_LOG(SPMDebug::Info, "Server replied from: ", ESC_ORANGE3, target, ESC_RST, " with message: ", received_msg);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not initialized !!!");
    //close(sckt);
  }
}

void SPM_SocketIO::SndCustomSettings(std::string target, ServerSettings ss)
{
  //SPM_SOCKET sckt = 0;
  struct sockaddr_in addr;
  std::ostringstream serialSettings; // Used for constructing the serialised server settings packet

  if(is_spm_init)
  {
    // Constructing the settings packet
    serialSettings << "customSettings: scktResp=" << ss.replies            << " allowSysInfo="  << ss.alow_sys_info;
    serialSettings << " dbgLog="                  << ss.debug_log          << " port="          << ss.listen_port;
    serialSettings << " skipProcScan="            << ss.skip_proc_scan     << " stdoutCapture=" << ss.stdout_capture;
    serialSettings << " terminateProcesses="      << ss.terminate_proceses << " writeLogFiles=" << ss.write_log_files;
    
    addressSetup(&addr, target, globalConf.port);

    // Sending the packet to the desired target
    sendto(main_socket, serialSettings.str().c_str(), strlen(serialSettings.str().c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Info, "Sent custom settings");
    
    /*
    Todo: Get server reply to check if server settings got saved 
    */
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::SndResetSettings(std::string target)
{
  //SPM_SOCKET sckt = 0;
  struct sockaddr_in addr;
  const char * packet_str = "RstSettings";
  
  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet_str, strlen(packet_str), 0, (struct sockaddr*)&addr, sizeof(addr));
    // Todo: Get feddback check from server to spm client if this action executed successfully
    SPM_LOG(SPMDebug::Success, "Server settings reset");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::SndClearLogs(std::string target)
{
  struct sockaddr_in addr;
  const char * packet_str = "clrLogs";
  
  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet_str, strlen(packet_str), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

// AddProtectedProc
void SPM_SocketIO::AddProtectedProc(std::string target, std::string proc)
{
  struct sockaddr_in addr;
  std::ostringstream packet;
  
  if(is_spm_init)
  {
    packet << "NewProtectedProc=" << proc;
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet.str().c_str(), strlen(packet.str().c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "Protected process was successfuly sent to ", target);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::RemoveProtectedProc(std::string target, std::string proc)
{
  struct sockaddr_in addr;
  std::ostringstream packet;
  
  if(is_spm_init)
  {
    packet << "RemoveProtectedProc=" << proc;
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet.str().c_str(), strlen(packet.str().c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "New protected process was successfully removed from ", target);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::AddUnauthorizedProc(std::string target, std::string proc)
{
  struct sockaddr_in addr;
  std::ostringstream packet;
  
  if(is_spm_init)
  {
    packet << "AddUnauthorizedProc=" << proc;
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet.str().c_str(), strlen(packet.str().c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully sent to ", target);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::RemoveUnauthorizedProc(std::string target, std::string proc)
{
  std::ostringstream packet;
  struct sockaddr_in addr;
  
  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet.str().c_str(), strlen(packet.str().c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "New unauthorized process was successfully removed from ", target);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::SndStopServer(std::string target)
{
  const char * packet = "stopServ";
  struct sockaddr_in addr;

  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet, strlen(packet), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "Sent stop server");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

void SPM_SocketIO::SndResumeServer(std::string target)
{
  const char * packet = "resumeServ";
  struct sockaddr_in addr;
 
  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet, strlen(packet), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "Sent clear logs");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "SPM Not Initialized !!!");
  }
}

bool SPM_SocketIO::InternalPing(int count, int delay, std::string ip)
{


  /*
  Veeryy wip rn
  */
  bool out;
  SPM_SOCKET sckt;
  const char * packet = "SpmPing";


  if(delay << 1)
  {
    SPM_LOG(SPMDebug::Err, "Delay cannot be smaller than 1 second.");
    SPM_LOG(SPMDebug::Info, "Setting delay to 1 second.");
    delay = 1;
  }

  if(is_spm_init)
  {
    if(count == 0)
    {
      SPM_LOG(SPMDebug::Info, "Infinite pinging with custom packet: ", ip);
      while(true)
      {
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        send(sckt, packet, strlen(packet), 0);
      }
    }
    else if(count << 0)
    {
      for(int i = 0; i < count; i++)
      {
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        send(sckt, packet, strlen(packet), 0);
        SPM_LOG(SPMDebug::Info, "Ping count: ", i);
      }
    }
    // send(sckt, packet, strlen(packet), 0);
    CloseSPM_Socket(sckt);
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
    CloseSPM_Socket(sckt);
  }


  return out; 
}

void SPM_SocketIO::SndKillServer(std::string target)
{
  const char * packet = "killServ";
  struct sockaddr_in addr;

  if(is_spm_init)
  {
    addressSetup(&addr, target, globalConf.port);
    sendto(main_socket, packet, strlen(packet), 0, (struct sockaddr*)&addr, sizeof(addr));
    SPM_LOG(SPMDebug::Success, "Sent kill server");
  }
  else
  {
    SPM_LOG(SPMDebug::Err, "Failed to initialize socket !!!");
  }
  
  // No more SPM stuff from here
  // duh
}
