/**
 * @see SizingMainCaller.hpp
 */
#include "../../Headers/SizingControllers/LinuxCentosSizingMainController.hpp"
#ifdef __linux__

// The macro variable is from the compiled process where is in the CMakeLists.txt file from the proper cpp file folder
// For more variable information, please refer to the file, namely .globalCmakeVariable.cmake
// When the OS_ID is not defined in the CMakeLists.txt file from the proper cpp file folder,
// the OS_ID will not be passed defined into compiled process
#ifdef OS_ID

namespace SizingControllers {
//===Global Declaration===
// Variables in .ini file
// Writing file path
char* _WRITING_FILE_LOCATION_ = nullptr;
// The time interval, "s" (the file will be recorded every "s" second(s))
unsigned int _WRITING_FILE_SECOND_ = 30;
// The output layout format type (default value is equal to 0.)
unsigned int _OUTPUT_LAYOUT_TYPE_ = 0;

// Determining if the "pcap_loop" shall be still working, 0x0: halting, 0x1: working
volatile char _IS_PCAP_WORKED_ = 0x1;
// Determining if the alarm shall be still working, 0x0: halting, 0x1: working
volatile char _IS_ALARM_WORKED_ = 0x1;
// Mutual locker
std::mutex _MUTEX_;

// Referring to the objects for stopping "pcap_loop"
std::vector<PCAP::PCAPPrototype*> _PCAP_POINTER_;
// The address of the global pointer referring to the file descriptor object
FILE** _FILE_POINTER_ = nullptr;

//===Static fields Declaration===
// For reserving the session's previous, the key is a tuple which combines sorted ip and port information;
// the second one is the session's previous packet type; the value is defined as follows: 0: undefined; 1: TX, and 2: RX
std::map<std::tuple <uint32_t, uint32_t, uint16_t, uint16_t>, char> LinuxCentosSizingMainController::sessionMap;
// For recording the maximum number of packets which contain SQL per second
long LinuxCentosSizingMainController::currentSqlMaxRequestNumberPerSec = 0;
// For reserving the starting time in the beginning or the updating time when the SQL statements receive
std::chrono::steady_clock::time_point LinuxCentosSizingMainController::startingTime = std::chrono::steady_clock::time_point::min();
// For recording the maximum size of tx packets per second
long LinuxCentosSizingMainController::currentMaxTxSizePerSec = 0;
// For recording the maximum size of rx packets per second
long LinuxCentosSizingMainController::currentMaxRxSizePerSec = 0;
// For reserving the starting time in the beginning or the updating time when receiving a tx packet
std::chrono::steady_clock::time_point LinuxCentosSizingMainController::startingTimeTX = std::chrono::steady_clock::time_point::min();
// For reserving the starting time in the beginning or the updating time when receiving a rx packet
std::chrono::steady_clock::time_point LinuxCentosSizingMainController::startingTimeRX = std::chrono::steady_clock::time_point::min();
/**
 * The starting process, the entry of the process
 *
 * @param argC [int] The number of the argument
 * @param argV [char**] The array of the argument
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors LinuxCentosSizingMainController::start(int argC, char** argV) {
    Commons::POSIXErrors result = Commons::POSIXErrors::OK;

    // TODO: This section shall be implemented by using "Bison" instead of the section defined in the following.
    // To determine if the argument is passed for the execution
    if(argC == 2 && strcmp(argV[1], "-l") == 0) {
        // Showing the information
        PCAP::LinuxPCAP::show();
        return result;
    }

    // The data structure from the configure function
    // The array for reserving the interface name
    std::vector<unitService> interfaceNameArray;
    if (interfaceNameArray.empty() == false) {
        interfaceNameArray.clear();
        interfaceNameArray.shrink_to_fit();
    }

    // Calling the config function
    result = config(&interfaceNameArray);
    if (result != Commons::POSIXErrors::OK) {
        return result;
    }

    // The starting postion of the process
    // The output path
    char* OutputFilePathRule = (char*)"Outputs/trafficMonitor_%lu.tsv";

    // Obtaining the epoch
    char OuputFilePathWithTime[128] = {'\0'};
    sprintf(OuputFilePathWithTime, OutputFilePathRule, Commons::Time::getEpoch());
    _WRITING_FILE_LOCATION_ = OuputFilePathWithTime;

    // Installing a signal handler, interrupt
    signal(SIGINT, LinuxCentosSizingMainController::signalInterruptedHandler);

    {  // Creating objects, opening the interfaces, executing the packet calculations
        // and closing the interfaces; the number of objects is equal to the number of
        // the interfaces
        std::vector<PCAP::LinuxPCAP*> pcapObjectOfInterface;  // Here each element shall be a pointer because there exist a pointer which refers to a
                                                              // resource in the class. When the vector reserve objects, the destructor will occur twice in the following loop.
                                                              // The best approach is used the dynamic memory allocation with pointers.
        for (unsigned int i = 0; i < interfaceNameArray.size(); i++) {
            PCAP::LinuxPCAP* pcapObject = new PCAP::LinuxPCAP();
            pcapObject->open(interfaceNameArray[i].interfaceName, BUFSIZ, 1, 1000, &(interfaceNameArray[i].port));

            // Putting each pcap object into thread array
            pcapObjectOfInterface.push_back(pcapObject);
        }

        // Releasing the memory from the config because the config information has been reserved into each pcap
        if (interfaceNameArray.empty() == false) {
            interfaceNameArray.clear();
            interfaceNameArray.shrink_to_fit();
        }

        FILE* fileDescriptor = nullptr;
        // n + 1  threads created; the n is equal to the number of interfaces;
        // When calling the functions in threads, the values in the thread imply function name,
        // function argument 1, function argument2 and so on;
        // the first type threads (n from "n + 1")
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < pcapObjectOfInterface.size(); i++) {
            // A thread can be registered by using the command as below,
            // "std::thread packetThread{packetTask, &pcapObject, packetHandler};".
            // Therefore, users can use emplace_back on vector to construct the one immediately.
            threads.emplace_back(packetTask, pcapObjectOfInterface[i], packetHandler);

            // Passing the pcap object to the global pointer
            _PCAP_POINTER_.push_back(pcapObjectOfInterface[i]);
        }

        // The second type thread (1 from "n + 1")
        std::thread writePacketFileThread{packetFileTask, &fileDescriptor, OuputFilePathWithTime};

        // When the functions finish or interrupt, those n + 1 threads shall
        // be joined into the main process
        for (unsigned int i = 0; i < pcapObjectOfInterface.size(); i++) {
            threads[i].join();
        }
        writePacketFileThread.join();

        // All pcap objects shall call the close function
        for (unsigned int i = 0; i < pcapObjectOfInterface.size(); i++) {
            if (pcapObjectOfInterface[i] != nullptr) {
                (pcapObjectOfInterface[i])->close();
                delete (pcapObjectOfInterface[i]);
            }
            pcapObjectOfInterface[i] = nullptr;
        }
        // Closing the file descriptor
        if (fileDescriptor != nullptr) {
            fclose(fileDescriptor);
            fileDescriptor = nullptr;
        }
    }

    return result;
}

/**
 * Setting the config to this application
 *
 * @param services [std::vector<unitService>*] The data structure which is the key "service"
 * defined in the .json file
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status
 * defined in the class "POSIXErrors"
 */
Commons::POSIXErrors LinuxCentosSizingMainController::config(std::vector<unitService>* services) {
    Commons::POSIXErrors error = Commons::POSIXErrors::OK;

    // Loading information from the .json file for the application
    // The current working directory is the project root; as a result, the related path is shown as follows.
    const unsigned char* path = (const unsigned char*)"Settings/.Json/SizingMain.json";
    FileParsers::InitializedJsonFileParser::parseInitializedFile(path);

    // Obtaining the number of interfaces
    unsigned char serviceJsonString[2048] = {'\0'};
    // Obtaining the attribute, writingFileSecond, in the .json file
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.writingFileSecond", serviceJsonString);
    if (error != Commons::POSIXErrors::OK) {
        std::cerr << "base.writingFileSecond does not exist in the .json file.\n";
        return Commons::POSIXErrors::E_EXIST;
    }
    // Parsing the string into the unsigned int
    std::stringstream stream;
    stream.clear();  // Removing the error flags
    stream.str("");  // Removing the value
    stream << serviceJsonString;
    stream >> _WRITING_FILE_SECOND_;

    // Obtaining the attribute, outputLayoutType, in the .json file
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.outputLayoutType", serviceJsonString);
    if (error != Commons::POSIXErrors::OK) {
        std::cerr << "base.outputLayoutType does not exist in the .json file.\n";
        return Commons::POSIXErrors::E_EXIST;
    }
    // Parsing the string into the unsigned int
    stream.clear();  // Removing the error flags
    stream.str("");  // Removing the value
    stream << serviceJsonString;
    stream >> _OUTPUT_LAYOUT_TYPE_;

    // Obtaining the attribute, service, in the .json file
    cJSON* cJsonItem = nullptr;
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.service", serviceJsonString, &cJsonItem);
    if (error != Commons::POSIXErrors::OK && cJsonItem->type != cJSON_Array) {
        std::cerr << "base.service does not exist in the .json file.\n";
        return Commons::POSIXErrors::E_EXIST;
    }
    // Obtaining the length of the service array
    unsigned int serviceLength = (unsigned int)cJSON_GetArraySize(cJsonItem);  // Obtaining the size of the array from "base.service" in the .json file
    // The array for reserving the interface name
    // Traversal of "service" defined in the .json file
    for (unsigned int i = 0; i < serviceLength; i++) {
        std::string interfaceName = "base.service.[" + std::to_string(i) + "].interface";
        error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)(interfaceName.c_str()), serviceJsonString);
        if (error != Commons::POSIXErrors::OK) {
            std::cerr << "base.service.[i].interface does not exist in the .json file.\n";
            break;
        }

