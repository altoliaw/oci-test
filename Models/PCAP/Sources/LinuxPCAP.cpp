/**
 * @see LinuxPCAP.hpp
 */
#include "../Headers/LinuxPCAP.hpp"
#ifdef __linux__
namespace PCAP {
/**
 * Constructor
 */
LinuxPCAP::LinuxPCAP() {
    errBuff[0] = '\0';
    descriptor = nullptr;
    pcapDescriptor = (pcap_t*)descriptor;
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
LinuxPCAP::~LinuxPCAP() {
    // PCAP handle shall be closed and NULL.
    if (pcapDescriptor != nullptr) {
        std::cerr << "[Error] pcapDescriptor shall has been deallocated.\n";
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
 * Opening the PCAP object according to an interface with different ports
 *
 * @param device [const char*] The interface
 * @param snaplen [const int] The upper bound of the bits for each packet
 * @param promisc [const int] Specifying that the device is to be put into promiscuous mode.
 * A value of 1 (True) turns promiscuous mode on.
 * @param timeout [const int] Timeout (milliseconds)
 * @param port [std::vector<int>*] The port of the server for distinguishing with the packets from rx and tx
 */
void LinuxPCAP::open(const char* device, const int snaplen, const int promisc, const int timeout, std::vector<int>* port) {
    pcapDescriptor = pcap_open_live(device, snaplen, promisc, timeout, errBuff);
    if (pcapDescriptor == nullptr) {
        std::cerr << "[Error] PCAP open failed; please verifying if the permission is root\n";
    }
    descriptor = (void*)pcapDescriptor;  // Passing the descriptor to the general type

    // Copying the NIC information into the object
    std::string deviceInterface(device);
    this->deviceInterface = deviceInterface;
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
void LinuxPCAP::execute(void (*callback)(u_char*, const pcap_pkthdr*, const u_char*)) {
    if (pcapDescriptor != nullptr) {
        // The forth argument in the pcap_loop will be associated to the first one parameter in the function, callback.
        // If the callback is not nullptr, the forth argument is the object.
        pcap_loop(pcapDescriptor,
                  0,
                  ((callback == nullptr) ? LinuxPCAP::packetHandler : callback),                                          // if callback is nullptr,
                                                                                                                          // the function will be the default function in the class
                  (callback == nullptr) ? reinterpret_cast<u_char*>(&rxPacketNumber) : reinterpret_cast<u_char*>(this)    // if callback is nullptr,
                                                                                                                          // the function will be the default function in the class
        );
    }
}

/**
 * Closing the PCAP
 */
void LinuxPCAP::close() {
    if (pcapDescriptor != nullptr) {
        pcap_close(pcapDescriptor);
        pcapDescriptor = nullptr;
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
void LinuxPCAP::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    int* packetCount = (int*)userData;
    (*packetCount)++;
    std::cout << *packetCount << "  packets\t";

    long* totalSize = (long*)(userData + sizeof(int));
    *totalSize += pkthdr->len;
    std::cout << pkthdr->len << "  total size\n";
    sleep(2);
}

/**
 * Displaying the devices for .Json settings; this function is for windows platform because
 * the interface name on windows platform is involved with the registered machine codes; as
 * a result, the on windows platform, the editor shall provide device information for users'
 * json definition
 */
void LinuxPCAP::show() {
    std::cout << "Please refer to the command, \"ifconfig\", manually.\n";
}

}  // namespace PCAP
#endif