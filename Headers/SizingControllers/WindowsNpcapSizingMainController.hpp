#pragma once
/** @file WindowsNpcapSizingMainController.hpp
 * The controller for Windows using the Npcap kernel.
 */

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../../Models/Commons/Headers/POSIXErrors.hpp"
#include "../../Models/Commons/Headers/Time.hpp"
#include "../../Models/FileParsers/Headers/InitializedJsonFileParser.hpp"
#include "../../Models/PCAP/Headers/PCAPPrototype.hpp"
#include "../../Models/PCAP/Headers/WindowsNpcapPCAP.hpp" // Include the Npcap model
#include "../../Services/SizingServices/Headers/Transformer.hpp"
#include "./SizingMainControllerPrototype.hpp"

#ifndef u_char
    typedef unsigned char u_char;
#endif

namespace SizingControllers {

class WindowsNpcapSizingMainController : public SizingMainControllerPrototype {
   public:
    // Packet parsing structs (can be reused)
    struct ether_header {
        u_char ether_dhost[6];
        u_char ether_shost[6];
        u_short ether_type;
    };

    struct ip {
        unsigned char ip_hl : 4;
        unsigned char ip_v : 4;
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

    struct tcphdr {
        u_short th_sport;
        u_short th_dport;
        u_int th_seq;
        u_int th_ack;
        u_char th_x2 : 4;
        u_char th_off : 4;
        u_char th_flags;
        u_short th_win;
        u_short th_sum;
        u_short th_urp;
    };

    struct udphdr {
        u_short uh_sport;
        u_short uh_dport;
        u_short uh_ulen;
        u_short uh_sum;
    };

    // Static members
    static std::map<std::tuple <uint32_t, uint32_t, uint16_t, uint16_t>, char> sessionMap;
    static long currentSqlMaxRequestNumberPerSec;
    static std::chrono::steady_clock::time_point startingTime;
    // For recording the maximum size of tx packets per second
    static long currentMaxTxSizePerSec;
    // For recording the maximum size of rx packets per second
    static long currentMaxRxSizePerSec;
    // For reserving the starting time in the beginning or the updating time when receiving a tx packet
    static std::chrono::steady_clock::time_point startingTimeTX;
    // For reserving the starting time in the beginning or the updating time when receiving a rx packet
    static std::chrono::steady_clock::time_point startingTimeRX;

    // Public interface
    Commons::POSIXErrors start(int, char**);

    // Static helper methods
    static BOOL WINAPI signalInterruptedHandler(DWORD);
    static void signalAlarmHandler();
    static Commons::POSIXErrors config(std::vector<unitService>*);
    static void packetHandler(u_char*, const pcap_pkthdr*, const u_char*);
    // Task function must accept the Npcap PCAP object
    static void packetTask(PCAP::WindowsNpcapPCAP*, void (*)(u_char*, const pcap_pkthdr*, const u_char*));
    static void packetFileTask(FILE**, const char*);
    static void executePacketInformationUpdate(long long, long*, long long*, long long*, long*, long long*, long long*, char*);
};

} // namespace SizingControllers
#endif