        // Copying the interface value into the element of the array
        unitService unit;
        memcpy(unit.interfaceName, serviceJsonString, strlen((char*)serviceJsonString));
        unit.interfaceName[strlen((char*)serviceJsonString)] = '\0';

        // Reserving the ports into the unitService
        interfaceName = "base.service.[" + std::to_string(i) + "].port";
        error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)(interfaceName.c_str()), serviceJsonString, &cJsonItem);
        if (error != Commons::POSIXErrors::OK && cJsonItem->type != cJSON_Array) {
            std::cerr << "base.service.[i].port does not exist in the .json file.\n";
            return Commons::POSIXErrors::E_EXIST;
        }

        // Obtaining the port array size
        unsigned int portLength = (unsigned int)cJSON_GetArraySize(cJsonItem);
        unsigned int portNumber = 0;
        for (unsigned int j = 0; j < portLength; j++) {
            interfaceName = "base.service.[" + std::to_string(i) + "].port.[" + std::to_string(j) + "]";
            error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)(interfaceName.c_str()), serviceJsonString);
            if (error != Commons::POSIXErrors::OK) {
                std::cerr << "base.service.[i].port.[j] does not exist in the .json file.\n";
                break;
            }
            stream.clear();  // Removing the error flags
            stream.str("");  // Removing the value
            stream << serviceJsonString;
            stream >> portNumber;
            unit.port.push_back(portNumber);
        }
        // Pushing the unit into the "services"
        services->push_back(unit);
    }
    // For verifying the previous loop
    if (error != Commons::POSIXErrors::OK) {
        return Commons::POSIXErrors::E_EXIST;
    }

    return Commons::POSIXErrors::OK;
}

