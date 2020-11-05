#include "spdlog/spdlog.h"

#include "socketServer.h"

int main(int argc, char const *argv[])
{
    SocketServer socketServer(8080);
    socketServer.open();

    return EXIT_SUCCESS;
}
