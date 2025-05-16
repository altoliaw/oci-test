/**
 * @see WindowsSizingMainCaller.hpp
 */
#include "../../Headers/SizingController/WindowsSizingMainCaller.hpp"
#ifdef _WIN32

namespace SizingMainCaller {
//===Global Declaration===
// Variables in .ini file
// Writing file path
char* _WRITING_FILE_LOCATION_ = nullptr;
// The time interval, "s" (the file will be recorded every "s" second(s))
unsigned int _WRITING_FILE_SECOND_ = 30;

// Determining if the "pcap_loop" shall be still working, 0x0: halting, 0x1: working
volatile char _IS_PCAP_WORKED_ = 0x1;
// Determining if the alarm shall be still working, 0x0: halting, 0x1: working
volatile char _IS_ALARM_WORKED_ = 0x1;
// Mutual locker(Windows version), using by the critical section (only works on a process on Windows)
CRITICAL_SECTION _CRITICAL_SECTION_;
// For the windows timer
HANDLE _TIMER_ = nullptr;
// For the windows exit signal
HANDLE _EXITED_EVENT_ = nullptr;

// Referring to the objects for stopping "pcap_loop"
std::vector<PCAP::PCAPPrototype*> _PCAP_POINTER_;
// The address of the global pointer referring to the file descriptor object
FILE** _FILE_POINTER_ = nullptr;

/**
 * The starting process, the entry of the process
 *
 * @param argC [int] The number of the argument
 * @param argV [char**] The array of the argument
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors WindowsSizingMainCaller::start(int argC, char** argV) {
    Commons::POSIXErrors result = Commons::POSIXErrors::OK;

    // TODO: This section shall be implemented by using "Bison" instead of the section defined in the following.
    // To determine if the argument is passed for the execution
    if (argC == 2 && strcmp(argV[1], "-l") == 0) {
        // Showing the information
        PCAP::WindowsPCAP::show();
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
    SetConsoleCtrlHandler(WindowsSizingMainCaller::signalInterruptedHandler, TRUE);

    {                                                           // Creating objects, opening the interfaces, executing the packet calculations
                                                                // and closing the interfaces; the number of objects is equal to the number of
                                                                // the interfaces
        std::vector<PCAP::WindowsPCAP*> pcapObjectOfInterface;  // Here each element shall be a pointer because there exist a pointer which refers to a
                                                                // resource in the class. When the vector reserve objects, the destructor will occur twice in the following loop.
                                                                // The best approach is used the dynamic memory allocation with pointers.
        for (unsigned int i = 0; i < interfaceNameArray.size(); i++) {
            PCAP::WindowsPCAP* pcapObject = new PCAP::WindowsPCAP();
            pcapObject->open(interfaceNameArray[i].interfaceName, BUFSIZ, 1, 1000, &(interfaceNameArray[i].port));

            // Putting each pcap object into thread array
            pcapObjectOfInterface.push_back(pcapObject);
        }

        // Releasing the memory from the config because the config information has been reserved into each pcap
        if (interfaceNameArray.empty() == false) {
            interfaceNameArray.clear();
            interfaceNameArray.shrink_to_fit();
        }

        // Before the threads, the MUTEX locker on Windows shall be initialized.
        InitializeCriticalSection(&_CRITICAL_SECTION_);

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

        // After threads have joined, the mutex locker shall be released.
        DeleteCriticalSection(&_CRITICAL_SECTION_);

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
Commons::POSIXErrors WindowsSizingMainCaller::config(std::vector<unitService>* services) {
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
void WindowsSizingMainCaller::packetTask(PCAP::WindowsPCAP* pcap, void (*packetHandler)(u_char*, const pcap_pkthdr*, const u_char*)) {
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
void WindowsSizingMainCaller::packetFileTask(FILE** fileDescriptor, const char* filePath) {
    // Registering the handler, "exit event"; this variable is used in the WindowsSizingMainCaller::signalInterruptedHandler
    _EXITED_EVENT_ = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (_EXITED_EVENT_ == nullptr) {
        _IS_ALARM_WORKED_ = 0x0;  // Disabled alarm
        std::cerr << "[Error] CreateEvent failed.\n";
        return;
    }

    // Installing a signal handler, alarm, on Windows
    _TIMER_ = CreateWaitableTimer(nullptr, FALSE, nullptr);
    if (_TIMER_ == nullptr) {
        std::cerr << "[Error] Failed to create a waitable timer.\n";
        _IS_ALARM_WORKED_ = 0x0;  // Disabled alarm
        return;
    }

    _FILE_POINTER_ = fileDescriptor;  // Passing as the global variable
    // Opening the file with the file descriptor
    if (*_FILE_POINTER_ == nullptr) {
        *_FILE_POINTER_ = fopen(filePath, "a+");
        if (*_FILE_POINTER_ == nullptr) {
            std::cerr << "Error opening the file!\n";
            WindowsSizingMainCaller::signalInterruptedHandler(CTRL_C_EVENT);  // Going to the end of the thread

        } else {  // Adding the header information in a line to the file
            char output[1024] = {'\0'};
            int length = sprintf(output,
                                 "UTC\tType\tInterface\tPort\tNumber(amount)\tSize(bytes)\tMaxSize(bytes)\t"
                                 "SQL number in the time interval\tSQL size(bytes) in the time interval\tSQL number per time interval(eps)\n");
            fwrite(output, sizeof(char), length, *_FILE_POINTER_);
            if (*_FILE_POINTER_ != nullptr) {
                fclose(*_FILE_POINTER_);
                *_FILE_POINTER_ = nullptr;
            }
        }
    }

    // Setting the alarm information
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = (LONGLONG)(-1) * (10000000LL) * (LONGLONG)_WRITING_FILE_SECOND_;  // Setting the first execution time when the timer executes

    // Setting the alarm and the callback function, signalAlarmHandler, will awake every "(_WRITING_FILE_SECOND_ * 1000)" milliseconds;
    // when the SetWaitableTimer(.) successes, the timer will executes periodically
    if (!SetWaitableTimer(_TIMER_, &dueTime, (_WRITING_FILE_SECOND_ * 1000), nullptr, nullptr, FALSE)) {
        std::cerr << "[Error] Failed to create a waitable timer.\n";
        _IS_ALARM_WORKED_ = 0x0;  // Disabled alarm
        return;
    }

    // Registering the handlers
    HANDLE handles[] = {_TIMER_, _EXITED_EVENT_};

    while (_IS_ALARM_WORKED_ == 0x1) {
        // Waiting the handlers, _TIMER_ and _EXITED_EVENT_, asynchronously
        DWORD waitedResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        switch (waitedResult) {
            case WAIT_OBJECT_0:  // "_TIMER_" case
                signalAlarmHandler();

            case (WAIT_OBJECT_0 + 1):  // "_EXITED_EVENT_" case
                break;
        }
    }

    {  // Cleaning process
        // When the exited event does not belong to nullptr, ...
        if (_EXITED_EVENT_ != nullptr) {
            // Closing the timer
            CloseHandle(_EXITED_EVENT_);
            _EXITED_EVENT_ = nullptr;
        }

        // When the timer does not belong to nullptr, ...
        if (_TIMER_ != nullptr) {
            // Closing the timer
            CloseHandle(_TIMER_);
            _TIMER_ = nullptr;
        }

        // Closing the file if the descriptor does not belong to nullptr
        if (*_FILE_POINTER_ != nullptr) {
            fclose(*_FILE_POINTER_);
            *_FILE_POINTER_ = nullptr;
        }
    }
}

/**
 * Calculating the amount of the packets
 *
 * @param userData [u_char*]
 * @param pkthdr [const struct pcap_pkthdr*] The address of the packet header
 * @param packet [const u_char*] The address of the packet
 */
void WindowsSizingMainCaller::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    // Due to the setting of the function, execute(.), the data of userData is the object of children classes (LinuxPCAP, WindowsPCAP and so on ...)
    PCAP::PCAPPrototype* pcapInstance = (PCAP::PCAPPrototype*)userData;
    // Determining what the instance belong to
    PCAP::WindowsPCAP* windowsPCAP = nullptr;
    if (dynamic_cast<PCAP::WindowsPCAP*>(pcapInstance)) {
        windowsPCAP = dynamic_cast<PCAP::WindowsPCAP*>(pcapInstance);
    }