/**
 * The function for the first type of the threads (n threads), packetThread; the task is to execute the "pcap_loop"
 *
 * @param pcap [PCAP::LinuxPCAP*] The address of the PCAP::LinuxPCAP object
 * @param packetHandler [void (*)(u_char*, const pcap_pkthdr*, const u_char*)] The callback function for pcap_loop
 */
void LinuxCentosSizingMainController::packetTask(PCAP::LinuxPCAP* pcap, void (*packetHandler)(u_char*, const pcap_pkthdr*, const u_char*)) {
    // The only argument will be set; as a result, the pcap object will be passed in the function, packetHandler.
    // For more information, please refer to the function, execute(.).
    pcap->execute(packetHandler);
}

/**
 * The function for the second type of the thread, writePacketFileThread; the task is to write the packet
 * information into the the specified file
 *
 * @param fileDescriptor [FILE**] The address of the pointer of the FILE descriptor
 * which users defined in .json file.
 * @param filePath [const char*] The file path for recording the information
 */
void LinuxCentosSizingMainController::packetFileTask(FILE** fileDescriptor, const char* filePath) {
    // Installing a signal handler, alarm
    signal(SIGALRM, LinuxCentosSizingMainController::signalAlarmHandler);
    _FILE_POINTER_ = fileDescriptor;  // Passing to the global variable

    // The first calling the function
    alarm(_WRITING_FILE_SECOND_);

    // Opening the file with the file descriptor
    if (*_FILE_POINTER_ == nullptr) {
        *_FILE_POINTER_ = fopen(filePath, "a+");
        if (*_FILE_POINTER_ == nullptr) {
            std::cerr << "Error opening the file!\n";
            LinuxCentosSizingMainController::signalInterruptedHandler(0);  // Going to the end of the thread

        } else {  // Adding the header information in a line to the file
            // Outputing the title by the mechanism from the "services" defined in the Services/SizingServices/Sources/Transformer.cpp
            SizingServices::Transformer::defaultOutputLayoutType = (int)_OUTPUT_LAYOUT_TYPE_;  // Assigning the output format
            switch ((int)SizingServices::Transformer::defaultOutputLayoutType) {
                case SizingServices::Transformer::DEFAULT:
                    // Printing the title; e.g.. UTC\tType\tInterface\tPort ...
                    SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::TITLE, 11, fileno(*_FILE_POINTER_));
                    break;
                case SizingServices::Transformer::FLOWTYPE:
                    SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::TITLE, 9, fileno(*_FILE_POINTER_));
                    break;
            }

            if (*_FILE_POINTER_ != nullptr) {
                SizingServices::Transformer::releaseDescriptors();
                fclose(*_FILE_POINTER_);
                *_FILE_POINTER_ = nullptr;
            }
        }
    }

    // Using a global variable to verify if the interrupt occurs
    while (_IS_ALARM_WORKED_ == 0x1) {
        sleep(5);  // A routine clock checker
    }

    // Closing the file
    if (*_FILE_POINTER_ != nullptr) {
        fclose(*_FILE_POINTER_);
    }
}

