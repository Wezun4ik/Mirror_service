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
	if (results.size() != 2)
		throw (std::invalid_argument("Header does not contain Content-Length attribute"));
	return (std::stoi(results[1]));
}

//copy data from asio::streambuf to vector
void					buf_to_vec(std::vector<uchar>& ret, streambuf& buf,
						size_t begin, size_t end) {
	std::vector<uchar>	vec(
	boost::asio::buffer_cast<const uchar*>(buf.data()) + begin,
	boost::asio::buffer_cast<const uchar*>(buf.data()) + end);
	ret.insert(ret.end(), vec.begin(), vec.end());
}

//reads message from client via socket
std::vector<uchar>		socket_read(tcp::socket& socket) {
	//read http header
	streambuf			info_buf;
	auto til_delim = read_until(socket, info_buf, "\r\n\r\n");
	std::string			header(boost::asio::buffer_cast<const char*>(info_buf.data()));

	//read how long is image in bytes
	auto len_of_image = get_content_length(header);
	if (len_of_image == 0)
		throw (std::invalid_argument("Nothing was sent"));

	std::vector<uchar>	ret;

	//read_until() sometimes reads beyond delimeter string
	//so copying trimtrail is required
	if (info_buf.size() - til_delim > 0)
		buf_to_vec(ret, info_buf, til_delim, info_buf.size());

	//read the rest of the image into vector
	if (len_of_image - ret.size() > 0) {
		streambuf		image_buf(len_of_image - ret.size());
		boost::asio::read(socket, image_buf, boost::asio::transfer_all());
		buf_to_vec(ret, image_buf, 0, len_of_image);
	}
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
	cv::Mat				decoded_image  = cv::imdecode(raw_data , cv::IMREAD_COLOR);
	if (decoded_image.empty())
		throw (std::invalid_argument("File is not correct .jpeg image"));

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
		try {
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
		} catch (std::invalid_argument& e) {
			std::cout << "Transmitted data is not valid: " << e.what() << std::endl;
		} catch (boost::exception& e) {
			std::cout << "Boost failed: " << dynamic_cast<std::exception const&>(e).what() << std::endl;
		} catch (cv::Exception& e) {
			std::cout << "CV failed: " << e.what() << std::endl;
		}
	}
	return (0);
}
