#ifndef MODEM_H
#define MODEM_H

#include <string>

class Modem {
private:
    int Port;

public:
    explicit Modem(const std::string& portName);
    ~Modem();

    std::string ReadFromModem();
    void WriteToModem(const std::string& response);
};

#endif // MODEM_H