/**
 * Calculating the amount of the packets, a callback function to throw into the PCAP module (user defined)
 *
 * @param userData [u_char*]
 * @param pkthdr [const struct pcap_pkthdr*] The address of the packet header
 * @param packet [const u_char*] The address of the packet
 */
void LinuxCentosSizingMainController::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    // Opening the clock when the value equals to "std::chrono::steady_clock::time_point::min()"
    if (startingTime == std::chrono::steady_clock::time_point::min()) {
        startingTime = std::chrono::steady_clock::now(); // Assign now to the startingTime variable
    }

    // Opening the clock when the value equals to "std::chrono::steady_clock::time_point::min()"
    if (startingTimeTX == std::chrono::steady_clock::time_point::min()) {
        startingTimeTX = std::chrono::steady_clock::now();  // Assign now to the startingTime variable
    }

    // Opening the clock when the value equals to "std::chrono::steady_clock::time_point::min()"
    if (startingTimeRX == std::chrono::steady_clock::time_point::min()) {
        startingTimeRX = std::chrono::steady_clock::now();  // Assign now to the startingTime variable
    }

    // Due to the setting of the function, execute(.), the data of userData is the object of children classes (LinuxPCAP, WindowsPCAP and so on ...)
    PCAP::PCAPPrototype* pcapInstance = (PCAP::PCAPPrototype*)userData;
    // Determining what the instance belong to
    PCAP::LinuxPCAP* linuxPCAP = nullptr;
    if (dynamic_cast<PCAP::LinuxPCAP*>(pcapInstance)) {
        linuxPCAP = dynamic_cast<PCAP::LinuxPCAP*>(pcapInstance);
    }

    // When the pcap belongs to linux pcap, ...
    if (linuxPCAP != nullptr) {
        std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>* tmpMap = &(linuxPCAP->portRelatedInformation); // Due to less ports in the setting
        
        // Obtaining the IP header; the ip_p column implies the protocol;
        // the number of the TCP is 6, and the UDP is 17
        ip* ip_header = (ip*)(packet + sizeof(ether_header));

        // Preparing the headers and the packet source/destination port variables
        tcphdr* tcpHeader = nullptr;
        udphdr* udpHeader = nullptr;
        uint16_t packetSourcePort = 0;
        uint16_t packetDestinationPort = 0;
        uint32_t packetSourceIp = 0;
        uint32_t packetDestinationIp = 0;
        // Preparing the flag information of the tcp;
        // when the flag of the tcp is equal to 0x18, the packet belongs to SQL packets
        uint8_t tcpFlag = 0;

        bool isKnownProtocol = true;
        // Determining the protocol (TCP or UDP)
        switch (ip_header->ip_p) {
            case IPPROTO_TCP:  // TCP
                tcpHeader = (tcphdr*)(packet + sizeof(ether_header) + sizeof(ip));
                packetSourcePort = ntohs(tcpHeader->source);
                packetDestinationPort = ntohs(tcpHeader->dest);
                tcpFlag = 0x0;
                tcpFlag |= tcpHeader->fin  << 0;
                tcpFlag |= tcpHeader->syn  << 1;
                tcpFlag |= tcpHeader->rst  << 2;
                tcpFlag |= tcpHeader->psh  << 3;
                tcpFlag |= tcpHeader->ack  << 4;
                tcpFlag |= tcpHeader->urg  << 5;
                packetSourceIp = ip_header->ip_src.s_addr;
                packetDestinationIp = ip_header->ip_dst.s_addr;
                break;
            case IPPROTO_UDP:  // UDP
                udpHeader = (udphdr*)(packet + sizeof(ether_header) + sizeof(ip));
                packetSourcePort = ntohs(udpHeader->source);
                packetDestinationPort = ntohs(udpHeader->dest);
                packetSourceIp = ip_header->ip_src.s_addr;
                packetDestinationIp = ip_header->ip_dst.s_addr;
                break;
            default:
                // Skipping (unknown)
                isKnownProtocol = false;
        }

        // If the protocol is unkown, the process shall be returned.
        if (isKnownProtocol == false) {
            return;
        }

        // Making a tuple with a sorted packet Ip; this will be the key in the session map
        std::tuple<uint32_t, uint32_t, uint16_t, uint16_t> sortedSessionTuple;
        if (packetSourceIp < packetDestinationIp) {
            sortedSessionTuple = std::make_tuple(packetSourceIp, packetDestinationIp, packetSourcePort, packetDestinationPort);
        } else {
            sortedSessionTuple = std::make_tuple(packetDestinationIp, packetSourceIp, packetDestinationPort, packetSourcePort);
        }

        char previousPacketType = 0x0; // Undefined (0x0); 0x1: TX, and 0x2: RX
        // Critical section, accessing the data area
        _MUTEX_.lock();

        // Operating the sessionMap, using the emplace for verifying if the session key has been existed;
        // the returned value contains a pair consisting of an iterator to the inserted element 
        // (or to the element that prevented the insertion) and a bool value;
        // when the key exists, the returned second value is false (e.g., insert failed); when the key does not
        // exist, the returned second value is true (e.g., insert success)
        std::pair<std::map<std::tuple<uint32_t, uint32_t, uint16_t, uint16_t>, char>::iterator, bool> insertedResult = SizingControllers::LinuxCentosSizingMainController::sessionMap.emplace(sortedSessionTuple, previousPacketType);
        if (insertedResult.second == true) { // Key will inserted ...
            // Do nothing
        } else { // Key exist
            previousPacketType = (insertedResult.first)->second;
        }
        

        // Comparing source and destination ports with the port to determine the direction
        char packetTypeDetermineSet = 0x0;  // A flag to check if the packet type has been determined
        // For readability, the author uses a variable, packetTypeDetermineSet, to determine the type of the packet. That implies that
        // a packet only belongs a type to demonstrate the phenomenons of mutual exclusion. The two sections are provided.
        if (packetTypeDetermineSet == 0x0) {  // First, TX packet section; when the packet does not hit the port map
            std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it = tmpMap->find((int)packetSourcePort);
            if (it != tmpMap->end()) {  // Hitting
                // Updating the TX information
                executePacketInformationUpdate(
                    (long long)(pkthdr->len),
                    &((it->second)->txPacketNumber),
                    &((it->second)->txSize),
                    &((it->second)->maxTxSize),
                    &(linuxPCAP->txPacketNumber),
                    &(linuxPCAP->txSize),
                    &(linuxPCAP->maxTxSize),
                    &packetTypeDetermineSet
                );

                {  // Determining if the time has been equal to and larger than 1 sec (tx max size per sec)
                    currentMaxTxSizePerSec += (long)(pkthdr->len);
                    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedSeconds = now - startingTimeTX;
                    if (elapsedSeconds.count() >= 1.0) {
                        // Determining if the kept data are lager than current reserved data in the same session
                        (it->second)->maxTxSizePerSec = ((it->second)->maxTxSizePerSec) > currentMaxTxSizePerSec ? ((it->second)->maxTxSizePerSec) : currentMaxTxSizePerSec;
                        startingTimeTX = now;
                        currentMaxTxSizePerSec = 0;
                    }
                }

                // Determining if the cyclic direction packets have been detected
                // When flow change occurs, the previous packet is "undefined" or "RX"
                if (previousPacketType == 0x0 || previousPacketType == 0x2) {
                    (it->second)->flowChangeNumber++; // Flow change occurs, the previous packet is RX or nothing
                    (it->second)->rxGroupNumber++; // rxGroupNumber in the port shall plus 1.
                    (insertedResult.first)->second = previousPacketType = 0x1; // Setting the previous packet type to TX
                    linuxPCAP->rxGroupNumber++; // rxGroupNumber shall plus 1.

                }
            }
        }

        if (packetTypeDetermineSet == 0x0) {  // Second, RX packet; when the packet does not hit the port map
            std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it = tmpMap->find((int)packetDestinationPort);
            if (it != tmpMap->end()) {  // Hitting
                // Updating the RX information
                executePacketInformationUpdate(
                    (long long)(pkthdr->len),
                    &((it->second)->rxPacketNumber),
                    &((it->second)->rxSize),
                    &((it->second)->maxRxSize),
                    &(linuxPCAP->rxPacketNumber),
                    &(linuxPCAP->rxSize),
                    &(linuxPCAP->maxRxSize),
                    &packetTypeDetermineSet
                );

                {  // Determining if the time has been equal to and larger than 1 sec (rx max size per sec)
                    currentMaxRxSizePerSec += (long)(pkthdr->len);
                    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedSeconds = now - startingTimeRX;
                    if (elapsedSeconds.count() >= 1.0) {
                        // Determining if the kept data are lager than current reserved data in the same session
                        (it->second)->maxRxSizePerSec = ((it->second)->maxRxSizePerSec) > currentMaxRxSizePerSec ? ((it->second)->maxRxSizePerSec) : currentMaxRxSizePerSec;
                        startingTimeRX = now;
                        currentMaxRxSizePerSec = 0;
                    }
                }

                // Determining if the cyclic direction packets have been detected
                // When flow change occurs, the previous packet is "undefined" or "TX"
                if (previousPacketType == 0x0 || previousPacketType == 0x1) {
                    (it->second)->flowChangeNumber++; // Flow change occurs, the previous packet is RX
                    (it->second)->txGroupNumber++; // txGroupNumber in the port shall plus 1.
                    (insertedResult.first)->second = previousPacketType = 0x2; // Setting the previous packet type to TX
                    linuxPCAP->txGroupNumber++; // txGroupNumber shall plus 1.

                    // In this if section, the meaning implies that the packet from the client to server contain a SQL statement (cyclic direction + PSH + ACK)
                    if (tcpFlag == 0x18) { // PSH + ACK flag
                        (it->second)->sqlRequestNumber++;
                        (it->second)->sqlRequestSize += (long long)(pkthdr->len);
                        currentSqlMaxRequestNumberPerSec++; // Adding the number
                        // Determining if the time has been equal to and larger than 1 sec
                        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                        std::chrono::duration<double> elapsedSeconds = now - startingTime;
                        if ( elapsedSeconds.count() >= 1.0) {
                            // Determining if the kept data are lager than current reserved data in the same session 
                            (it->second)->sqlMaxRequestNumberPerSec = ((it->second)->sqlMaxRequestNumberPerSec) > currentSqlMaxRequestNumberPerSec ?
                                                                    ((it->second)->sqlMaxRequestNumberPerSec):
                                                                    currentSqlMaxRequestNumberPerSec;
                            startingTime = now;
                            currentSqlMaxRequestNumberPerSec = 0;
                        }
                    }
                }
            }
        }

        // Critical section end
        _MUTEX_.unlock();
    }

    // Verifying if the "pcap_loop" shall be stopped; "_IS_PCAP_WORKED_" is
    // a global variable and is controlled by the signal mechanism
    if (_IS_PCAP_WORKED_ == 0x0) {
        pcap_breakloop((pcap_t*)linuxPCAP->descriptor);
    }
}

