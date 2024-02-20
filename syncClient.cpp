#include<iostream>
#include<boost/asio.hpp>
#include<thread>
#include <atomic>

const int MAX_MSG_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

using namespace boost::asio::ip;

int main()
{
    try {
        std::atomic<bool> is_running(true);
        //创建上下文服务
        boost::asio::io_context   ioc;
        //构造endpoint
        tcp::endpoint  remote_ep(address::from_string("127.0.0.1"), 10086);
        tcp::socket  sock(ioc);
        boost::system::error_code   error = boost::asio::error::host_not_found; ;
        sock.connect(remote_ep, error);
        if (error) {
            std::cout << "connect failed, code is " << error.value() << " error msg is " << error.message();
            return 0;
        }
        std::thread send_thread([&sock,&is_running]() {
            while(is_running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                const char* request = "hello world!";
                size_t request_length = strlen(request);
                char send_data[MAX_MSG_LENGTH] = { 0 };
                short request_len_net = boost::asio::detail::socket_ops::host_to_network_short(request_length);
                memcpy(send_data, &request_len_net, HEAD_LENGTH);
                memcpy(send_data + HEAD_LENGTH, request, request_length);
                boost::asio::write(sock, boost::asio::buffer(send_data, request_length + 2));
            }
            });
        std::thread recv_thread([&sock,&is_running]() {
            while(is_running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                std::cout << "begin to receive..." << std::endl;
                char reply_head[HEAD_LENGTH];
                size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply_head, HEAD_LENGTH));
                short msglen = 0;
                memcpy(&msglen, reply_head, HEAD_LENGTH);
                msglen = boost::asio::detail::socket_ops::network_to_host_short(msglen);
                char msg[MAX_MSG_LENGTH] = { 0 };
                size_t  msg_length = boost::asio::read(sock, boost::asio::buffer(msg, msglen));
                std::cout << "Reply is: ";
                std::cout.write(msg, msglen) << std::endl;
                std::cout << "Reply len is " << msglen;
                std::cout << "\n";
            }
            });
        std::this_thread::sleep_for(std::chrono::seconds(300));
        is_running = false;
        send_thread.join();
        recv_thread.join();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}