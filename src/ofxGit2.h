#pragma once

#include <iostream>
#include <string>

struct git_transfer_progress;
struct git_repository;

namespace ofxGit
{
class commandLineUtils
{
public:
	static int print(std::string text)
	{
		std::cout << text << std::flush;
		return text.size();
	}
	static void del(int size)
	{
		std::cout << std::string(size, '\b');
	}
};
class repository
{
public:
	repository(std::string path);
	bool open(std::string path);

	bool isRepository();
	bool clone(std::string url);
	bool checkoutCommit(std::string hash);
	bool checkoutTag(std::string name);
	bool checkout(std::string checkout);
	std::string getCommitHash();
	std::string getRemoteUrl(std::string name = "origin");

	std::string getLongHash(std::string hash);

	bool isTag(std::string name);
	// bool isBranch(std::string name);
	bool isCommit(std::string hash);

	static int transferProgressCallback(const git_transfer_progress *stats, void *payload);
	static void checkoutProgressCallback(const char *path, size_t cur, size_t tot, void *payload);
	static void setSilent(bool value = true);

private:
	git_repository *_repo = nullptr;
	std::string _path;
	int _error;
};
}; // namespace ofxGit