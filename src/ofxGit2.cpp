#include "ofxGit2.h"

ofxGit::repository::repository(std::string path) : _path(path) {
	git_libgit2_init();
}
bool ofxGit::repository::isRepository() {
	int error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	return error >= 0;
}
bool ofxGit::repository::clone(std::string url) {
	int error = git_clone(&_repo, url.c_str(), _path.c_str(), NULL);
	if (error < 0) {
		ofLogError("ofxGit2") << "Could not clone repository:" << giterr_last()->message;
	}
	return error >= 0;
}

bool ofxGit::repository::checkout(std::string checkout) {
	return false;
}

std::string ofxGit::repository::getCommitHash() {
	int error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (error < 0) {
		ofLogError("ofxGit2") << "Could not open repository:" << giterr_last()->message;
	}
	// ofLogNotice("ofxGit2") << "head" << _repo->Head.Tip.Id;
	// return _repo->Head.Tip.Id;
	return "";
}