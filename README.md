# BaseApp

Example Application build with:
* [CMake](https://github.com/Kitware/CMake)
* [hunter](https://github.com/ruslo/hunter/)
    hunter_add_package
* [Poco](https://github.com/pocoproject/poco)
    Application
* [Async++](https://github.com/Amanieu/asyncplusplus)
    shared_task, task, spawn
* [Qt](qt-project.org)
    QCoreApplication::postEvent()



[submodule "gate"]
  path = gate
  url = https://github.com/hunter-packages/gate.git

[submodule "wiki"]
  path = wiki
  url = https://github.com/BaseCpp/BaseApp.wiki.git

[submodule "magic_get"]
  path = thirdparty/boost/magic_get
  url = https://github.com/apolukhin/magic_get.git


[submodule "di"]
  path = thirdparty/boost/di
  url = https://github.com/boost-experimental/di.git

[submodule "application"]
  path = thirdparty/boost/application
  url = https://github.com/retf/Boost.Application.git
  
[submodule "stacktrace"]
	path = thirdparty/stacktrace
	url = https://github.com/apolukhin/stacktrace.git