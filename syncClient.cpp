#include<iostream>
#include<boost/asio.hpp>
const unsigned int MAX_LENGTH = 1024;
using namespace boost::asio;
//测试git是否存储
int main() {
	try {
		// Step 1. Here we assume that the server is listening on port 3333
		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8080);	//这里会自动推断ip协议版本
		// Step 2. Creating and connecting the socket.
		io_context ioc;
		boost::system::error_code ec;
		ip::tcp::socket sock(ioc);
		sock.connect(ep,ec);
		if (ec) {
			std::cerr << "Error: " << ec.message() << std::endl;
			return 0;
		}
		// Step 3. At this point the connection is established. We can send data to the server.
		char request[MAX_LENGTH];
		std::cin.getline(request, MAX_LENGTH);
		size_t request_length = strlen(request);
		write(sock, buffer(request, request_length));
		// Step 4. This is the blocking call. The program will wait here until data is received.
		char reply[MAX_LENGTH];
		size_t reply_length = read(sock, buffer(reply, request_length));
		std::cout << "Reply is: ";
		std::cout.write(reply, reply_length);
		std::cout << "\n";
		// Step 5. Shutting down the connection and the socket.
		sock.shutdown(ip::tcp::socket::shutdown_both);
	}
	catch (boost::system::system_error& e) {
		std::cerr << "Error code: " << e.code() << std::endl;
		std::cerr << "Error message: " << e.what() << std::endl;
	}
	return 0;
}