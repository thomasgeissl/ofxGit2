#pragma once
#include "ofMain.h"
#include "git2.h"

namespace ofxGit {
	class repository {
		public:
		repository(std::string path);

		bool isRepository();
		bool clone(std::string url);
		bool checkout(std::string checkout);
		std::string getCommitHash();

		private:
		git_repository *_repo = NULL;
		std::string _path;
	};
};