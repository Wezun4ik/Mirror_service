#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace boost::asio;
using ip::tcp;

//dig HTTP header for content length
size_t							get_content_length(std::string str) {
	//static for perfomance; not global for readability
	static const boost::regex	reg_exp("Content-Length: (\\d+)");
	boost::smatch				results;

	boost::regex_search(str, results, reg_exp);
	//add error handling later
	return (std::stoi(results[1]));
}

//reads message from client via socket
std::vector<uchar>		socket_read(tcp::socket& socket) {
	//read http header
	streambuf 			info_buf;
	std::stringstream	stream;
	read_until(socket, info_buf, "\n");
	stream << &info_buf;

	//read how long is image in bytes
	auto len_of_image = get_content_length(stream.str());

	//read image into vector
	streambuf 			image_buf;
	boost::asio::read(socket, image_buf, boost::asio::transfer_exactly(len_of_image));
	std::vector<uchar>	ret(boost::asio::buffer_cast<const uchar*>(image_buf.data()),
							boost::asio::buffer_cast<const uchar*>(image_buf.data()) + len_of_image);
	return (ret);
}

//sends message to client via socket
void	socket_send(tcp::socket& socket, std::vector<uchar>& message) {
	//write to socket; not to be confused with c-style write
	write(socket, boost::asio::buffer(message));
}

//image processing
std::vector<uchar>		mirror_image(std::vector<uchar>& image) {
	//transform image to IR
	cv::Mat				raw_data(1, image.size(), CV_8UC1, image.data());
	cv::Mat				decoded_image  =  cv::imdecode(raw_data , cv::IMREAD_COLOR);
	//error handling to add later

	//mirror IR-form image
	cv::Mat				mirror_matrix;
	flip(decoded_image, mirror_matrix, 1);

	//write image to buffer
	std::vector<uchar>	ret;
	cv::imencode(".jpeg", mirror_matrix, ret);
	return (ret);
}

int						main() {
	for (;;) {
		//setting up boost.asio
		io_service		mirror_io_service;
		tcp::acceptor	mirror_acceptor(mirror_io_service, tcp::endpoint(tcp::v4(), 5875));
		tcp::socket		mirror_socket(mirror_io_service);

		//accept incoming request
		mirror_acceptor.accept(mirror_socket);

		//read image from socket
		auto image = socket_read(mirror_socket);

		//return mirrored image ready to transfer
		auto mirrored_image = mirror_image(image);

		//send mmirrored image back
		socket_send(mirror_socket, mirrored_image);
	}
	return (0);
}
