/**
 * @see WindowsPCAP.hpp
 */
#include "../Headers/WindowsWinDivertPCAP.hpp"

#ifdef _WIN32
namespace PCAP {
/**
 * Constructor
 */
WindowsWinDivertPCAP::WindowsWinDivertPCAP() {
    errBuff[0] = '\0';
    descriptor = nullptr;
    pcapDescriptor = (HANDLE)descriptor; // HANDLE is the macro, thr original definition is equal to the pointer type.
    deviceInterface = "";
    rxSize = 0;
    txSize = 0;
    rxPacketNumber = 0;
    txPacketNumber = 0;
    rxGroupNumber = 0;
    txGroupNumber = 0;
    flowChangeNumber = 0;
    maxRxSize = 0;
    maxTxSize = 0;
}

/**
 * Destructor
 */
WindowsWinDivertPCAP::~WindowsWinDivertPCAP() {
    // PCAP handle shall be closed and NULL.
    if (pcapDescriptor != nullptr && pcapDescriptor != INVALID_HANDLE_VALUE) {
        this->close();
    } else {
        pcapDescriptor = nullptr;
        descriptor = nullptr;
    }
    errBuff[0] = '\0';
    deviceInterface = "";
    rxSize = 0;
    txSize = 0;
    rxPacketNumber = 0;
    txPacketNumber = 0;
    rxGroupNumber = 0;
    txGroupNumber = 0;
    flowChangeNumber = 0;
    maxRxSize = 0;
    maxTxSize = 0;

    // To ensure that the map is empty
    if (portRelatedInformation.empty() == false) {
        for (std::unordered_map<int, PCAPPortInformation*>::iterator it = portRelatedInformation.begin();
             it != portRelatedInformation.end();
             it++) {
            if (it->second != nullptr) {
                delete (it->second);
            }
            (it->second) = nullptr;
        }
        portRelatedInformation.clear();
    }
}


/**
 * Subclass constructor 
 */
WindowsWinDivertPCAP::PcapSignal::PcapSignal() {
    isExecuted = true; // Initializing the value is true for the simulated function, pcap_loop, working
    checkPCAPSignal = [this](bool input) { // Capturing by value; here the flag, input, 
                                           // determines whether the simulated pcap_loop continues or stops
        isExecuted &= input; // Calculating with & operator
        return isExecuted;
    };

    // Calling checkPCAPSignal with a true value
    checkPCAPSignal(true);
}

/**
 * Subclass destructor 
 */
WindowsWinDivertPCAP::PcapSignal::~PcapSignal() { 
    isExecuted = false;
    checkPCAPSignal = nullptr;
}


/**
 * Opening the PCAP object according to an interface with different ports
 *
 * @param device [const char*] The interface
 * @param snaplen [const int] The upper bound of the bits for each packet
 * @param promisc [const int] Specifying that the device is to be put into promiscuous mode.
 * A value of 1 (True) turns promiscuous mode on.
 * @param timeout [const int] Timeout (milliseconds)
 * @param port [std::vector<int>*] The port of the server for distinguishing with the packets from rx and tx
 */
void WindowsWinDivertPCAP::open(const char* device, const int snaplen, const int promisc, const int timeout, std::vector<int>* port) {

    /* Setting the variable of receiving packet filter; when the value is equal to "" or "true",
       that implies that the winDivert will receive all packets; when the value equals to "tcp or udp",
       that indicates that the winDivert will receive tcp or udp packets; for more information, 
       please refer to the URL as below: https://github.com/basil00/WinDivert/wiki/WinDivert-Documentation#filter_language.
     */
    // Copying the NIC information into the object
    std::string deviceInterface(device);
    this->deviceInterface = deviceInterface;
    std::string filter = "ifIdx == " + deviceInterface;
    // Opening the WinDivert handle
    pcapDescriptor = WinDivertOpen((const char*)filter.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0);
    if (pcapDescriptor == INVALID_HANDLE_VALUE) {
        std::cerr << "[Error] PCAP open failed; please verifying if the permission is root" << GetLastError() << "\n";
    }
    descriptor = (void*)pcapDescriptor;  // Passing the descriptor to the general type
    
    // Copying the ports information into each portRelatedInformation (set)
    for (unsigned int i = 0; i < port->size(); i++) {
        PCAPPortInformation* PCAPPortInstance = new PCAPPortInformation();
        PCAPPortInstance->port = (*port)[i];
        portRelatedInformation.emplace((*port)[i], PCAPPortInstance);
    }
}

/**
 * Looping for obtaining packets; if the developer does not pass the argument, the default static function, LinuxPCAP::packetHandler,
 * defined in class will be injected; otherwise, the user-defined function will be referred
 *
 * @param callback [void (*)(u_char*, const pcap_pkthdr*, const u_char*)] The callback function for pcap_loop;
 * the default value of the function is "nullptr" (has been initialized in the declaration)
 */
void WindowsWinDivertPCAP::execute(void (*callback)(u_char*, const pcap_pkthdr*, const u_char*)) {
    // When pcapDescriptor belongs to INVALID_HANDLE_VALUE, ...
    if (pcapDescriptor != INVALID_HANDLE_VALUE) {
        pcap_loop(&pcapDescriptor, 
                   0, 
                   ((callback == nullptr) ? WindowsWinDivertPCAP::packetHandler : callback), // if callback is nullptr, 
                    reinterpret_cast<u_char*>(this)
        );
    }
}

/**
 * Closing the PCAP
 */
void WindowsWinDivertPCAP::close() {
    if (pcapDescriptor != nullptr && pcapDescriptor != INVALID_HANDLE_VALUE) {
        WinDivertClose(pcapDescriptor);
        pcapDescriptor = INVALID_HANDLE_VALUE;  // Setting the handle to invalid value
    }
}

/**
 * Calculating the amount of the packets, a callback function to throw into the PCAP module (default)
 * When the outer does not throw the user defined callback function, the function below will execute automatically.
 *
 * @param userData [u_char*] The additional information for the function, packetHandler; the additional information will be binding with
 * the forth argument in the pcap_loop
 * @param pkthdr [const struct pcap_pkthdr*] The header of the packet (metadata)
 * @param packet [const u_char*] The data from the last position of the header of the packet
 */
void WindowsWinDivertPCAP::packetHandler(u_char* userData, const pcap_pkthdr* pkthdr, const u_char* packet) {
    // Due to the setting of the function, execute(.), the data of userData is the object of children classes (LinuxPCAP, WindowsPCAP and so on ...)
    PCAP::PCAPPrototype* pcapInstance = (PCAP::PCAPPrototype*)userData;
    // Determining what the instance belong to
    PCAP::WindowsWinDivertPCAP* windowsPCAP = nullptr;
    if (dynamic_cast<PCAP::WindowsWinDivertPCAP*>(pcapInstance)) {
        windowsPCAP = dynamic_cast<PCAP::WindowsWinDivertPCAP*>(pcapInstance);
        (windowsPCAP->rxGroupNumber) ++;
        std::cout << windowsPCAP->rxGroupNumber<< "  packets\t";
        long* totalSize = (long*)(userData + sizeof(int));
    }
    // Unit: milliseconds; 2000 milliseconds = 2 seconds
    Sleep(2000);
}

/**
 * The simulation of the pcap_loop; in winDirvert, there is no pcap_loop function in winDivert; as a result, the function shall be defined manually
 * 
 * @param pcapDescriptorPointer [HANDLE*] The pointer to the pcapDescriptorPointer
 * @param count [int] The number of packets to capture before the function returns; when the value is equal to 0 or -1, this implies that
 * the process will capture packets indefinitely until an error occurs or the loop is manually stopped; when the value is larger than 0, 
 * this implies that the process will capture exactly count packets
 * @param callback [void (*)(u_char*, const pcap_pkthdr*, const u_char*)] The callback function  
 */
void WindowsWinDivertPCAP::pcap_loop(HANDLE* pcapDescriptorPointer, int count, void (*callback)(u_char*, const pcap_pkthdr*, const u_char*), u_char* userDefinedData) {
    WindowsWinDivertPCAP* object = reinterpret_cast<WindowsWinDivertPCAP*>(userDefinedData); // Re-referring to the object with different pointers
    // The callback execution (implemented by the lambda as below:)
    std::function<bool()> receiveAndHandlePacket = [=]() -> bool {
         // Buffer for packet data information
        unsigned char packet[65535 + 64] = {'\0'}; // The size here is allocated 65535 + 64 bytes, which is larger than the maximum size announced by the WinDivert
        WINDIVERT_ADDRESS address;
        UINT packetLength = 0;
        if (!WinDivertRecv(*pcapDescriptorPointer, packet, sizeof(packet), &packetLength, &address)) { // When receiving nothing, ...
            DWORD error = GetLastError();
            if (error == ERROR_OPERATION_ABORTED) { // Checking for cancellation
                // Obtaining the signal "ctrl+c"
                // Doing nothing and returning false
                return false; // Exiting the loop
            } else {
                std::cerr << "[Error] Failed to receive packet.\n";
                return true;
            }
        } else {
            PWINDIVERT_IPHDR ipHeader = nullptr;
            PWINDIVERT_TCPHDR tcpHeader = nullptr;
            PWINDIVERT_UDPHDR udpHeader = nullptr;
            PVOID payload = nullptr;
            UINT payload_len = 0;
            if (packetLength >= sizeof(WINDIVERT_IPHDR)) {
                bool isSuccess = WinDivertHelperParsePacket( packet, packetLength, &ipHeader, nullptr, nullptr,
                    nullptr, nullptr, &tcpHeader, &udpHeader, &payload, 
                    &payload_len, nullptr, nullptr);
                
                // Assembling the ip header, TCP and UDP packet information;
                // in the winDivert, TCP and UDP headers cannot be obtained from ip header directly
                WINDIVERT_GROUP_TYPE packetGroupType = {ipHeader, tcpHeader, udpHeader, packetLength};

                if (isSuccess == true) {
                    //  Calling the callback function
                    WINDIVERT_GROUP_TYPE* ipHeader = &packetGroupType;
                    // WINDIVERT_IPHDR* ipHeader = (WINDIVERT_IPHDR*)(packet);
                    if (callback == nullptr) {
                        WindowsWinDivertPCAP::packetHandler((u_char*)&rxPacketNumber, ipHeader, packet);
                    } else {
                        callback(userDefinedData, ipHeader, packet);
                    }
                }
            }

            // When receiving packets, sending the packet without any changes
            if (!WinDivertSend(*pcapDescriptorPointer, packet, packetLength, nullptr, &address)) {
                std::cerr << "[Error] Failed to send packet back. Code: " << GetLastError() << "\n";
            }
            return true;
        }
    };

    // Loop case determination
    if (count == 0 || count == -1) { // Case infinity
        while (true && (object->pcapSignalInfo).checkPCAPSignal(true)) { // Verifying if the signal in the pcap had received a suspend/interrupt
            // When the lambda returns false, the process shall break out the loop. 
            bool isSuccess = receiveAndHandlePacket();
            if (isSuccess == false) {
                break;
            }
        }
    } else if (count > 0) { // Case finiteness
        while (count > 0 &&  (object->pcapSignalInfo).checkPCAPSignal(true)) {
            // When the lambda returns false, the process shall break out the loop. 
            bool isSuccess = receiveAndHandlePacket();
            if (isSuccess == false) {
                break;
            }
            count--;
        }
    }
}

/**
 * The simulation of the pcap_breakloop; in winDirvert, there is no pcap_breakloop function in winDivert; as a result, the function shall be defined manually
 * 
 */
void WindowsWinDivertPCAP::pcap_breakloop() {
    (this->pcapSignalInfo).checkPCAPSignal(false); 
}

/**
 * Displaying the devices for .Json settings; this function is for windows platform because
 * the interface name on windows platform is involved with the registered machine codes; as
 * a result, the on windows platform, the editor shall provide device information for users' 
 * json definition
 */
void WindowsWinDivertPCAP::show() {
    WSADATA wsaData; // An object for reserving the "window sockets application" (wsa) information for winsock 
                     // and the winsock introduction is list as below:
                     // https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Initializing the Winsock with the major and the minor versions are 2 and 2, respectively

    IP_ADAPTER_ADDRESSES* adapterAddresses = nullptr; // A window network interface pointer
    ULONG outBuffLength = (ULONG)OUT_BUFF_lENGTH; // The default size of the buffer
    DWORD dwReturnVal = 0; // double word size's return value

    // Using for loop to extend the memory buffer automatically; the for loop implies a retry mechanism; only 5 tries are in the case
    for (int attempts = 0; attempts < 5; attempts++) {
        adapterAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBuffLength); // Allocating the memory size dynamically
        if (!adapterAddresses) {
            std::cerr << "Memory allocation failed\n";
            return;
        }

        /* Obtaining all network interfaces' information from the NIC, for more details, please refer to th following URL:
         * https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses?utm_source=chatgpt.com */
        dwReturnVal = GetAdaptersAddresses( AF_UNSPEC, // Obtaining the IPV4 and IPV6 information
                                            GAA_FLAG_INCLUDE_PREFIX, // The address length
                                            NULL,
                                            adapterAddresses, // The related contents
                                            &outBuffLength // The size of the related contents
                      );

        if (dwReturnVal == ERROR_BUFFER_OVERFLOW) { // When the error code belongs to "buffer too small", 
            free(adapterAddresses);
            adapterAddresses = nullptr; 
            continue; // Trying again
        }
        // When accepted, ...
        break;
    }

