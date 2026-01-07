#pragma once
/** @file WindowsSizingWinDivertMainController.hpp
 * The headers and global variables from other package for Linux programs
 *
 * @author Nick, Liao
 * @date 2024/08/15
 */

#ifdef _WIN32

#include <signal.h>  // For the alarm and interrupted signal
#include <stdio.h>   // For some C io functions
#include <stdlib.h>  // For some C functions

// Windows
#include <winsock2.h>  // This included library shall be put in the front of the one, namely "windows.h";
                       // otherwise warning information will be displayed when compiling
#include <windows.h>   // For capturing the interrupt signal

#include <algorithm>  // For std::max
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <map>
#include <chrono>

#include "../../Models/Commons/Headers/POSIXErrors.hpp"
#include "../../Models/Commons/Headers/Time.hpp"
#include "../../Models/FileParsers/Headers/InitializedJsonFileParser.hpp"
#include "../../Models/PCAP/Headers/PCAPPrototype.hpp"
#include "../../Models/PCAP/Headers/WindowsWinDivertPCAP.hpp"
#include "./SizingMainControllerPrototype.hpp"

/* In some windows platforms, u_char type is not defined; for ensuring that u_char can be used 
 * on all windows, here the process will verify whether u_char exists (TODO, delete)*/
#ifndef u_char
    // Alias to the type unsigned char to the one, u_char
    typedef unsigned char u_char;
#endif

namespace SizingControllers {
class WindowsWinDivertSizingMainController : public SizingMainControllerPrototype {
   public:
    /**
     * Definition for the Windows's ether_header structure; this structure is
     * equivalent to the one defined in linux (including the field names); to
     * more information, please refer to the following URL. https://www.cnblogs.com/LyShark/p/12989949.html
     */
    struct ether_header {
        u_char ether_dhost[6];  // Destination address (MAC address)
        u_char ether_shost[6];  // Source address (MAC address)
        u_short ether_type;
    };

    /**
     * Definition for the Windows's ether_header structure; this structure is
     * equivalent to the one defined in linux (including the field names); to
     * more information, please refer to the following URL. https://www.cnblogs.com/LyShark/p/12989949.html
     */
    struct ip {
        unsigned char ip_hl : 4;  // 4 bits
        unsigned char ip_v : 4;   // 4 bits
        unsigned char ip_tos;
        unsigned short ip_len;
        unsigned short ip_id;
        unsigned short ip_off;
        unsigned char ip_ttl;
        unsigned char ip_p;
        unsigned short ip_sum;
        struct in_addr ip_src;
        struct in_addr ip_dst;
    };

    /**
     * Definition for the Windows's ether_header structure; this structure is
     * equivalent to the one defined in linux (including the field names); to
     * more information, please refer to the following URL. https://www.cnblogs.com/LyShark/p/12989949.html
     */
    struct tcphdr {
        u_short th_sport;
        u_short th_dport;
        u_int th_seq;
        u_int th_ack;
        u_char th_x2 : 4;   // 4 bits
        u_char th_off : 4;  // 4 bits
        u_char th_flags;
        u_short th_win;
        u_short th_sum;
        u_short th_urp;
    };

    /**
     * Definition for the Windows's ether_header structure; this structure is
     * equivalent to the one defined in linux (including the field names); to
     * more information, please refer to the following URL. https://www.cnblogs.com/LyShark/p/12989949.html
     */
    struct udphdr {
        u_short uh_sport;
        u_short uh_dport;
        u_short uh_ulen;
        u_short uh_sum;
    };

    // For reserving the session's previous, the key is a tuple which combines sorted ip and port information;
    // the second one is the session's previous packet type;   
    // The value is defined as follows: 0: undefined; 1: TX, and 2: RX
    static std::map<std::tuple <uint32_t, uint32_t, uint16_t, uint16_t>, char> sessionMap;
    // For recording the maximum number of packets per second
    static long currentSqlMaxRequestNumberPerSec;
    // For reserving the starting time in the beginning or the updating time when the SQL statements receive
    static std::chrono::steady_clock::time_point startingTime;

    Commons::POSIXErrors start(int, char**);
    static BOOL WINAPI signalInterruptedHandler(DWORD);
    static void signalAlarmHandler();
    static Commons::POSIXErrors config(std::vector<unitService>*);
    static void packetHandler(u_char*, const pcap_pkthdr*, const u_char*);
    static void packetTask(PCAP::WindowsWinDivertPCAP*, void (*)(u_char*, const pcap_pkthdr*, const u_char*));
    static void packetFileTask(FILE**, const char*);
    static void executePacketInformationUpdate(long long, long*, long long*, long long*, long*, long long*, long long*, char*);
};
}  // namespace SizingMainCaller
#endif