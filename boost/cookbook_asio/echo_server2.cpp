
//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <memory>

const int port = 8081;

using namespace  boost::asio::ip;

class session : public std::enable_shared_from_this<session> {
public :
    session(tcp::socket && sock)
        :socket (std::move(sock)),
         timer(socket.get_io_service()),
         strand(socket.get_io_service()){
         //nothing
    }

    void go() {
        // keep to live untile end of go
        // todo:: ~session will accur in worker thread, try to move to another thread
        auto self = this->shared_from_this();
        boost::asio::spawn(strand, [this, self](boost::asio::yield_context yield){
            //keep to live within any clousure with this
            try {
                char data[128];
                for(;;){
                    timer.expires_from_now(std::chrono::seconds(10)); // reset timer
                    auto n = socket.async_read_some(boost::asio::buffer(data), yield);
                    //throw a exception when failed
                    boost::asio::async_write(socket, boost::asio::buffer(data,n),yield);
                }
            } catch (std::exception & e){ //catch a exception when socket closed
                socket.close();
                timer.cancel();
            }
        });

        boost::asio::spawn(strand, [this, self](boost::asio::yield_context yield){
            while( socket.is_open()) {
                boost::system::error_code ec;
                timer.async_wait(yield[ec]);
                if( timer.expires_from_now() <= std::chrono::seconds(0))
                    socket.close();
            }
        });
    }

private:
    tcp::socket socket;
    boost::asio::steady_timer timer;
    boost::asio::io_service::strand strand;
};


int main(int argc , char ** argv ) {
    try {
        boost::asio::io_service io_service;
        boost::asio::spawn(io_service, [&io_service](boost::asio::yield_context yield ){
            boost::asio::ip::tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port) );
            for(;;) {
                boost::system::error_code ec;
                boost::asio::ip::tcp::socket socket(io_service);
                acceptor.async_accept(socket, yield[ec]);
                if(!ec) {
//                    std::cout<< socket.get_option()
                    std::make_shared<session>(std::move(socket))->go();
                }
            }
        });
        io_service.run();
    } catch ( std::exception & e) {
        std::cerr<<"Exception :" << e.what() << std::endl;
        return 0;
    }
}
