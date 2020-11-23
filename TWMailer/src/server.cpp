#include <string>

#include "libraries/spdlog/spdlog.h"
#include "libraries/CLI11/CLI11.hpp"

#include "socketServer.hpp"

int main(int argc, char const *argv[]) {
    CLI::App app{"TWMailer Server"};

    std::string storagePath{"./storage"};
    app.add_option("-s,--storageFolder", storagePath, "Path to mailspool folder")->required();

    int port = {8080};
    app.add_option("-p,--port", port, "Port, which should be used")->required()->check(CLI::PositiveNumber);

    bool debug{false};
    app.add_flag("--debug", debug, "debug messages");

    bool disableLdap{false};
    app.add_flag("--disableLdap", disableLdap, "disables ldap");

    std::string key;
    app.add_option("-k,--key", key, "Key for encryption. Must be 16 characters long!");

    CLI11_PARSE(app, argc, argv);

    if (debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    if (!key.empty()) {
        if (key.length() != 16) {
            spdlog::error("Key for encryption must be exactly 16 characters long!");
        }
        MailRepository::instance().setKey(key);
    }

    std::filesystem::path storageFolderPath{storagePath};
    MailRepository::instance().setStorageFolder(storageFolderPath);
    BanRepository::instance().setStorageFolder(storageFolderPath);

    SocketServer socketServer(port);
    spdlog::info("Server started. Listening for connectins.");
    socketServer.open(disableLdap);

    return EXIT_SUCCESS;
}
