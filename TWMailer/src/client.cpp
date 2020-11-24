#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

#include "response.hpp"

#include "libraries/CLI11/CLI11.hpp"

#include "utils/socketUtils.hpp"
#include "utils/inputUtils.hpp"
#include <utils/stringUtils.hpp>

#define BUFFER_SIZE 1024

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

string receiveResponse(int socketFileDescriptor) {
    string response = socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    if (response.empty()) {
        cerr << "Server closed the connection!" << endl;
        exit(EXIT_FAILURE);
    }
    return response;
}

void login(int socketFileDescriptor) {
    bool loggingIn = true;

    while (loggingIn) {
        cout << "Username: ";
        string username;
        getline(cin, username);
        cout << "Password: " << flush;
        string password = inputUtils::getPass();

        socketUtils::writeAll(socketFileDescriptor,
                              string("LOGIN\n").append(username).append("\n").append(password).append("\n"));
        if (receiveResponse(socketFileDescriptor) == RESPONSE_OK) {
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
    socketUtils::writeAll(socketFileDescriptor,
                          string("SEND\n").append(receivers).append("\n").append(subject).append("\n").append(message));
    if (receiveResponse(socketFileDescriptor) == RESPONSE_OK) {
        cout << "Message sent successfully." << endl;
    } else {
        cout << "There was an error sending the message." << endl;
    }
}

void listMessages(int socketFileDescriptor) {
    socketUtils::writeAll(socketFileDescriptor, "LIST\n");
    string result = receiveResponse(socketFileDescriptor);
    if (result.starts_with(RESPONSE_OK)) {
        vector lines = stringUtils::split(result, "\n");
        cout << "Total count: " << lines.at(1) << "\nMessages: " << endl;
        for (int i = 2; i < lines.size(); i++) {
            cout << lines.at(i) << endl;
        }
    } else {
        cout << "There was an error listing the messages." << endl;
    }
}

void listSentMessages(int socketFileDescriptor) {
    socketUtils::writeAll(socketFileDescriptor, "LIST_SENT\n");
    string result = receiveResponse(socketFileDescriptor);
    if (result.starts_with(RESPONSE_OK)) {
        vector lines = stringUtils::split(result, "\n");
        cout << "Total count: " << lines.at(1) << "\nMessages: " << endl;
        for (int i = 2; i < lines.size(); i++) {
            cout << lines.at(i) << endl;
        }
    } else {
        cout << "There was an error listing the messages." << endl;
    }
}

void readMessage(int socketFileDescriptor) {
    cout << "MessageID: ";
    string messageId;
    getline(cin, messageId);
    socketUtils::writeAll(socketFileDescriptor, string("READ\n").append(messageId).append("\n"));
    string result = receiveResponse(socketFileDescriptor);
    if (result.starts_with(RESPONSE_OK)) {
        vector lines = stringUtils::split(result, "\n");
        cout << "Sender: " << lines.at(1) << "\nReceivers: " << lines.at(2) << "\nSubject: " << lines.at(3) << endl;
        for (int i = 4; i < lines.size(); i++) {
            cout << lines.at(i) << endl;
        }
    } else {
        cout << "There was an error retrieving the message." << endl;
    }
}

void deleteMessage(int socketFileDescriptor) {
    cout << "MessageID: ";
    string messageId;
    getline(cin, messageId);
    socketUtils::writeAll(socketFileDescriptor, string("DEL\n").append(messageId).append("\n"));
    if (receiveResponse(socketFileDescriptor) == RESPONSE_OK) {
        cout << "Message deleted successfully." << endl;
    } else {
        cout << "There was an error deleting the message." << endl;
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

    login(socketFileDescriptor);

    while (true) {
        cout
                << "Please select you command:\n1 - Send new message\n2 - List messages\n3 - List sent messages\n4 - Read message\n5 - Delete message\n6 - Quit"
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
                listMessages(socketFileDescriptor);
                break;
            case 3:
                listSentMessages(socketFileDescriptor);
                break;
            case 4:
                readMessage(socketFileDescriptor);
                break;
            case 5:
                deleteMessage(socketFileDescriptor);
                break;
            case 6:
                close(socketFileDescriptor);
                return EXIT_SUCCESS;
            default:
                cout << "The number needs to be between 1 and 5" << endl;
        }
    }
}