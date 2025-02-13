#include "pch.h"

std::shared_ptr<spdlog::logger> Logger::logger;

int main(int argc, char** argv)
{
    Logger::init("Omega-Server");

    Logger::Info("Server is active!");

    //boost::asio::io_context ioContext;
    //unsigned short port = PORT;
    //Server server(ioContext, port);
    //server.startAccepting();
    //ioContext.run();

    std::system("pause");

	return 0;
}
