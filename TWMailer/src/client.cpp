#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

#include "response.hpp"

#include "libraries/CLI11/CLI11.hpp"

#include "utils/socketUtils.hpp"
#include "utils/inputUtils.hpp"

#define BUFFER_SIZE 1024

void sendMessage(int socketFileDescriptor);

void logIn(int socketFileDescriptor);

using namespace std;

int connect(const char *address, int port) {
    auto socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0) {
        spdlog::error("Could not create socket!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socketAddress{};

    socketAddress.sin_family = AF_INET;
    inet_aton(address, &(socketAddress.sin_addr));
    socketAddress.sin_port = htons(port);

    if (connect(socketFileDescriptor, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) != 0) {
        spdlog::error("Unable to connect to server {}!", inet_ntoa(socketAddress.sin_addr));
        exit(EXIT_FAILURE);
    }
    return socketFileDescriptor;
}

void sendRequest(int socketFileDescriptor, const std::string &text) {
    if (!socketUtils::writeAll(socketFileDescriptor, text)) {
        cerr << "Server closed the connection!" << endl;
        exit(EXIT_FAILURE);
    }
}

void logIn(int socketFileDescriptor) {
    bool loggingIn = true;

    while (loggingIn) {
        cout << "Username: ";
        string username;
        getline(cin, username);
        cout << "Password: " << flush;
        string password = inputUtils::getPass();

        sendRequest(socketFileDescriptor,
                    string("LOGIN\n").append(username).append("\n").append(password).append("\n"));
        if (socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE) == RESPONSE_OK) {
            cout << "Logged in successfully" << endl;
            loggingIn = false;
        } else {
            cout << "Invalid credentials!" << endl;
        }
    }
}

void sendMessage(int socketFileDescriptor) {
    cout << "Receivers (separated by spaces): ";
    string receivers;
    getline(cin, receivers);
    cout << "Subject: ";
    string subject;
    getline(cin, subject);
    cout << "Message (Line with just a dot to declare the end): " << endl;
    string message;
    string line;
    while (line != ".") {
        getline(cin, line);
        message.append(line).append("\n");
    }
    sendRequest(socketFileDescriptor,
                string("SEND\n").append(receivers).append("\n").append(subject).append("\n").append(
                        message));
    string result = socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    if (result == RESPONSE_OK) {
        cout << "Message sent successfully." << endl;
    } else {
        cout << "There was an error sending the message." << endl;
    }
}

int main(int argc, char const *argv[]) {
    CLI::App app{"TWMailer Client"};

    std::string address{"127.0.0.1"};
    app.add_option("-a,--address", address, "IPv4 Address of the server")->required()->check(CLI::ValidIPV4);

    int port = {8080};
    app.add_option("-p,--port", port, "Port of the server")->required()->check(CLI::PositiveNumber);

    bool debug{false};
    app.add_flag("--debug", debug, "debug messages");

    CLI11_PARSE(app, argc, argv);

    if (debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    auto socketFileDescriptor = connect(address.c_str(), port);

    logIn(socketFileDescriptor);

    while (true) {
        cout
                << "Please select you command:\n1 - Send new message\n2 - List messages\n3 - Read message\n4 - Delete message\n5 - Quit"
                << endl;
        int selection;
        if (!(cin >> selection)) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << "The input needs to be a number!" << endl;
            continue;
        }
        cin.ignore(INT_MAX, '\n');
        switch (selection) {
            case 1:
                sendMessage(socketFileDescriptor);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                close(socketFileDescriptor);
                return EXIT_SUCCESS;
            default:
                cout << "The number needs to be between 1 and 5" << endl;
        }
    }

    sendRequest(socketFileDescriptor,
                "SEND\nif19b002\ntestSubject\nThis is a great message\nwith some newlines\nand an end.\n.\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "LIST\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "QUIT\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);

    sendRequest(socketFileDescriptor, "LOGIN\nif19b002\npasswort\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "LIST\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    // sendRequest(socketFileDescriptor, "DEL\n5665d682-75bb-4236-a7df-9f8cca24791e\n");
    // socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    // sendRequest(socketFileDescriptor, "READ\nc9150128-830c-49aa-ae0f-368ce4af0f91\n");
    // socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);

    return EXIT_SUCCESS;
}