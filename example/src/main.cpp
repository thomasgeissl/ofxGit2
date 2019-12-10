#include "ofxGit2.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofUtils.h"

int main()
{
    ofDirectory dataPath = ofDirectory(ofToDataPath("."));
    if (!dataPath.exists())
    {
        dataPath.create();
    }
    ofxGit::repository repo(ofToDataPath("ofxMidi"));
    ofLogNotice("main") << "cloning ofxMidi: https://github.com/danomatika/ofxMidi.git";
    repo.clone("https://github.com/danomatika/ofxMidi.git");
    ofLogNotice("main") << "remote origin url " << repo.getRemoteUrl("origin");
    ofLogNotice("main") << "f1d92fd8e65f97383fcdc6612dd6b92e3a37e7e0 is commit " << repo.isCommit("f1d92fd8e65f97383fcdc6612dd6b92e3a37e7e0");
    ofLogNotice("main") << "-1 is commit " << repo.isCommit("-1");
    ofLogNotice("main") << "1.1.1 is commit " << repo.isCommit("1.1.1");
    ofLogNotice("main") << "2f0e6343 (tag 1.1.1) is commit " << repo.isTag("2f0e6343");
    ofLogNotice("main") << "1.1.1 is tag " << repo.isTag("1.1.1");
    ofLogNotice("main") << "-1 is tag " << repo.isTag("-1");
    ofLogNotice("main") << "successfully checked out commit 2f0e6343c817a4f5a33b60339c82b5d10be8af01: " << repo.checkoutCommit("2f0e6343c817a4f5a33b60339c82b5d10be8af01");
    ofLogNotice("main") << "current hash: " << repo.getCommitHash();
    repo.checkoutCommit("511173e7a43b0162cc80c8f630a51b6f0fe66925");
    ofLogNotice("main") << "successfully checked out tag 1.1.1: " << repo.checkoutTag("1.1.1");
    ofLogNotice("main") << "current hash: " << repo.getCommitHash();
}