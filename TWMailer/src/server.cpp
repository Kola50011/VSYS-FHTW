#include <string>
#include <experimental/filesystem>

#include "libraries/spdlog/spdlog.h"

#include "socketServer.h"
#include "persistence/mailRepository.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        // TODO Better message
        spdlog::error("Invalid amount of arguments specified! Usage:");
        return EXIT_FAILURE;
    }

    std::string storageFolder = argv[1];
    std::filesystem::path storageFolderPath{storageFolder};
    MailRepository::instance().setStorageFolder(storageFolderPath);

    SocketServer socketServer(8080);
    socketServer.open();

    return EXIT_SUCCESS;
}
