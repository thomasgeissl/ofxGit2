meta:
	ADDON_NAME = ofxGit2
	ADDON_DESCRIPTION = Addon for working with git repositories
	ADDON_AUTHOR = Thomas Geissl
	ADDON_TAGS = "git" "libgit2"
	ADDON_URL = https://github.com/thomasgeissl/ofxGit2

osx:
	ADDON_LIBS += libs/libgit2/lib/osx/libgit2.a
	ADDON_FRAMEWORKS += Security
	ADDON_LDFLAGS += -liconv

linux64:
    ADDON_PKG_CONFIG_LIBRARIES = libgit2
	ADDON_LIBS_EXCLUDE = libs/libgit2 libs/libssh2
	ADDON_INCLUDES_EXCLUDE = libs/libgit2/include libs/libssh2/include

vs:
	ADDON_INCLUDES_EXCLUDE = libs/libgit2/include/git2/sys
	ADDON_LIBS += libs/libgit2/lib/vs/x64/git2.lib
	ADDON_DLLS_TO_COPY += libs/libgit2/lib/vs/x64/git2.dll
