#include <iostream>
#include <string>
#include <csignal>
#include "modem.h"
#include "dictionary.h"

int main() {
    std::string csvPath = "./config.csv";
    std::string ttyPort = "/dev/pts/2";

    try {
        PatternDictionary dictionary(csvPath);
        Modem modem(ttyPort);

        while (true) {            
            std::string incoming_command = modem.ReadFromModem();
            std::string response = dictionary.FindCommand(incoming_command);
            if (response.empty()) {
                response = "ERROR"; 
            }
            modem.WriteToModem(response);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
