#pragma once
/** @file WindowsNpcapPCAP.hpp
 * The PCAP class for Windows using Npcap
 *
 * @author Nick, Liao
 * @date 2025/10/20
 */
#include <winsock2.h>
#include <windows.h>


#include <iostream>
#include <string>
#include <vector>
// The windows's pcap file from Vendors (The included approach is bad
// because the third party files in nPcap are included by using "<...>");
// that implies that the include directory shall be assigned to the compiler
#include <pcap.h>


#include "./PCAPPrototype.hpp"
namespace PCAP {
/**
 * The inherited class to the parent class, PCAPPrototype
 */
class WindowsNpcapPCAP : public PCAPPrototype {
   public:
    pcap_t* pcapDescriptor;  // The pointer for referring to the variable "handle" from the prototype
    WindowsNpcapPCAP();      
    virtual ~WindowsNpcapPCAP();
    void open(const char*, const int, const int, const int, std::vector<int>*);
    void execute(void (*)(u_char*, const pcap_pkthdr*, const u_char*) = nullptr);
    void close(void);
    static void show(void);
    void pcap_breakloop(void);




   private:
    static void packetHandler(u_char*, const struct pcap_pkthdr*, const u_char*);
};


}  // namespace PCAP