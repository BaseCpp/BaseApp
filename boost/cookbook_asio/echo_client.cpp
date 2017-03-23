
#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <memory>

class client : public std::enable_shared_from_this<client> {
public:
        client(boost::asio::io_service & io_service, int i):
            socket(io_service),
            timer(io_service),
            strand(io_service),
            id(i){
            ;
        }

        void connect(const boost::asio::ip::tcp::endpoint & end) {
            auto self = this->shared_from_this();
            boost::asio::spawn(strand, [this, self, end](boost::asio::yield_context yield){
//                boost::system::error_code ec;
                socket.async_connect(end,  [this, self](boost::system::error_code ec){
                    self->go();
                });
            });
        }

        void go() {
            auto self = this->shared_from_this();
            boost::asio::spawn(strand, [this, self](boost::asio::yield_context yield) {
                try {
                    char data[128];
                    for(int i = 0; i < 100000000; i++){
                        timer.expires_from_now(std::chrono::seconds(10));
                        std::memcpy(data, &i, sizeof(i));
                        socket.async_send(boost::asio::buffer(data),yield);
                        socket.async_receive(boost::asio::buffer(data),yield);
                        //std::cout<<"id="<<id<<", seq="<< i << std::endl;
                    }
                } catch ( std::exception * e ){
                    socket.close();
                    timer.cancel();
                }
            });
            boost::asio::spawn(strand, [this, self](boost::asio::yield_context yield){
                if( socket.is_open() ) {
                    boost::system::error_code ec;
                    timer.async_wait(yield[ec]);
                    if( timer.expires_from_now() <= std::chrono::seconds(0))
                        socket.close();
                }
            });
        }

private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::steady_timer timer;
    boost::asio::io_service::strand strand;
    int id;
};

int main(int argc , char ** argv ) {
    try {
        long count = std::atoi(argv[1]);
        std::cout << count <<std::endl;
        boost::asio::io_service io_service;
        boost::asio::spawn(io_service, [&io_service, argv, count](boost::asio::yield_context yield){
            boost::asio::ip::tcp::resolver resolver(io_service);
            auto end = resolver.resolve({boost::asio::ip::tcp::v4(), argv[2], argv[3]});
            for(int i = 0; i < count; ++i ){
                std::make_shared<client>(io_service, i)->connect(*end);
            }
        });
        io_service.run();
    } catch ( std::exception & e) {
        std::cerr<<"Exception :" << e.what() << std::endl;
        return 0;
    }
}
