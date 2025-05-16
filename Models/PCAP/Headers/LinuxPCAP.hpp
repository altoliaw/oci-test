#pragma once
/** @file LinuxPCAP.hpp
 * The PCAP class for Linux
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */
#ifdef __linux__  // Linux

#include <unistd.h>

#include <iostream>
#include <string>
// The linux's pcap file, not PF-RING's one
#include <pcap.h>

#include <vector>

#include "./PCAPPrototype.hpp"
namespace PCAP {
/**
 * The inherited class to the parent class, PCAPPrototype
 */
class LinuxPCAP : public PCAPPrototype {
   public:
    pcap_t* pcapDescriptor;  // The pointer for referring to the variable "handle" from the prototype
    LinuxPCAP();
    virtual ~LinuxPCAP();
    void open(const char*, const int, const int, const int, std::vector<int>*);
    void execute(void (*)(u_char*, const pcap_pkthdr*, const u_char*) = nullptr);
    void close(void);
    
    static void show(void);

   private:
    static void packetHandler(u_char*, const struct pcap_pkthdr*, const u_char*);
};


}  // namespace PCAP
#endif