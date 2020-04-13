#include "ofAppNoWindow.h"
#include "ofAppRunner.h"
#include "ofxUnitTests.h"
#include "ofxGit2.h"

class ofApp : public ofxUnitTestsApp
{
    void run()
    {
        ofxGit::repository repo(ofToDataPath("ofxMidi"));
        repo.clone("https://github.com/danomatika/ofxMidi.git");
        ofxTestEq("https://github.com/danomatika/ofxMidi.git", repo.getRemoteUrl("origin"), "correct remote url");
        ofxTest(repo.isCommit("511173e7a43b0162cc80c8f630a51b6f0fe66925"), "is valid commit hash");
        ofxTest(!repo.isCommit("-1"), "is not a valid commit hash");
        std::string validCommitHash = "511173e7a43b0162cc80c8f630a51b6f0fe66925";
        repo.checkout(validCommitHash);
        ofxTest(validCommitHash.rfind(repo.getCommitHash(), 0) == 0, "checkout commit by hash");
        std::string validTag = "1.1.1";
        ofxTest(repo.isTag(validTag), "1.1.1 is valid tag");
        ofxTest(!repo.isCommit(validTag), "1.1.1 is invalid commit");
        repo.checkout(validTag);
        std::string hashOfTag = "2f0e6343c817a4f5a33b60339c82b5d10be8af01";
        ofxTest(hashOfTag.rfind(repo.getCommitHash(), 0) == 0, "checkout commit by tag");
    }
};

int main()
{
    auto window = std::make_shared<ofAppNoWindow>();
    auto app = std::make_shared<ofApp>();
    ofRunApp(window, app);
    return ofRunMainLoop();
}
