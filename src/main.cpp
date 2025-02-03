#pragma warning(disable:4828)

#include <QtCore>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

/*
class Logger 
{
public:
    static std::shared_ptr<spdlog::logger> logger;

    static void init()
    {
        logger = std::make_shared<spdlog::logger>("Server");
    }

    static std::shared_ptr<spdlog::logger>& getLogger() 
    {
        return logger;
    }
};
*/
int main(int argc, char** argv)
{
    //Logger::init();
    
    //Logger::getLogger()->info("Server is active!");

    boost::asio::io_context ioContext;
    unsigned short port = 8080;
    //Server server(ioContext, port);
    //server.startAccepting();

    ioContext.run();

	return 0;
}
