//
// Created by tom on 16-11-12.
//

#include "QtApplication.h"
#include <stringstream>

namespace Poco {
    namespace Util {
        void QtApplication::initialize(Application& self)
        {
            _app = std::make_shared<QApplication>();
            loadConfiguration(); // load default configuration files, if present
            Application::initialize(self);
        }

        void QtApplication::uninitialize()
        {
            Application::uninitialize();
            _app.reset();
        }

        void QtApplication::reinitialize(Application &self) {
            Application::reinitialize(self);
        }

        void setupMainUi() {
            _main = std::make_shared<QMainWindow>();
            _main->show();
        }

        int QtApplication::main(const ArgVec& args)
        {
            if (!_helpRequested)
            {
                logger().information("Command line:");
                std::ostringstream ostr;
                for (ArgVec::const_iterator it = argv().begin(); it != argv().end(); ++it)
                {
                    ostr << *it << ' ';
                }
                logger().information(ostr.str());
                logger().information("Arguments to main():");
                for (ArgVec::const_iterator it = args.begin(); it != args.end(); ++it)
                {
                    logger().information(*it);
                }
                logger().information("Application properties:");
            }
            setupMainUi();
            _app->exec();
            return Application::EXIT_OK;
        }
    }
}