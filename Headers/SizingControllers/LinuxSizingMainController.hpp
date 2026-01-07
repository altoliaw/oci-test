#pragma once
/** @file LinuxSizingMainCaller.hpp
 * The headers and global variables from other package for Linux programs
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */

#ifdef __linux__

// The macro variable is from the compiled process where is in the CMakeLists.txt file from the proper cpp file folder
// For more variable information, please refer to the file, namely .globalCmakeVariable.cmake
// When the OS_ID is not defined in the CMakeLists.txt file from the proper cpp file folder,
// the OS_ID will not be passed defined into compiled process
#ifndef OS_ID
#include <netinet/if_ether.h>  // For Ethernet header structure
#include <netinet/ip.h>        // For IP header structure
#include <netinet/tcp.h>       // For TCP header structure
#include <netinet/udp.h>       // For UDP header structure
#include <signal.h>            // For the alarm and interrupted signal
#include <stdio.h>             // For some C io functions
#include <stdlib.h>            // For some C functions
#include <unistd.h>            // For sleep and pid functions

#include <algorithm>  // For std::max
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
#include "../../Models/PCAP/Headers/LinuxPCAP.hpp"
#include "../../Services/SizingServices/Headers/Transformer.hpp"
#include "./SizingMainControllerPrototype.hpp"

namespace SizingControllers {
class LinuxSizingMainController : public SizingMainControllerPrototype {
   public:
    // For reserving the session's previous, the key is a tuple which combines sorted ip and port information;
    // the second one is the session's previous packet type;
    // The value is defined as follows: 0: undefined; 1: TX, and 2: RX
    static std::map<std::tuple<uint32_t, uint32_t, uint16_t, uint16_t>, char> sessionMap;
    // For recording the maximum number of packets where contain SQL per second
    static long currentSqlMaxRequestNumberPerSec;
    // For reserving the starting time in the beginning or the updating time when the SQL statements receive
    static std::chrono::steady_clock::time_point startingTime;
    // For recording the maximum size of tx packets per second
    static long currentMaxTxSizePerSec;
    // For recording the maximum size of rx packets per second
    static long currentMaxRxSizePerSec;
    // For reserving the starting time in the beginning or the updating time when receiving a tx packet
    static std::chrono::steady_clock::time_point startingTimeTX;
    // For reserving the starting time in the beginning or the updating time when receiving a rx packet
    static std::chrono::steady_clock::time_point startingTimeRX;

    Commons::POSIXErrors start(int, char**);
    static void signalInterruptedHandler(int);
    static void signalAlarmHandler(int);
    static Commons::POSIXErrors config(std::vector<unitService>*);
    static void packetHandler(u_char*, const struct pcap_pkthdr*, const u_char*);
    static void packetTask(PCAP::LinuxPCAP*, void (*)(u_char*, const pcap_pkthdr*, const u_char*));
    static void packetFileTask(FILE**, const char*);
    static void executePacketInformationUpdate(long long, long*, long long*, long long*, long*, long long*, long long*, char*);
};
}  // namespace SizingMainCaller
#endif
#endif