    // When the pcap belongs to linux pcap, ...
    if (windowsPCAP != nullptr) {
        std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>* tmpMap = &(windowsPCAP->portRelatedInformation);

        // The Ip collection, the variable will be static
        static std::unordered_map<uint32_t, char> ipMap;

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

        // Determining the protocol (TCP or UDP)
        switch (ip_header->ip_p) {
            case IPPROTO_TCP:  // TCP
                tcpHeader = (tcphdr*)(packet + sizeof(ether_header) + sizeof(ip));
                packetSourcePort = ntohs(tcpHeader->th_sport);
                packetDestinationPort = ntohs(tcpHeader->th_dport);
                tcpFlag = tcpHeader->th_flags;
                packetSourceIp = ip_header->ip_src.s_addr;
                packetDestinationIp = ip_header->ip_dst.s_addr;
                break;
            case IPPROTO_UDP:  // UDP
                udpHeader = (udphdr*)(packet + sizeof(ether_header) + sizeof(ip));
                packetSourcePort = ntohs(udpHeader->uh_sport);
                packetDestinationPort = ntohs(udpHeader->uh_dport);
                packetSourceIp = ip_header->ip_src.s_addr;
                packetDestinationIp = ip_header->ip_dst.s_addr;
                break;
            default:
                tcpHeader = (tcphdr*)(packet + sizeof(ether_header) + sizeof(ip));
                packetSourcePort = ntohs(tcpHeader->th_sport);
                packetDestinationPort = ntohs(tcpHeader->th_dport);
                tcpFlag = tcpHeader->th_flags;
                packetSourceIp = ip_header->ip_src.s_addr;
                packetDestinationIp = ip_header->ip_dst.s_addr;
        }

        // Critical section, accessing the data area
        EnterCriticalSection(&_CRITICAL_SECTION_);

        // Comparing source and destination ports with the port to determine the direction
        char packetTypeDetermineSet = 0x0;  // A variable to determine the type of the packet
        // For readability, the author uses a variable, packetTypeDetermineSet, to determine the type of the packet. That implies that
        // a packet only belongs a type to demonstrate the phenomenons of mutual exclusion.
        if (packetTypeDetermineSet == 0x0) {  // TX packet
            std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it = tmpMap->find((int)packetSourcePort);
            if (it != tmpMap->end()) {  // Hitting
                // previousPacketType[it->first] = 0x0;
                (it->second)->txPacketNumber++;                    // txPacketNumber in the port shall plus 1.
                (it->second)->txSize += (long long)(pkthdr->len);  // txSize in the port shall plus the current one.

                // Obtaining the maximum size in the port
                if ((it->second)->maxTxSize < (long long)(pkthdr->len)) {
                    (it->second)->maxTxSize = (long long)(pkthdr->len);
                }

                windowsPCAP->txPacketNumber++;                    // txPacketNumber shall plus 1.
                windowsPCAP->txSize += (long long)(pkthdr->len);  // txSize shall plus the current one.

                // Obtaining the maximum size
                if (windowsPCAP->maxTxSize < (long long)(pkthdr->len)) {
                    windowsPCAP->maxTxSize = (long long)(pkthdr->len);
                }
                packetTypeDetermineSet = 0x1;
            }
        }

        if (packetTypeDetermineSet == 0x0) {  // RX packet
            std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it = tmpMap->find((int)packetDestinationPort);
            if (it != tmpMap->end()) {  // Hitting
                // previousPacketType[it->first] = 0x1;
                (it->second)->rxPacketNumber++;                    // rxPacketNumber in the port shall plus 1.
                (it->second)->rxSize += (long long)(pkthdr->len);  // rxSize in the port shall plus the current one.

                // Obtaining the maximum size in the port
                if ((it->second)->maxRxSize < (long long)(pkthdr->len)) {
                    (it->second)->maxRxSize = (long long)(pkthdr->len);
                }

                // In this if section, the meaning implies that the packet from the client to server contain a SQL statement
                if (tcpFlag == 0x18) {
                    (it->second)->sqlRequestNumber++;
                    (it->second)->sqlRequestSize += (long long)(pkthdr->len);
                }

                windowsPCAP->rxPacketNumber++;                    // rxPacketNumber shall plus 1.
                windowsPCAP->rxSize += (long long)(pkthdr->len);  // rxSize shall plus the current one.

                // Obtaining the maximum size
                if (windowsPCAP->maxRxSize < (long long)(pkthdr->len)) {
                    windowsPCAP->maxRxSize = (long long)(pkthdr->len);
                }
                packetTypeDetermineSet = 0x1;

                // Recording the IP when first meeting the rx from the port; this IP will be reserved in the container for
                // the case when the later packets' port are not in the defined array; this Ip can determine the type of the packet
                std::unordered_map<uint32_t, char>::iterator itIp = ipMap.find(packetDestinationIp);
                if (itIp == ipMap.end()) {  // No one hitting
                    ipMap.emplace(packetDestinationIp, 0x0);
                }
            }
        }

        if (packetTypeDetermineSet == 0x0) {  // The port is not defined in the .json file

            // Obtaining no type; because there are no ports match in the array that users defined
            char packetTypeByIp = 0x0;
            // For readability, the author uses a variable, packetTypeByIp, to determine the type of the packet. That implies that
            // a packet only belongs a type to demonstrate the phenomenons of mutual exclusion.
            if (packetTypeByIp == 0x0) {  // TX consideration
                std::unordered_map<uint32_t, char>::iterator it = ipMap.find((int)packetSourceIp);
                if (it != ipMap.end()) {  // Hitting
                    windowsPCAP->txPacketNumber++;
                    windowsPCAP->txSize += (long long)(pkthdr->len);

                    // Obtaining the maximum size
                    if (windowsPCAP->maxTxSize < (long long)(pkthdr->len)) {
                        windowsPCAP->maxTxSize = (long long)(pkthdr->len);
                    }
                    packetTypeByIp = 0x1;
                }
            }

            if (packetTypeByIp == 0x0) {  // RX consideration
                std::unordered_map<uint32_t, char>::iterator it = ipMap.find((int)packetDestinationIp);
                if (it != ipMap.end()) {  // Hitting
                    windowsPCAP->rxPacketNumber++;
                    windowsPCAP->rxSize += (long long)(pkthdr->len);

                    // Obtaining the maximum size
                    if (windowsPCAP->maxRxSize < (long long)(pkthdr->len)) {
                        windowsPCAP->maxRxSize = (long long)(pkthdr->len);
                    }
                    packetTypeByIp = 0x1;
                }
            }
        }

        // Critical section end
        LeaveCriticalSection(&_CRITICAL_SECTION_);
    }

