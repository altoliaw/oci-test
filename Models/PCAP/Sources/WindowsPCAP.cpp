/**
 * @see WindowsPCAP.hpp
 */
#include "../Headers/WindowsPCAP.hpp"

#ifdef _WIN32
namespace PCAP {
/**
 * Constructor
 */
WindowsPCAP::WindowsPCAP() {
    errBuff[0] = '\0';
    descriptor = nullptr;
    pcapDescriptor = (pcap_t*)descriptor;
    deviceInterface = "";
    rxSize = 0;
    txSize = 0;
    rxPacketNumber = 0;
    txPacketNumber = 0;
    maxRxSize = 0;
    maxTxSize = 0;
}

/**
 * Destructor
 */
WindowsPCAP::~WindowsPCAP() {
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
void WindowsPCAP::open(const char* device, const int snaplen, const int promisc, const int timeout, std::vector<int>* port) {
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
void WindowsPCAP::execute(void (*callback)(u_char*, const pcap_pkthdr*, const u_char*)) {
    if (pcapDescriptor != nullptr) {
        // The forth argument in the pcap_loop will be associated to the first one parameter in the function, callback.
        // If the callback is not nullptr, the forth argument is the object.
        pcap_loop(pcapDescriptor,
                  0,
                  ((callback == nullptr) ? WindowsPCAP::packetHandler : callback),                                        // if callback is nullptr,
                                                                                                                          // the function will be the default function in the class
                  (callback == nullptr) ? reinterpret_cast<u_char*>(&rxPacketNumber) : reinterpret_cast<u_char*>(this));  // if callback is nullptr,
                                                                                                                          // the function will be the default function in the class
    }
}

/**
 * Closing the PCAP
 */
void WindowsPCAP::close() {
    if (pcapDescriptor != nullptr) {
        pcap_close(pcapDescriptor);
        pcapDescriptor = nullptr;
    }
}

/**
 * Calculating the amount of the packets
 *
 * @param userData [u_char*] The additional information for the function, packetHandler; the additional information will be binding with
 * the forth argument in the pcap_loop
 * @param pkthdr [const struct pcap_pkthdr*] The header of the packet
 * @param packet [const u_char*] The data from the last position of the header of the packet
 */
void WindowsPCAP::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    int* packetCount = (int*)userData;
    (*packetCount)++;
    std::cout << *packetCount << "  packets\t";

    long* totalSize = (long*)(userData + sizeof(int));
    *totalSize += pkthdr->len;
    std::cout << pkthdr->len << "  total size\n";
    // Unit: milliseconds; 2000 milliseconds = 2 seconds
    Sleep(2000);
}

/**
 * Displaying the devices for .Json settings; this function is for windows platform because
 * the interface name on windows platform is involved with the registered machine codes; as
 * a result, the on windows platform, the editor shall provide device information for users'
 * json definition
 */
void WindowsPCAP::show() {
    pcap_if_t* allDevices;  // All devices (linked list)
    pcap_if_t* device;      // Each device pointer

    char errBuff[PCAP_ERRBUF_SIZE];  // For error message
    // Obtaining all devices into the device list; when there are no devices here, -1 will be obtained
    if (pcap_findalldevs(&allDevices, errBuff) == -1) {
        std::cerr << "Error in pcap_findalldevs: " << errBuff << "\n";
        return;
    }

    int index = 0;  // The index for device traversal
    std::cout << "The devices on Windows platform are listed with the format \"$1: $2 ($3)\" in the following "
              << "where the {{$#}}s are denoted as the serial number, device name, and device description in sequence. "
              << "Please copy the device name into the .json file in the \"Settings\" directory.\n\n";
    for (device = allDevices; device != nullptr; device = device->next) {
        std::cout << ++index << ": " << device->name << "\t";
        if (device->description) {
            std::cout << " (" << device->description << ")" << "\n";
        } else {
            std::cout << " (No description available)" << "\n";
        }
    }

    // Releasing the memory which was allocated for the devices
    if (allDevices != nullptr) {
        pcap_freealldevs(allDevices);
        allDevices = nullptr;
    }
}

}  // namespace PCAP
#endif