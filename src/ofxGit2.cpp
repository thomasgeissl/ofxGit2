#include "git2.h"

#include "ofxGit2.h"
#include "ofLog.h"
#include <string>

// some good explanations can be found here: https://ben.straub.cc/2013/06/03/refs-tags-and-branching/

static bool _silent = false;
static float _progress;

ofxGit::repository::repository(std::string path) : _path(path)
{
	git_libgit2_init();
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
	git_object *treeish = NULL;
	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	opts.progress_cb = checkoutProgressCallback;
	opts.checkout_strategy = GIT_CHECKOUT_SAFE;
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
	std::cout << git_oid_tostr_s(&oid) << " " << git_commit_summary(commit) << std::endl;
	git_repository_set_head_detached(_repo, &oid);
	git_commit_free(commit);
	ofLogNotice() << "done";
	return true;
}
bool ofxGit::repository::checkoutTag(std::string name)
{
	std::string tagRef = name;

	std::string masterRefString = "refs/heads/master";
	// TODO: check name vs oid
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

	// auto each_ref = [](git_reference *ref, void *payload) {
	// 	std::string *name = (std::string *)payload;
	// 	// ofLogNotice() << "name: " << name;
	// 	auto refName = git_reference_name(ref);
	// 	switch (git_reference_type(ref))
	// 	{
	// 	case GIT_REF_OID:
	// 	{
	// 		char oidstr[41] = {0};
	// 		git_oid_fmt(oidstr, git_reference_target(ref));
	// 		// ofLogNotice("ofxGit2") << refName << " is a direct reference to " << oidstr;
	// 		auto oidString = std::string(oidstr);
	// 		if (oidString.find(*name, 0) == 0)
	// 		{
	// 			ofLogNotice("checkout") << refName;
	// 		}
	// 		break;
	// 	}
	// 	case GIT_REF_SYMBOLIC:
	// 	{
	// 		// ofLogNotice("ofxGit2") << refName << " is a symbolic reference to " << git_reference_symbolic_target(ref);
	// 		break;
	// 	}
	// 	}
	// 	return 0;
	// };
	// git_reference_foreach(_repo, each_ref, &name);

	// auto each_name_cb = [](const char *name, void *payload) {
	// 	std::string *name = (std::string *)payload;
	// }
	// int error = git_reference_foreach_glob(_repo, "refs/tags/*", each_name_cb, &name);

	git_reference_iterator *iter = NULL;
	_error = git_reference_iterator_glob_new(&iter, _repo, "refs/tags/*");

	const char *tagName = NULL;
	while (!(_error = git_reference_next_name(&tagName, iter)))
	{
		git_oid oid;
		git_reference_name_to_id(&oid, _repo, tagName);
		char oidstr[41] = {0};
		git_oid_fmt(oidstr, &oid);
		auto oidString = std::string(oidstr);
		if (oidString.find(std::string(name), 0) == 0)
		{
			tagRef = tagName;
		}
		else
		{
			auto tagNameString = std::string(tagName);
			if (tagNameString.find(std::string(name), 0) > 0)
			{
				tagRef = tagName;
			}
		}
	}

	if (_error)
	{
	}

	git_reference *masterRef;
	_error = git_reference_lookup(&masterRef, _repo, "refs/heads/master");

	git_reference *new_ref;
	_error = git_reference_lookup(&new_ref, _repo, tagRef.c_str());

	git_revwalk *walker;
	_error = git_revwalk_new(&walker, _repo);
	_error = git_revwalk_push_ref(walker, tagRef.c_str());

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
		return false;
	}

	_error = git_checkout_head(_repo, &opts);
	if (_error < 0)
	{
		if (!_silent)
		{
			ofLogError("ofxGit2") << "an error occured while checking head: " << giterr_last()->message;
		}
		return false;
	}

	git_revwalk_free(walker);
	return _error >= 0;
}

bool ofxGit::repository::checkout(std::string checkout)
{
	if (isCommit(checkout))
	{
		return checkoutCommit(checkout);
	}
	else if (isTag(checkout))
	{
		return checkoutTag(checkout);
	}
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

	// const char *name = git_remote_name(remote);
	const char *url = git_remote_url(remote);
	// const char *pushurl = git_remote_pushurl(remote);
	// TODO: free remote
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
		return "-1";
	}

	char shortsha[10] = {0};
	git_oid_tostr(shortsha, 10, &oid);
	return shortsha;
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
	if (_error < 0)
	{
		// ofLogError("ofxGit2") << "Could not parse revision: " << giterr_last()->message;
		return false;
	}
	// git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	// _error = git_checkout_tree(_repo, treeish, &opts);
	return _error >= 0;
}
// bool ofxGit::repository::isBranch(std::string name){
// 	return _error >= 0;
// }
bool ofxGit::repository::isCommit(std::string hash)
{
	const char *sha = hash.c_str();
	git_oid oid;
	_error = git_oid_fromstr(&oid, sha);
	if (_error < 0)
	{
		// ofLogError("ofxGit2") << "Could not get oid from string:" << giterr_last()->message;
		return false;
	}
	git_commit *commit;
	_error = git_commit_lookup(&commit, _repo, &oid);
	if (_error < 0)
	{
		// ofLogError("ofxGit2") << "Could not lookup commit:" << giterr_last()->message;
		return false;
	}
	return _error >= 0;
}

int ofxGit::repository::transferProgressCallback(const git_transfer_progress *stats, void *payload)
{
	float v = (float)stats->received_objects / stats->total_objects;
	if (abs(_progress - v) > 0.1)
	{
		if (!_silent)
		{
			ofLogNotice("ofxGit2", "transfer progress: %f %%", v * 100);
		}
		_progress = v;
	}
	return 0;
}

void ofxGit::repository::checkoutProgressCallback(const char *path, size_t cur, size_t tot, void *payload)
{
	float v = (float)cur / tot;
	if (abs(_progress - v) > 0.1)
	{
		if (!_silent)
		{
			ofLogNotice("ofxGit2", "checkout progress: %f %%", v * 100);
		}
		_progress = v;
	}
}
void ofxGit::repository::setSilent(bool value)
{
	_silent = value;
}