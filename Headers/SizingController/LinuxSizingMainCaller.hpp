#pragma once
/** @file LinuxSizingMainCaller.hpp
 * The headers and global variables from other package for Linux programs
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */

#ifdef __linux__
#include <netinet/if_ether.h>  // For Ethernet header structure
#include <netinet/ip.h>        // For IP header structure
#include <netinet/tcp.h>       // For TCP header structure
#include <netinet/udp.h>       // For UDP header structure
#include <signal.h>            // For the alarm and interrupted signal
#include <stdio.h>             // For some C io functions
#include <stdlib.h>            // For some C functions
#include <unistd.h>            // For sleep and pid functions

#include <algorithm>  // For std::max
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../Models/Commons/Headers/IOSpecification.hpp"
#include "../../Models/Commons/Headers/POSIXErrors.hpp"
#include "../../Models/Commons/Headers/Time.hpp"
#include "../../Models/FileParsers/Headers/InitializedJsonFileParser.hpp"
#include "../../Models/PCAP/Headers/LinuxPCAP.hpp"
#include "./SizingMainCallerPrototype.hpp"

namespace SizingMainCaller {
class LinuxSizingMainCaller : public SizingMainCallerPrototype {
   public:
    Commons::POSIXErrors start(int, char**);
    
    static void signalInterruptedHandler(int);
    static void signalAlarmHandler(int);
    static Commons::POSIXErrors config(std::vector<unitService>*);
    static void packetHandler(u_char*, const struct pcap_pkthdr*, const u_char*);
    static void packetTask(PCAP::LinuxPCAP*, void (*)(u_char*, const pcap_pkthdr*, const u_char*));
    static void packetFileTask(FILE**, const char*);
};
}  // namespace SizingMainCaller
#endif