    // Verifying if the "pcap_loop" shall be stopped; "_IS_PCAP_WORKED_" is
    // a global variable and is controlled by the signal mechanism
    if (_IS_PCAP_WORKED_ == 0x0) {
        pcap_breakloop((pcap_t*)windowsPCAP->descriptor);
    }
}

/**
 * A handler when receiving the SIGINT signal
 *
 * @param signal [DWORD] The signal type
 * @return [BOOL WINAPI] The successful result; the TRUE shows okay; otherwise false
 */
BOOL WINAPI WindowsSizingMainCaller::signalInterruptedHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {  // When encountering the interrupted signal
        std::cerr << "\n"
                  << "Interrupted signal occurs, please wait.\n";
        // Using these two global variables to break the loops in different threads
        _IS_PCAP_WORKED_ = 0x0;
        _IS_ALARM_WORKED_ = 0x0;

        // Setting the event handler and signaling WaitForMultipleObjects to leaving the loop
        if (_EXITED_EVENT_ != nullptr) {
            SetEvent(_EXITED_EVENT_);
        }

        // Cancelling the timer
        if (_TIMER_ != nullptr) {
            CancelWaitableTimer(_TIMER_);
        }
    }
    return TRUE;
}

/**
 * A handler when receiving the SIGALRM signal; in the function, the main task is
 * writing the packet information to the file
 *
 * @param signalType [int] The signal type and the parameter is useless in this method
 */
