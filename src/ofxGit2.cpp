#include "git2.h"

#include "ofxGit2.h"
#include "ofLog.h"
#include "ofUtils.h"
#include <string>

// some good explanations can be found here: https://ben.straub.cc/2013/06/03/refs-tags-and-branching/

static bool _silent = false;
static float _progress = 0;
static int _numberOfCharsPrinted = 0;

ofxGit::repository::repository(std::string path) : _path(path)
{
	git_libgit2_init();
}

bool ofxGit::repository::open(std::string path)
{
	if (!_silent)
	{
		ofLogNotice("ofxGit2") << "opening repository " << path;
	}
	_path = path;
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "could not open repository";
		}
		return false;
	}
	return true;
}

bool ofxGit::repository::isRepository()
{
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	return _error >= 0;
}
bool ofxGit::repository::clone(std::string url)
{
	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
	clone_opts.checkout_opts.progress_cb = checkoutProgressCallback;
	clone_opts.fetch_opts.callbacks.transfer_progress = transferProgressCallback;

	int error = git_clone(&_repo, url.c_str(), _path.c_str(), &clone_opts);
	if (error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "Could not clone repository:" << giterr_last()->message;
		}
	}
	return error >= 0;
}

bool ofxGit::repository::checkoutCommit(std::string hash)
{
	if (!_silent)
	{
		ofLogNotice("ofxGit2") << "checking out commit " << hash;
	}
	if (!isCommit(hash))
	{
		return false;
	}
	hash = getLongHash(hash);
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);

	const char *sha = hash.c_str();
	git_oid oid;
	_error = git_oid_fromstr(&oid, sha);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "Could not get oid from string:" << giterr_last()->message;
		}
		return false;
	}

	git_commit *commit = nullptr;
	git_commit_lookup(&commit, _repo, &oid);
	// std::cout << git_oid_tostr_s(&oid) << " " << git_commit_summary(commit) << std::endl;
	git_repository_set_head_detached(_repo, &oid);
	git_commit_free(commit);
	return true;
}
bool ofxGit::repository::checkoutTag(std::string name)
{

	if (!_silent)
	{
		ofLogNotice("ofxGit2") << "checking out tag " << name;
	}
	std::string tagRef = "refs/tags/" + name;

	std::string masterRefString = "refs/heads/master";
	if (!_silent)
	{
		ofLogVerbose("ofxGit2") << "checking out tag " << name;
	}

	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "Could not open repository: " << giterr_last()->message;
		}
		return false;
	}

	git_reference_iterator *iter = nullptr;
	git_reference *masterRef = nullptr;
	git_reference *new_ref = nullptr;
	git_revwalk *walker = nullptr;

	_error = git_reference_iterator_glob_new(&iter, _repo, "refs/tags/*");

	const char *tagName = NULL;
	while (!(_error = git_reference_next_name(&tagName, iter)))
	{
		git_oid oid;
		git_reference_name_to_id(&oid, _repo, tagName);
		char oidstr[41] = {0};
		git_oid_fmt(oidstr, &oid);

		auto tagNameString = std::string(tagName);
		auto oidString = std::string(oidstr);

		if (oidString.find(std::string(name), 0) == 0)
		{
			tagRef = tagName;
		}
		else
		{
			if (tagNameString.find(std::string(name), 0) > 0 && tagNameString.find(std::string(name), 0) < 64)
			{
				tagRef = tagNameString;
			}
		}
	}

	if (_error)
	{
	}

	_error = git_reference_lookup(&masterRef, _repo, "refs/heads/master");
	_error = git_reference_lookup(&new_ref, _repo, tagRef.c_str());
	_error = git_revwalk_new(&walker, _repo);
	_error = git_revwalk_push_ref(walker, tagRef.c_str());

	auto freePointers = [&]() {
		git_reference_iterator_free(iter);
		git_reference_free(masterRef);
		git_reference_free(new_ref);
		git_revwalk_free(walker);
	};

	git_oid oid;
	_error = git_revwalk_next(&oid, walker);

	_error = git_reference_set_target(&new_ref, masterRef, &oid, "message");

	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

	_error = git_repository_set_head_detached(_repo, &oid);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "an error occured while detaching head: " << giterr_last()->message;
		}
		freePointers();
		return false;
	}

	_error = git_checkout_head(_repo, &opts);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "an error occured while checking head: " << giterr_last()->message;
		}
		freePointers();
		return false;
	}

	freePointers();
	return _error >= 0;
}

bool ofxGit::repository::checkout(std::string checkout)
{
	if (!_silent)
	{
		ofLogVerbose("ofxGit2") << "checking out " << checkout;
	}
	if (isCommit(checkout))
	{
		return checkoutCommit(checkout);
	}
	else if (isTag(checkout))
	{
		return checkoutTag(checkout);
	}
	// else if (isBranch(checkout))
	// {
	// 	return checkoutBranch(checkout);
	// }
	return false;
}

