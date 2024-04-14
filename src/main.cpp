// #include "Logger.hpp"

// int main() {
//     Logger::setLogLevel(DEBUG);
//     Logger::setLogFilename("log.txt");
//     LOG_INFO("Hello, world!");
//     LOG_DEBUG("This is a debug message.");
//     LOG_ERROR("This is an error message.");
//     LOG_WARNING("This is a warning message.");
//     return 0;
// }

#include "Socket.hpp"
#include <iostream>
#include <cstring> // For memset

int main() {

    std::string port = "8080";  // Define the port to listen on
    SocketConfiguration config("localhost", port, 1024); // Create a new SocketConfiguration object
    Socket serverSocket(&config);

    if (!serverSocket.setupAddrInfo()) {
        std::cerr << "Failed to initialize the server socket." << std::endl;
        return 1;
    }

    if (!serverSocket.bindAndListen()) {
        std::cerr << "Failed to bind or listen on port " << port << "." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // Main loop to accept and handle data from clients
    while (true) {
        std::string clientIP;
        int clientFd = serverSocket.acceptIncoming(clientIP);

        if (clientFd == -1) {
            std::cerr << "Failed to accept a new connection." << std::endl;
            continue; // Continue to accept new connections
        }

        std::cout << "Accepted new connection from " << clientIP << std::endl;

        // Receive data from the client
        std::string buffer[1024];
        int bytesRead;
        bool success = serverSocket.receive(clientFd ,buffer, sizeof(buffer), bytesRead);
        std::cout << "Success: " << success << " Bytes Read: " << bytesRead << std::endl;
        if (success && bytesRead > 0) {
            std::cout << "Received data: " << buffer << std::endl;

            // Echo the data back to the client
            if (!serverSocket.sendtoClient(buffer, bytesRead)) {
                std::cerr << "Failed to send data back to client." << std::endl;
            }
        } else if (bytesRead == 0) {
            std::cout << "Client closed the connection." << std::endl;
            serverSocket.removeSocket(); // Close the client socket
            break; // Exit the loop
        } else {
            std::cerr << "Error receiving data from client." << std::endl;
        }
    }

    return 0;
}
