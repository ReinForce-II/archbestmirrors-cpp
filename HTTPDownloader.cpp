#include "HTTPDownloader.h"
#include <fstream>
#include <iostream>
#include <functional>
using namespace std;
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	reinterpret_cast<ofstream*>(stream)->write(reinterpret_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
}
HTTPDownloader::HTTPDownloader() {
	curl = curl_easy_init();
}
HTTPDownloader::~HTTPDownloader() {
	curl_easy_cleanup(curl);
}

int HTTPDownloader::download(const std::string& url, const std::string& path, long timeout) {
	this->timeout = timeout;
	starttime = chrono::high_resolution_clock::now();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
	std::ofstream out(path, std::ios::out | std::ios::trunc | std::ios::binary);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
	CURLcode res = curl_easy_perform(curl);

	curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed);
	out.close();
	return res;
}
