#pragma once
/** @file WindowsPCAP.hpp
 * The PCAP class for Windows
 *
 * @author Nick, Liao
 * @date 2024/08/19
 */
#ifdef _WIN32  // Windows
#include <functional>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>

#include "./PCAPPrototype.hpp"

/* This is for the default buffer size for displaying the network information by using window API. */
#define OUT_BUFF_lENGTH 15000
namespace PCAP {
/**
 * The inherited class to the parent class, PCAPPrototype
 */
class WindowsWinDivertPCAP : public PCAPPrototype {
   public:
    /* This class is for reserving the value for verifying if the simulated function, pcap_loop, 
       shall be executed.
     */
    class PcapSignal {
       public:
        bool isExecuted; /* This field is for determining whether the simulated function, pcap_loop, shall be executed.
                            When the value is 0, the simulated function shall be stopped; when the value is 1, the function 
                            shall be executed. */
        std::function<bool(bool)> checkPCAPSignal; // A lambda variable, for displaying the current signal in the simulated function, pcap_loop
        PcapSignal();
        virtual ~PcapSignal();
    };

    PcapSignal pcapSignalInfo; // The object for displaying the flag in the PCAP simulated function
    HANDLE pcapDescriptor;  // The pointer for referring to the variable "handle" from the prototype
    WindowsWinDivertPCAP();
    virtual ~WindowsWinDivertPCAP();
    void open(const char*, const int, const int, const int, std::vector<int>*);
    void execute(void (*)(u_char*, const pcap_pkthdr*, const u_char*) = nullptr);
    void close(void);
    void pcap_loop(HANDLE*, int, void (*)(u_char*, const pcap_pkthdr*, const u_char*), u_char*);
    void pcap_breakloop();
    static void show(void);

   private:
    static void packetHandler(u_char*, const pcap_pkthdr*, const u_char*);
};

}  // namespace PCAP
#endif