std::string ofxGit::repository::getRemoteUrl(std::string name)
{
	if (!_silent)
	{
		ofLogVerbose("ofxGit2") << "getting remote url of " << name;
	}
	git_remote *remote = nullptr;
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	_error = git_remote_lookup(&remote, _repo, name.c_str());
	std::string url;

	if (_error >= 0)
	{
		url = std::string(git_remote_url(remote));
	}
	git_remote_free(remote);

	// const char *name = git_remote_name(remote);
	// const char *pushurl = git_remote_pushurl(remote);
	return url;
}
std::string ofxGit::repository::getCommitHash()
{
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "Could not open repository:" << giterr_last()->message;
		}
	}

	git_commit *commit = nullptr;
	git_oid oid;
	_error = git_reference_name_to_id(&oid, _repo, "HEAD");

	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "Could not get oid of head " << giterr_last()->message;
		}
		git_commit_free(commit);
		return "-1";
	}

	char shortsha[10] = {0};
	git_oid_tostr(shortsha, 10, &oid);
	git_commit_free(commit);
	return shortsha;
}

std::string ofxGit::repository::getLongHash(std::string hash)
{
	std::string value = "";
	git_revwalk *walker = nullptr;
	git_revwalk_new(&walker, _repo);
	git_revwalk_sorting(walker, GIT_SORT_NONE);
	git_revwalk_push_head(walker);
	git_oid oid;

	while (!git_revwalk_next(&oid, walker))
	{
		git_commit *commit = nullptr;
		git_commit_lookup(&commit, _repo, &oid);
		auto oidString = std::string(git_oid_tostr_s(&oid));
		if (oidString.find(std::string(hash), 0) == 0)
		{
			value = oidString;
		}
		git_commit_free(commit);
	}
	git_revwalk_free(walker);

	return value;
}

bool ofxGit::repository::isTag(std::string name)
{
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0)
	{
		// ofLogError("ofxGit2") << "Could not open repository: " << giterr_last()->message;
		return false;
	}
	git_object *treeish = nullptr;
	_error = git_revparse_single(&treeish, _repo, name.c_str());
	git_object_free(treeish);
	if (_error < 0)
	{
		// ofLogError("ofxGit2") << "Could not parse revision: " << giterr_last()->message;
		return false;
	}
	return _error >= 0;
}
// bool ofxGit::repository::isBranch(std::string name){
// 	return _error >= 0;
// }
bool ofxGit::repository::isCommit(std::string hash)
{
	bool value = false;
	git_revwalk *walker = nullptr;
	git_revwalk_new(&walker, _repo);
	git_revwalk_sorting(walker, GIT_SORT_NONE);
	git_revwalk_push_head(walker);
	git_oid oid;

	while (!git_revwalk_next(&oid, walker))
	{
		git_commit *commit = nullptr;
		git_commit_lookup(&commit, _repo, &oid);
		auto oidString = std::string(git_oid_tostr_s(&oid));
		if (oidString.find(std::string(hash), 0) == 0)
		{
			value = true;
		}
		git_commit_free(commit);
	}
	git_revwalk_free(walker);

	return value;
}

int ofxGit::repository::transferProgressCallback(const git_transfer_progress *stats, void *payload)
{
	float v = (float)stats->received_objects / stats->total_objects;
	if (!_silent)
	{
		if (v < 1)
		{
			ofxGit::commandLineUtils::del(_numberOfCharsPrinted);
			int length = v * 10;
			std::string text = "transfer progress: [" + ofToString(v * 100, 0, 3, ' ') + "] " + std::string(length, '-');
			_numberOfCharsPrinted = ofxGit::commandLineUtils::print(text);
		}
		else
		{
			if (_numberOfCharsPrinted != 0)
			{
				std::cout << " done" << std::endl;
				_numberOfCharsPrinted = 0;
			}
		}
	}
	_progress = v;
	return 0;
}

void ofxGit::repository::checkoutProgressCallback(const char *path, size_t cur, size_t tot, void *payload)
{
	float v = (float)cur / tot;
	if (!_silent)
	{
		if (v < 1)
		{
			ofxGit::commandLineUtils::del(_numberOfCharsPrinted);
			int length = v * 10;
			std::string text = "checkout progress: [" + ofToString(v * 100, 0, 3, ' ') + "] " + std::string(length, '-');
			_numberOfCharsPrinted = ofxGit::commandLineUtils::print(text);
		}
		else
		{
			if (_numberOfCharsPrinted != 0)
			{
				std::cout << " done" << std::endl;
				_numberOfCharsPrinted = 0;
			}
		}
	}
}
void ofxGit::repository::setSilent(bool value)
{
	_silent = value;
}