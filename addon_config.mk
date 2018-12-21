meta:
	ADDON_NAME = ofxGit2
	ADDON_DESCRIPTION = Addon for working with git repositories
	ADDON_AUTHOR = Thomas Geissl
	ADDON_TAGS = "git" "libgit2"
	ADDON_URL = https://github.com/thomasgeissl/ofxGit2

linux64:
	# ADDON_PKG_CONFIG_LIBRARIES = libgit2
	ADDON_LIBS = libs/libgit2/lib/linux64/libgit2.a
	ADDON_LIBS += libs/libssh2/lib/linux64/libssh2.a
	# ADDON_INCLUDES = /usr/include/libssh2.h