/**
 * Updating the RX and TX packet information (according to the parameter)
 * 
 * @param packetHeaderLength [long long] The packet header length
 * @param portXPacketNumber [long*] The TX/RX packet number from the defined ports
 * @param portXSize [long long*] The TX/RX packet size from the defined ports
 * @param portMaxXSize [long long*] The TX/RX max packet size from the defined ports
 * @param XPacketNumber [long*] The TX/RX packet number
 * @param XSize [long long*] The TX/RX packet size
 * @param maxXSize [long long*] The TX/RX max packet size
 * @param packetTypeDetermineSet [char] The flag if the section is exected; 0 is not executed, and 1 is executed
 * @return [void] None
 */
void LinuxCentosSizingMainController::executePacketInformationUpdate(long long packetHeaderLength,
                                long* portXPacketNumber,
                                long long* portXSize,
                                long long* portMaxXSize,
                                long* XPacketNumber,
                                long long* XSize,
                                long long* maxXSize,
                                char* packetTypeDetermineSet) {
    
    (*portXPacketNumber)++;              // rx/txPacketNumber in the port shall plus 1.
    (*portXSize) += packetHeaderLength;  // rx/txSize in the port shall plus the current one.

    // Obtaining the rx/txmaximum size in the port
    if ((*portMaxXSize) < packetHeaderLength) {
        (*portMaxXSize) = packetHeaderLength;
    }

    (*XPacketNumber)++;                  // rx/txPacketNumber shall plus 1.
    (*XSize) += packetHeaderLength;      // rx/txSize shall plus the current one.
    
    // Obtaining the rx/tx maximum size
    if ((*maxXSize) < packetHeaderLength) {
        (*maxXSize) = packetHeaderLength;
    }
    (*packetTypeDetermineSet) = 0x1;
}

