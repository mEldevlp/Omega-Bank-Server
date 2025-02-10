#pragma warning(disable:4828)

#include "pch.h"

std::shared_ptr<spdlog::logger> Logger::logger;

int main(int argc, char** argv)
{
    Logger::init("Omega-Server");

    Logger::Info("Server is active!");

    boost::asio::io_context ioContext;
    unsigned short port = 8080;
    //Server server(ioContext, port);
    //server.startAccepting();

    ioContext.run();

    std::system("pause");

	return 0;
}
