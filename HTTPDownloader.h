#ifndef HTTPDOWNLOADER_HPP
#define HTTPDOWNLOADER_HPP

#include <string>
#include <chrono>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>

class HTTPDownloader {
public:
	HTTPDownloader();
	~HTTPDownloader();
	int download(const std::string& url, const std::string& path, long timeout);
	double speed;
private:
	void* curl;
	decltype(std::chrono::high_resolution_clock::now()) starttime;
	int timeout;
};

#endif  /* HTTPDOWNLOADER_HPP */