void WindowsSizingMainCaller::signalAlarmHandler() {
    // File writing
    char output[1024] = {"\0"};
    if (*_FILE_POINTER_ == nullptr) {
        // Opening the file
        *_FILE_POINTER_ = fopen(_WRITING_FILE_LOCATION_, "a+");

        if (*_FILE_POINTER_ == nullptr) {
            std::cerr << "Error opening the file!\n";
            WindowsSizingMainCaller::signalInterruptedHandler(CTRL_C_EVENT);  // Going to the end of the thread

        } else {
            // Critical section, accessing the data area
            EnterCriticalSection(&_CRITICAL_SECTION_);

            // "UTC\tType\tPort\tNumber(amount)\tSize(byte)\tMaxSize\tSQL number per time interval(eps)\tSQL size per time interval(eps)\n";
            time_t timeEpoch = Commons::Time::getEpoch();

            // Looping all pcap object for printing the results
            for (std::vector<PCAP::PCAPPrototype*>::iterator it = _PCAP_POINTER_.begin();
                 it != _PCAP_POINTER_.end();
                 it++) {
                if (dynamic_cast<PCAP::WindowsPCAP*>(*it)) {
                    // Passing the object to the correct type
                    PCAP::WindowsPCAP* tmp = dynamic_cast<PCAP::WindowsPCAP*>(*it);
                    for (std::unordered_map<int, PCAP::PCAPPrototype::PCAPPortInformation*>::iterator it2 = (tmp->portRelatedInformation).begin();
                         it2 != (tmp->portRelatedInformation).end();
                         it2++) {
                        // TX part; in the section, the last two result will be to zero because the packets
                        // from the record set from the SQL server shall be ignored
                        int length = sprintf(output,
                                             "%lu\tTX\t%s\t%d\t%lu\t%llu\t%lu\t%lu\t%llu\t%llu\n",
                                             timeEpoch,
                                             (tmp->deviceInterface).c_str(),
                                             (it2)->first,  // port number
                                             tmp->txPacketNumber,
                                             tmp->txSize,
                                             tmp->maxTxSize,
                                             (long)0,
                                             (long long)0,
                                             (long long)0);
                        fwrite(output, sizeof(char), length, *_FILE_POINTER_);
                        ((it2)->second)->txPacketNumber = 0;
                        ((it2)->second)->txSize = 0;
                        ((it2)->second)->maxTxSize = 0;

                        // RX part
                        length = sprintf(output,
                                         "%lu\tRX\t%s\t%d\t%lu\t%llu\t%lu\t%lu\t%llu\t%llu\n",
                                         timeEpoch,
                                         (tmp->deviceInterface).c_str(),
                                         (it2)->first,  // port number
                                         tmp->rxPacketNumber,
                                         tmp->rxSize,
                                         tmp->maxRxSize,
                                         (it2->second)->sqlRequestNumber,
                                         (it2->second)->sqlRequestSize,
                                         (it2->second)->sqlRequestNumber / (long long)_WRITING_FILE_SECOND_);
                        fwrite(output, sizeof(char), length, *_FILE_POINTER_);
                        ((it2)->second)->rxPacketNumber = 0;
                        ((it2)->second)->rxSize = 0;
                        ((it2)->second)->maxRxSize = 0;
                        ((it2)->second)->sqlRequestNumber = 0;
                        ((it2)->second)->sqlRequestSize = 0;
                    }

                    // Clearing the rx and tx number, size and max size information when all ports' information is written
                    tmp->txPacketNumber = 0;
                    tmp->txSize = 0;
                    tmp->maxTxSize = 0;
                    tmp->rxPacketNumber = 0;
                    tmp->rxSize = 0;
                    tmp->maxRxSize = 0;
                }
            }
            // Critical section end
            LeaveCriticalSection(&_CRITICAL_SECTION_);

            // Closing the file
            if (*_FILE_POINTER_ != nullptr) {
                fclose(*_FILE_POINTER_);
                *_FILE_POINTER_ = nullptr;
            }
        }
    } else {  // Closing the descriptor and skipping the handling in the ith loop
        fclose(*_FILE_POINTER_);
        *_FILE_POINTER_ = nullptr;
    }
}

}  // namespace SizingMainCaller
#endif