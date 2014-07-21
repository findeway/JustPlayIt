#include "StdAfx.h"
#include "SyncHttpClient.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include "Util.h"

using boost::asio::ip::tcp;

CSyncHttpClient::CSyncHttpClient(void)
{
}

CSyncHttpClient::~CSyncHttpClient(void)
{
}

std::string CSyncHttpClient::GetHttpResponse(const wchar_t* url)
{
	std::string httpResponse;
    try
    {
        boost::asio::io_service io_service;
        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(W2Utf8(GetHost(url).c_str()), "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;  

		//Try each endpoint until we successfully establish a connection.  
		boost::asio::ip::tcp::socket socket(io_service);  
		boost::system::error_code error = boost::asio::error::host_not_found;  
		while (error && endpoint_iterator != end)  
		{  
			socket.close();  
			socket.connect(*endpoint_iterator++, error);  
		}  
		if (error)  
			throw boost::system::system_error(error); 

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << W2Utf8(url) << " HTTP/1.0\r\n";
        request_stream << "Host: " << W2Utf8(GetHost(url).c_str()) << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            //"Invalid response\n";
            return "";
        }
        if (status_code != 200)
        {
            //"Response returned with status code ";
            return "";
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r")
		{
            //std::cout << header << "\n";
		}

        // Read until EOF, writing data to output as we go.
		while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)){;}

		string temp;  
		while(!response_stream.eof()){  
			std::getline(response_stream, temp);  
			httpResponse.append(temp);  
		}  
        if (error != boost::asio::error::eof)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e)
    {
        return "";
    }
	return httpResponse;
}

std::wstring CSyncHttpClient::GetHost(const wchar_t* url)
{
	std::wstring strUrl = url;
	int startPos = strUrl.find(L"http://");
	if(startPos != std::wstring::npos)
	{
		startPos += wcslen(L"http://");
	}
	else
	{
		startPos = 0;
	}
	int endPos = strUrl.find(L"/",startPos + 1);
	std::wstring str = strUrl.substr(startPos,endPos-startPos);
	return strUrl.substr(startPos,endPos-startPos);
}

