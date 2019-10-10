#include "git2.h"

#include "ofxGit2.h"
#include "ofLog.h"

ofxGit::repository::repository(std::string path) : _path(path) {
	git_libgit2_init();
}
bool ofxGit::repository::isRepository() {
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	return _error >= 0;
}
bool ofxGit::repository::clone(std::string url) {
	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
	clone_opts.checkout_opts.progress_cb = checkoutProgressCallback;
	clone_opts.fetch_opts.callbacks.transfer_progress = transferProgressCallback;


	int error = git_clone(&_repo, url.c_str(), _path.c_str(), &clone_opts);
	if (error < 0) {
		ofLogError("ofxGit2") << "Could not clone repository:" << giterr_last()->message;
	}
	return error >= 0;
}

bool ofxGit::repository::checkoutCommit(std::string hash) {
	if(!isCommit(hash)){
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
	if (_error < 0) {
		ofLogError("ofxGit2") << "Could not get oid from string:" << giterr_last()->message;
		return false;
	}

	git_commit * commit = nullptr;
	git_commit_lookup(&commit, _repo, &oid);
	std::cout << git_oid_tostr_s(&oid) << " " << git_commit_summary(commit) << std::endl; 
	git_repository_set_head_detached(_repo, &oid);
	git_commit_free(commit);
	return true;
}
bool ofxGit::repository::checkoutTag(std::string name) {
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0) {
		ofLogError("ofxGit2") << "Could not open repository: " << giterr_last()->message;
		return false;
	}
	// git_object *treeish = nullptr;
	// _error = git_revparse_single(&treeish, _repo, name.c_str());
	// if (_error < 0) {
	// 	ofLogError("ofxGit2") << "Could not parse revision: " << giterr_last()->message;
	// 	return false;
	// }
	// git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	// opts.progress_cb = checkoutProgressCallback;
	// _error = git_checkout_tree(_repo, treeish, &opts);
	// if (_error < 0) {
	// 	ofLogError("ofxGit2") << "Could not checkout tree: " << giterr_last()->message;
	// 	return false;
	// }
	// TODO
	// git_repository_set_head_detached(_repo, treeish->oid);

	git_reference *ref;
	std::string tagRef = "refs/tags/";
	tagRef += name;
    git_reference_lookup(&ref, _repo, "refs/heads/master");

    git_reference *new_ref;
    git_reference_lookup(&new_ref, _repo, tagRef.c_str());

    git_revwalk *walker;
    git_revwalk_new(&walker, _repo);
    git_revwalk_push_ref(walker, tagRef.c_str());


    git_oid oid;
    git_revwalk_next(&oid, walker);

    git_reference_set_target(&new_ref, ref, &oid, "message");

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

    _error = git_repository_set_head_detached(_repo,&oid);
	//  cerr<<"problem occured while detaching head"<< endl;

    _error = git_checkout_head(_repo, &opts);
	//  cerr << "problem checkout head" << endl;

    git_revwalk_free(walker);
	return _error >= 0;

}
bool ofxGit::repository::checkout(std::string checkout) {
	if(isCommit(checkout)){
		return checkoutCommit(checkout);
	}else if(isTag(checkout)){
		return checkoutTag(checkout);
	}
	return false;
}

std::string ofxGit::repository::getRemoteUrl(std::string name){
	ofLogVerbose("ofxGit2") << "getting remote url of " << name;
	git_remote *remote = nullptr;
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	_error = git_remote_lookup(&remote, _repo, name.c_str());

	// const char *name = git_remote_name(remote);
	const char *url  = git_remote_url(remote);
	// const char *pushurl = git_remote_pushurl(remote);
	// TODO: free remote
	return url;
}
std::string ofxGit::repository::getCommitHash() {
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0) {
		ofLogError("ofxGit2") << "Could not open repository:" << giterr_last()->message;
	}

	git_commit *commit = nullptr;
  	git_oid oid;
	_error = git_reference_name_to_id( &oid, _repo, "HEAD" );

	if (_error < 0) {
		ofLogError("ofxGit2") << "Could not get oid of head "<< giterr_last()->message;
		return "-1";
	}

	char shortsha[10] = {0};
	git_oid_tostr(shortsha, 10, &oid);
	return shortsha;
}
bool ofxGit::repository::isTag(std::string name){
	_error = git_repository_open_ext(&_repo, _path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
	if (_error < 0) {
		// ofLogError("ofxGit2") << "Could not open repository: " << giterr_last()->message;
		return false;
	}
	git_object *treeish = nullptr;
	_error = git_revparse_single(&treeish, _repo, name.c_str());
	if (_error < 0) {
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
bool ofxGit::repository::isCommit(std::string hash){
	const char *sha = hash.c_str();
	git_oid oid;
	_error = git_oid_fromstr(&oid, sha);
	if (_error < 0) {
		// ofLogError("ofxGit2") << "Could not get oid from string:" << giterr_last()->message;
		return false;
	}
	git_commit *commit;
	_error = git_commit_lookup(&commit, _repo, &oid);
	if (_error < 0) {
		// ofLogError("ofxGit2") << "Could not lookup commit:" << giterr_last()->message;
		return false;
	}
	return _error >= 0;
}

int ofxGit::repository::transferProgressCallback(const git_transfer_progress *stats, void *payload){
	float v = (float)stats->received_objects / stats->total_objects;
	ofLogNotice("ofxGit2", "transfer progress: %f %%", v*100);
	return 0;
}

void ofxGit::repository::checkoutProgressCallback(const char *path, size_t cur, size_t tot, void *payload){
	float v = (float)cur / tot;
	ofLogNotice("ofxGit2", "checkout progress: %f %%", v*100);
}