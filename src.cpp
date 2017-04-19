#include "HTTPDownloader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <map>
#include <mutex>
using namespace std;
#define TEST_TIMEOUT 1L
#define MISSIONS 1
#define BATCH 20
class result {
public:
	result(double _time, double _speed, const string& _url) :time(_time), speed(_speed), url(_url) {};
	double time;
	double speed;
	string url;
};
result test_thread(const string& url) {
	auto t_start = chrono::high_resolution_clock::now();
	HTTPDownloader downloader;
	int ec = 0;
	if ((ec = downloader.download(url + "core/os/x86_64/core.files", "/dev/null", TEST_TIMEOUT)) != 0) {
		if (ec != 28) {
			return result(TEST_TIMEOUT*MISSIONS, 0, url);
		}
	}
	auto t_end = chrono::high_resolution_clock::now();
	chrono::duration<double> diff = t_end - t_start;
	return result(diff.count(), downloader.speed, url);
}

int main(int argc, char** argv) {
	HTTPDownloader downloader;
	if (downloader.download("http://mirrors.kernel.org/archlinux/core/os/x86_64/pacman-mirrorlist-20170316-1-any.pkg.tar.xz", "/tmp/mirror.tar.xz", 3)) {
		if (downloader.download("http://mirrors.tuna.tsinghua.edu.cn/archlinux/core/os/x86_64/pacman-mirrorlist-20170316-1-any.pkg.tar.xz", "/tmp/mirror.tar.xz", 3)) {
			cerr << "Download mirrorlist from failed, Abort." << endl;
			system("rm -f /tmp/mirror.tar.xz");
			return -1;
		}
	}
	system("tar xJf /tmp/mirror.tar.xz -C /tmp etc/pacman.d/mirrorlist");
	ifstream file("/tmp/etc/pacman.d/mirrorlist");
	ostringstream s_buf;
	copy(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), ostreambuf_iterator<char>(s_buf));
	file.close();
	system("rm -rf /tmp/etc; rm -f /tmp/mirror.tar.xz");
	string buf = s_buf.str();
	regex rgx("(^|\\n)#Server = (.+)\\$repo/os/\\$arch(?=\\n|$)");
	std::smatch match;
	vector<string> v_mirror;
	while (regex_search(buf, match, rgx)) {
		v_mirror.push_back(match[2].str());
		buf = match.suffix().str();
	}
	cout << "Testing " << v_mirror.size() << " mirrors... This will take about " << v_mirror.size() / BATCH*TEST_TIMEOUT << " seconds." << endl;
	vector<future<result>> v_thread;
	vector<result> v_result;
	int count = 0;
	for (auto& mirror : v_mirror) {
		count++;
		if (count % BATCH == 0) {
			this_thread::sleep_for(chrono::seconds(TEST_TIMEOUT*MISSIONS));
		}
		auto t_future = async(launch::async, test_thread, mirror);
		v_thread.push_back(std::move(t_future));
	}
	this_thread::sleep_for(chrono::seconds(TEST_TIMEOUT*MISSIONS));

	for (auto& thread : v_thread) {
		result t_result = thread.get();
		v_result.push_back(result(t_result.time, t_result.speed, t_result.url));
	}

	sort(v_result.begin(), v_result.end(), [](result a, result b) {return b.speed < a.speed; });
	for (auto& result : vector<result>(v_result.begin(), v_result.begin() + 10)) {
		cout << result.speed / 1048576.0 << "MB/s\t" << result.url << endl;
	}
}
