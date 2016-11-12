//
// Created by tom on 16-11-12.
//

#include "QtApplication.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <sstream>
#include "AppWidget.h"

using namespace Poco::Util;

class MyEvent : public QEvent {
public:
    MyEvent(async::task_run_handle &&t)
            : QEvent(QEvent::User), _handle(std::move(t)) {
        ;
    }
    async::task_run_handle _handle;
};

class QtSchedulerImpl : public QtScheduler, public QObject
{
public:
    QtSchedulerImpl(QObject * parent)
            :QObject(parent)
    {
        ;
    }

    void schedule(async::task_run_handle t)
    {
        qApp->postEvent(this, new MyEvent(std::move(t)), Qt::HighEventPriority);
    }

    bool event(QEvent * ev)
    {
        //SHOULD N
        //bool accept = QObject::event(ev);
        //if( accept )
        //    return true;
        //cast event to myEvent
        if( ev->type() == QEvent::User) {
            MyEvent *myEvent = dynamic_cast<MyEvent *>(ev);
            if (myEvent) {
                myEvent->_handle.run();
                myEvent->accept();
                return true;
            }
        }
        return false;
    }

};



static QtSchedulerImpl * globalptr = nullptr;
QtScheduler & qtui() {
    Q_ASSERT(globalptr && "SHOULD CALL qiui() between QtApplication::initialize~uninitialize");
    return *globalptr;
}



void QtApplication::init(int argc, char * argv[] )
{
    _argc = argc;
    _argv = argv;
    Application::init(argc, argv);
}

void QtApplication::initialize(Application& self)
{
    loadConfiguration();
    _app.reset(new QApplication(_argc, _argv));
    ::globalptr = new QtSchedulerImpl(_app.get());


    loadConfiguration(); // load default configuration files, if present
    Application::initialize(self);
}


void QtApplication::uninitialize()
{
    Application::uninitialize();
    ::globalptr = nullptr;
    _app.reset();
}

void QtApplication::reinitialize(Application &self) {
    Application::reinitialize(self);
}

void QtApplication::defineOptions(OptionSet& options)
{
    Application::defineOptions(options);

    options.addOption(
            Option("help", "h", "display help information on command line arguments")
                    .required(false)
                    .repeatable(false)
                    .callback(OptionCallback<QtApplication>(this, &QtApplication::handleHelp)));

    options.addOption(
            Option("define", "D", "define a configuration property")
                    .required(false)
                    .repeatable(true)
                    .argument("name=value")
                    .callback(OptionCallback<QtApplication>(this, &QtApplication::handleDefine)));

    options.addOption(
            Option("config-file", "f", "load configuration data from a file")
                    .required(false)
                    .repeatable(true)
                    .argument("file")
                    .callback(OptionCallback<QtApplication>(this, &QtApplication::handleConfig)));

    options.addOption(
            Option("bind", "b", "bind option value to test.property")
                    .required(false)
                    .repeatable(false)
                    .argument("value")
                    .binding("test.property"));
}

void QtApplication::handleHelp(const std::string& name, const std::string& value)
{
    _helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
}

void QtApplication::handleDefine(const std::string& name, const std::string& value)
{
    defineProperty(value);
}

void QtApplication::handleConfig(const std::string& name, const std::string& value)
{
    loadConfiguration(value);
}

void QtApplication::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A sample application that demonstrates some of the features of the Poco::Util::Application class.");
    helpFormatter.format(std::cout);
}

void QtApplication::defineProperty(const std::string& def)
{
    std::string name;
    std::string value;
    std::string::size_type pos = def.find('=');
    if (pos != std::string::npos)
    {
        name.assign(def, 0, pos);
        value.assign(def, pos + 1, def.length() - pos);
    }
    else name = def;
    config().setString(name, value);
}

void QtApplication::printProperties(const std::string& base)
{
    AbstractConfiguration::Keys keys;
    config().keys(base, keys);
    if (keys.empty())
    {
        if (config().hasProperty(base))
        {
            std::string msg;
            msg.append(base);
            msg.append(" = ");
            msg.append(config().getString(base));
            logger().information(msg);
        }
    }
    else
    {
        for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
        {
            std::string fullKey = base;
            if (!fullKey.empty()) fullKey += '.';
            fullKey.append(*it);
            printProperties(fullKey);
        }
    }
}

void QtApplication::setupMainUi() {
    _main = std::make_shared<QMainWindow>();
    _main->resize(800, 600);
    _main->setCentralWidget(new AppWidget(_main.get()));
    _main->show();
}

void QtApplication::shotdownMainUi() {
    _main.reset();
}

int QtApplication::main(const std::vector<std::string>& args)
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
    shotdownMainUi();
    return Application::EXIT_OK;
}
