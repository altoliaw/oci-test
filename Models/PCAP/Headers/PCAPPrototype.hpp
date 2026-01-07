#pragma once
/** @file PCAPPrototype.hpp
 * The abstract class of PCAP
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */

// Definitions on different platforms
#ifdef __linux__
// The linux's pcap file, not PF-RING's one
#include <pcap.h>

#elif defined(_WIN32)
#ifdef OS_ID
// Importing the header of winDivert, the file is in the folder, Vendors/winDivert/Includes/
#include "windivert.h"

// Canceling the two marcos, __out and __in, defined in winDivert to avoid naming conflicts
#ifdef __out
#undef __out
#endif
#ifdef __in
#undef __in
#endif

/* In some windows platforms, u_char type is not defined; for ensuring that u_char can be used
 * on all windows, here the process will verify whether u_char exists (TODO, delete) */
#ifndef u_char
// Alias to the type unsigned char to the one, u_char
typedef unsigned char u_char;
#endif

/**
 * This part is for the winDivert's header, because the winDivert's header is not compatible with the pcap's header (pcap_pkthdr).
 */
namespace PCAP {
/**
 * This part is for the winDivert's header, because the winDivert's header is not compatible with the pcap's header (pcap_pkthdr).
 */
struct WINDIVERT_GROUP_TYPE {
    PWINDIVERT_IPHDR ipHeader;    // IP header
    PWINDIVERT_TCPHDR tcpHeader;  // TCP header
    PWINDIVERT_UDPHDR udpHeader;  // UDP header
    UINT packetLength;            // The length of the packet
};
}  // namespace PCAP

typedef PCAP::WINDIVERT_GROUP_TYPE pcap_pkthdr;
/* In winDivert, 'WINDIVERT_IPHDR' type is substituted for the 'pcap_pkthdr' type in Linux.
 * To co-compile on different platforms, 'WINDIVERT_IPHDR' shall be alias to 'pcap_pkthdr'. */
// typedef WINDIVERT_IPHDR pcap_pkthdr;

/* In addition to the pcap_pkthdr, in windDivert, PCAP_ERRBUF_SIZE is not defined. Hence, here
 * the macro shall be set as 256 manually */
#define PCAP_ERRBUF_SIZE 256
#else // For Npcap on Windows
#include <pcap.h>
#endif 

#endif

#include <string>
#include <unordered_map>
#include <vector>

namespace PCAP {
/**
 * The abstract class as the parent class
 */
class PCAPPrototype {
   public:
    /**
     * This structure is for reserving the information with a specified port.
     */
    struct PCAPPortInformation {
       public:
        int port;  // The port number for distinguishing to the packets of rx an tx
        // char previousPacketType;   // The previous packet type; 0: undefined; 1: TX, and 2: RX
        long long rxSize;      // The size of the rx channel with the port
        long long txSize;      // The size of the rx channel with the port
        long rxPacketNumber;   // The number of the packets in the rx channel with the port
        long txPacketNumber;   // The number of the packets in the tx channel with the port
        long long maxRxSize;   // The max size of the rx packets during the time interval with the port
        long long maxTxSize;   // The max size of the tx packets during the time interval with the port
        long maxRxSizePerSec;  // The max size of the rx packets with the port per a second
        long maxTxSizePerSec;  // The max size of the tx packets with the port per a second
        // The number of the packet groups in the rx channel within the port;
        // the group indicates a packet direction reversal followed by a return to the original direction (i.e., rx → tx → rx)
        long rxGroupNumber;
        // The number of the packet groups in the tx channel within the port;
        // the group indicates a packet direction reversal followed by a return to the original direction (i.e., tx → rx → tx)
        long txGroupNumber;
        // The number of the flow change during the time interval within the port;
        // the change indicates a packet direction reversal followed by a return to the original direction (i.e., tx → rx → tx)
        long flowChangeNumber;
        long sqlRequestNumber;           // The number if the packets containing SQL statement in rx with the port
        long long sqlRequestSize;        // The size if the packets containing SQL statement in rx with the port
        long sqlMaxRequestNumberPerSec;  // The number if the packets containing SQL statement in rx with the port
        PCAPPortInformation();
        ~PCAPPortInformation();
        long sqlRequestNumberPerTime(int);
    };

    std::unordered_map<int, PCAPPortInformation*> portRelatedInformation;  // The port related information
    char errBuff[PCAP_ERRBUF_SIZE];                                        // For error message
    void* descriptor;                                                      // PCAP descriptor
    std::string deviceInterface;                                           // The interface name
    // All information of the interface
    long long rxSize;     // The size of the rx channel
    long long txSize;     // The size of the rx channel
    long rxPacketNumber;  // The number of the packets in the rx channel
    long txPacketNumber;  // The number of the packets in the tx channel
    // The number of the packet groups in the rx channel within the port;
    // the group indicates a packet direction reversal followed by a return to the original direction (i.e., rx → tx → rx)
    long rxGroupNumber;
    // The number of the packet groups in the tx channel within the port;
    // the group indicates a packet direction reversal followed by a return to the original direction (i.e., tx → rx → tx)
    long txGroupNumber;
    // The number of the flow change during the time interval within the port;
    // the change indicates a packet direction reversal followed by a return to the original direction (i.e., tx → rx → tx)
    long flowChangeNumber;
    long long maxRxSize;  // The max size of the rx packets during the time interval
    long long maxTxSize;  // The max size of the tx packets during the time interval
    virtual void open(const char*, const int,
                      const int, const int, std::vector<int>*) = 0;  // "open(.)" approach
    virtual void execute(void (*)(u_char*, const pcap_pkthdr*,
                                  const u_char*)) = 0;  // Executing the loop for obtaining the packets;
                                                        // here the type pcap_pkthdr is for user-defined function
    virtual void close(void) = 0;                       // "close(.)" approach
};

}  // namespace PCAP