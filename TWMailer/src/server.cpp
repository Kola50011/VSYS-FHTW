#include <string>
#include <experimental/filesystem>

#include "libraries/spdlog/spdlog.h"
#include "libraries/CLI11/CLI11.hpp"

#include "socketServer.h"
#include "persistence/mailRepository.h"

int main(int argc, char const *argv[])
{
    CLI::App app{"TWMailer Server"};

    std::string storagePath{"./storage"};
    app.add_option("-s,--storageFolder", storagePath, "Path to mailspool folder")->required();

    int port = {8080};
    app.add_option("-p,--port", port, "Port, which should be used")->required()->check(CLI::PositiveNumber);

    bool debug{false};
    app.add_flag("--debug", debug, "debug messages");

    CLI11_PARSE(app, argc, argv);

    if (debug)
    {
        spdlog::set_level(spdlog::level::debug);
    }

    std::filesystem::path storageFolderPath{storagePath};
    MailRepository::instance().setStorageFolder(storageFolderPath);

    SocketServer socketServer(port);
    spdlog::info("Server started. Listening for connectins.");
    socketServer.open();

    return EXIT_SUCCESS;
}
