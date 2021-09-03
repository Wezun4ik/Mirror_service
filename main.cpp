#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;
using std::cout;
using std::endl;

//for debug
std::ostream& operator <<(std::ostream& stream, std::vector<char>& vec) {
	//not effective bc for debug purposes
	for (size_t i = 0; i < vec.size(); i++)
		stream << vec[i];
	return (stream);
}

//temporary empty
std::vector<char>	read(ip::tcp::socket & socket) {
	return (std::vector<char>{0});
}

//temporary emptry
void send(ip::tcp::socket & socket, std::vector<char> message) {
}

int main () {
	//setting up boost.asio
	io_service			mirror_io_service;
	ip::tcp::acceptor	mirror_acceptor(mirror_io_service, ip::tcp::endpoint(ip::tcp::v4(), 5875));
	ip::tcp::socket		mirror_socket(mirror_io_service);

	//accept incoming request
	mirror_acceptor.accept(mirror_socket);
	//read image from socket
	auto image = read(mirror_socket);
	//debug; delete later
	cout << image << endl;
	//send mmirrored image back
	send(mirror_socket, image);
	//debug; delete later
	cout << "it Works!" << endl;
	return (0);
}
