#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/regex.hpp>

using namespace boost::asio;
using std::cout;
using std::endl;
using ip::tcp;

//for debug
std::ostream&		operator <<(std::ostream& stream, std::vector<char>& vec) {
	//not effective bc for debug purposes
	for (size_t i = 0; i < vec.size(); i++)
		stream << vec[i];
	return (stream);
}

//dig HTTP header for content length
size_t				get_content_length(std::string str) {
	static const boost::regex	reg_exp("Content-Length: (\\d+)");
	boost::smatch results;

	boost::regex_search(str, results, reg_exp);
	//add error handling later
	return (std::stoi(results[1]));
}

//reads message from client via socket
std::vector<char>	socket_read(tcp::socket& socket) {
	streambuf 					info_buf;
	std::stringstream			stream;

	//read http header
	read_until(socket, info_buf, "\n");
	stream << &info_buf;
	//read how long is image in bytes
	auto len_of_image = get_content_length(stream.str());
	//debug; delete later
	cout << len_of_image << endl;
	streambuf					image_buf;
	boost::asio::read(socket, image_buf, boost::asio::transfer_exactly(len_of_image));
	std::vector<char>	ret(boost::asio::buffer_cast<const char*>(image_buf.data()),
							boost::asio::buffer_cast<const char*>(image_buf.data()) + len_of_image);
	return (ret);
}

//sends message to client via socket
void				socket_send(tcp::socket& socket, std::vector<char>& message) {
	//write to socket; not to be confused with c-style write
	write(socket, boost::asio::buffer(message));
}

int					main() {
	//setting up boost.asio
	io_service			mirror_io_service;
	tcp::acceptor		mirror_acceptor(mirror_io_service, tcp::endpoint(tcp::v4(), 5875));
	tcp::socket			mirror_socket(mirror_io_service);

	//accept incoming request
	mirror_acceptor.accept(mirror_socket);

	//read image from socket
	auto image = socket_read(mirror_socket);

	//send mmirrored image back
	socket_send(mirror_socket, image);

	return (0);
}
