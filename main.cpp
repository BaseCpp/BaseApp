//
// Created by tom on 16-11-12.
//

#include "QtApplication.h"
int main(int argc, char** argv)
{
    Poco::AutoPtr<QtApplication> pApp = new QtApplication;
    try
    {
        pApp->init(argc, argv);
    }
    catch (Poco::Exception& exc)
    {
        pApp->logger().log(exc);
        return Poco::Util::Application::EXIT_CONFIG;
    }
    return pApp->run();
}