    if (dwReturnVal != NO_ERROR) { // When error occurs, ...
        std::cerr << "GetAdaptersAddresses failed with error: " << dwReturnVal << "\n";
        if (adapterAddresses) {
            free(adapterAddresses);
            adapterAddresses = nullptr;
        }
        WSACleanup(); // Closing and cleaning the winsock
        return;
    }

    // Displaying the device information (i.e., the codes are simulated as the function, pcap_findalldevs, in NPCAP)
    int index = 0;
    for (IP_ADAPTER_ADDRESSES* adapter = adapterAddresses;
         adapter != nullptr; adapter = adapter->Next) {

        ULONG ifIndex = adapter->IfIndex; // Windows' Ifindex (this number is for the setting when using winDivert)
        std::string adapterName = "\\Device\\NPF_"; // On windows platforms, the prefix will display the prefix \"Device\NPF_\" and the uuid. 
        adapterName = (adapter->AdapterName) ? (adapterName + adapter->AdapterName ) : "Unknown";
        std::cerr << (++index) << ".\t" << adapterName << "  IfIndex: " << ifIndex  <<"\t";

        // Traversal of network adapters and their own unicast IP; (i.e., unicast implies a transformation from a node to a node)
        for ( IP_ADAPTER_UNICAST_ADDRESS* unicast = adapter->FirstUnicastAddress;
              unicast != nullptr; 
              unicast = unicast->Next ) {
            
            char ipStringBuffer[INET6_ADDRSTRLEN] = {'\0'};
            int family = unicast->Address.lpSockaddr->sa_family; // Obtaining the sa_family field indicates which protocol family this address belongs to

            if (family == AF_INET) { // Displaying the individual IP
                getnameinfo(
                    unicast->Address.lpSockaddr, // The pointer to the IP address (as sockaddr)
                    (family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6), // The address structure length based on protocol family
                    ipStringBuffer, // The output buffer for the resulting IP string
                    sizeof(ipStringBuffer), // The size of the output buffer
                    nullptr,
                    0,
                    NI_NUMERICHOST // Forcing numeric IP output (no DNS lookup)
                );
                std::cout << "IP Address: " << ipStringBuffer << "\n";
            }
        }
    }

    free(adapterAddresses); // Releasing the dynamic memory
    WSACleanup(); // Closing and cleaning the winsock
    return;
}

}  // namespace PCAP
#endif
