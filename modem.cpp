#include "modem.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <stdexcept>

Modem::Modem(const std::string& portName) : Port(-1) {
    Port = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (Port < 0) {
        throw std::runtime_error("Error: Could not open tty device: " + portName);
    }

    struct termios tty;
    if (tcgetattr(Port, &tty) != 0) {
        close(Port);
        throw std::runtime_error("Error: Failed to fetch tty attributes.");
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag &= ~PARENB;        // Disable parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;         // Clear size bit-mask
    tty.c_cflag |= CS8;            // 8 data bits
    tty.c_cflag |= CLOCAL | CREAD; // Local line (no modem control lines), enable reading

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable software flow control
    tty.c_iflag &= ~(ICRNL | INLCR);                 // Prevent translating CR to NL and vice-versa
    tty.c_oflag &= ~OPOST;                           // Disable output post-processing

    tty.c_cc[VMIN] = 1;  // read() blocks until at least 1 byte arrives
    tty.c_cc[VTIME] = 0; // No inter-character timeout

    if (tcsetattr(Port, TCSANOW, &tty) != 0) {
        close(Port);
        throw std::runtime_error("Error: Failed to apply tty attributes.");
    }
}

Modem::~Modem() {
    if (Port >= 0) {
        close(Port);
    }
}

std::string Modem::ReadFromModem() {
    char ch;
    std::string command = "";

    while (true) {
        auto bytesRead = read(Port, &ch, 1);
        
        if (bytesRead < 0) {
            std::cerr << "Warning: Error encountered while reading tty line.\n";
            break;
        }
        
        if (bytesRead == 0) {
            continue;
        }

        if (ch == '\r' || ch == '\n') { // AT commands are terminated by a Carriage Return (\r) or Line Feed (\n)
            if (!command.empty()) {
                return command;
            }
            continue;
        }
        command += ch;
    }
    return command;
}

void Modem::WriteToModem(const std::string& response) {
    if (Port < 0) return;
    std::string fullResponse = response + "\r\n";
    auto bytesWritten = write(Port, fullResponse.c_str(), fullResponse.length());
    if (bytesWritten < 0) {
        std::cerr << "Warning: Failed to write response data to tty line.\n";
    }
}