/**
 * A handler when receiving the SIGINT signal
 *
 * @param [int] The signal type (ignore)
 */
void LinuxCentosSizingMainController::signalInterruptedHandler(int) {
    std::cerr << "\n"
              << "Interrupted signal occurs, please wait.\n";
    // Using these two global variables to break the loops in different threads
    _IS_PCAP_WORKED_ = 0x0;
    _IS_ALARM_WORKED_ = 0x0;
    alarm(0);
}

/**
 * A handler when receiving the SIGALRM signal; in the function, the main task is
 * writing the packet information to the file
 *
 * @param signalType [int] The signal type and the parameter is useless in this method
 */
void LinuxCentosSizingMainController::signalAlarmHandler(int) {
    // File writing
    if (*_FILE_POINTER_ == nullptr) {
        // Opening the file
        *_FILE_POINTER_ = fopen(_WRITING_FILE_LOCATION_, "a+");

        if (*_FILE_POINTER_ == nullptr) {
            std::cerr << "Error opening the file!\n";
            LinuxCentosSizingMainController::signalInterruptedHandler(0);  // Going to the end of the thread

        } else {
            SizingServices::Transformer::defaultOutputLayoutType = (int)_OUTPUT_LAYOUT_TYPE_;  // Assigning the output format
            _MUTEX_.lock();
            // "UTC\tType\tPort\tNumber(amount)\tSize(byte)\tMaxSize\tSQL number per time interval(eps)\tSQL size per time interval(eps)\n";
            time_t timeEpoch = Commons::Time::getEpoch();

            // Looping all pcap object for printing the results
            for (std::vector<PCAP::PCAPPrototype*>::iterator it = _PCAP_POINTER_.begin();
                 it != _PCAP_POINTER_.end();
                 it++) {
                if (dynamic_cast<PCAP::LinuxPCAP*>(*it)) {
                    // Passing the object to the correct type
                    PCAP::LinuxPCAP* tmp = dynamic_cast<PCAP::LinuxPCAP*>(*it);
                    for (std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it2 = (tmp->portRelatedInformation).begin();
                         it2 != (tmp->portRelatedInformation).end();
                         it2++) {
                        // TX part; in the section, the last two result will be to zero because the packets
                        // from the record set from the SQL server shall be ignored
                        // Outputing the format with arguments by the mechanism from the "services" defined in the Services/SizingServices/Sources/Transformer.cpp
                        switch ((int)SizingServices::Transformer::defaultOutputLayoutType) {
                            case SizingServices::Transformer::DEFAULT:
                                SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::FORMAT, 11, fileno(*_FILE_POINTER_),
                                                                          timeEpoch, "TX", (tmp->deviceInterface).c_str(), (it2)->first, tmp->txPacketNumber,
                                                                          tmp->txSize, tmp->maxTxSize, (long)0, (long long)0, (long long)0,
                                                                          (long long)0);
                                break;
                            case SizingServices::Transformer::FLOWTYPE:
                                SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::FORMAT, 9, fileno(*_FILE_POINTER_),
                                                                          timeEpoch, "TX", (tmp->deviceInterface).c_str(), (it2)->first, _WRITING_FILE_SECOND_,
                                                                          (tmp->txSize / (long)_WRITING_FILE_SECOND_), (it2->second)->maxTxSizePerSec, (long long)0, (long long)0);
                                break;
                        }
                        ((it2)->second)->txGroupNumber = 0;
                        ((it2)->second)->txPacketNumber = 0;
                        ((it2)->second)->txSize = 0;
                        ((it2)->second)->maxTxSize = 0;

                        // RX part
                        // Outputing the format with arguments by the mechanism from the "services" defined in the Services/SizingServices/Sources/Transformer.cpp
                        switch ((int)SizingServices::Transformer::defaultOutputLayoutType) {
                            case SizingServices::Transformer::DEFAULT:
                                SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::FORMAT, 11, fileno(*_FILE_POINTER_),
                                                                          timeEpoch, "RX", (tmp->deviceInterface).c_str(), (it2)->first, tmp->rxPacketNumber,
                                                                          tmp->rxSize, tmp->maxRxSize, (it2->second)->sqlRequestNumber, (it2->second)->sqlRequestSize, (it2->second)->sqlRequestNumber / (long long)_WRITING_FILE_SECOND_,
                                                                          (it2->second)->sqlMaxRequestNumberPerSec);
                                break;
                            case SizingServices::Transformer::FLOWTYPE:
                                SizingServices::Transformer::printContent((unsigned int)SizingServices::Transformer::LayoutFormatAndStringType::FORMAT, 9, fileno(*_FILE_POINTER_),
                                                                          timeEpoch, "RX", (tmp->deviceInterface).c_str(), (it2)->first, _WRITING_FILE_SECOND_,
                                                                          (tmp->rxSize / (long)_WRITING_FILE_SECOND_), (it2->second)->maxRxSizePerSec, (it2->second)->sqlRequestNumber / (long long)_WRITING_FILE_SECOND_, (it2->second)->sqlMaxRequestNumberPerSec);
                                break;
                        }
                        ((it2)->second)->rxGroupNumber = 0;
                        ((it2)->second)->rxPacketNumber = 0;
                        ((it2)->second)->rxSize = 0;
                        ((it2)->second)->maxRxSize = 0;
                        ((it2)->second)->sqlRequestNumber = 0;
                        ((it2)->second)->sqlRequestSize = 0;
                        ((it2)->second)->sqlMaxRequestNumberPerSec = 0;
                    }

                    // Clearing the rx and tx number, size and max size information when all ports' information is written
                    tmp->txPacketNumber = 0;
                    tmp->txGroupNumber = 0;
                    tmp->txSize = 0;
                    tmp->maxTxSize = 0;
                    tmp->rxPacketNumber = 0;
                    tmp->rxGroupNumber = 0;
                    tmp->rxSize = 0;
                    tmp->maxRxSize = 0;                   
                }
            }
            _MUTEX_.unlock();
            alarm(_WRITING_FILE_SECOND_);
            // Closing the file
            if (*_FILE_POINTER_ != nullptr) {
                SizingServices::Transformer::releaseDescriptors();
                fclose(*_FILE_POINTER_);
                *_FILE_POINTER_ = nullptr;
            }
        }
    } else {  // Closing the descriptor and skipping the handling in the ith loop
        SizingServices::Transformer::releaseDescriptors();
        fclose(*_FILE_POINTER_);
        *_FILE_POINTER_ = nullptr;
    }
}

}  // namespace SizingMainController
#endif
#endif