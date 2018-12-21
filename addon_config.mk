meta:
	ADDON_NAME = ofxGit2
	ADDON_DESCRIPTION = Addon for working with git repositories
	ADDON_AUTHOR = Thomas Geissl
	ADDON_TAGS = "git" "libgit2"
	ADDON_URL = https://github.com/thomasgeissl/ofxGit2

linux64:
	# ADDON_PKG_CONFIG_LIBRARIES = libgit2
	ADDON_LIBS = /usr/lib/x86_64-linux-gnu/libgit2.a
	ADDON_LIBS += /usr/lib/x86_64-linux-gnu/libssh2.a
	# ADDON_INCLUDES = /usr/include/libssh2.h