//
// Created by tom on 16-11-12.
//

#ifndef BASEAPP_QTSUBSYSTEM_H
#define BASEAPP_QTSUBSYSTEM_H

#include <Poco/Util/Application.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <memory>
namespace Poco
{
    namespace Util
    {
        class QtApplication : public Application
        {
        public:

            ///   - initialize() (the one-argument, protected variant)
            ///   - uninitialize()
            ///   - reinitialize()
            ///   - defineOptions()
            ///   - handleOption()
            ///   - main()

            QApplication * app() { return _app.get(); }

        protected:
            void initialize(Application& self) override;

            void uninitialize() override;

            void reinitialize(Application& self);

            int main(const ArgVec& args) override;

            virtual void setupMainUi() ;

        private:
            std::shared_ptr<QMainWindow> _main;
            std::shared_ptr<QApplication> _app;
        };
    }
}


#endif //BASEAPP_QTSUBSYSTEM_H
