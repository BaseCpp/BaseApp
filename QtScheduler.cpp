
#include "QtScheduler.h"


#include <Poco/SingletonHolder.h>
#include <QtGui>
#include <QtCore>
#include <mutex>


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
    QtSchedulerImpl()
    {
        Q_ASSERT(qApp);
        this->setParent(qApp);
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




QtScheduler & QtScheduler::instance() {
      static Poco::SingletonHolder<QtSchedulerImpl> sh;
      return *sh.get();
}


QtScheduler & qtui() {
    return QtScheduler::instance();
}