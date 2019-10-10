meta:
	ADDON_NAME = ofxGit2
	ADDON_DESCRIPTION = Addon for working with git repositories
	ADDON_AUTHOR = Thomas Geissl
	ADDON_TAGS = "git" "libgit2"
	ADDON_URL = https://github.com/thomasgeissl/ofxGit2

linux64:
	# ADDON_PKG_CONFIG_LIBRARIES = libgit2
	# ADDON_LIBS = libs/libgit2/lib/linux64/libgit2.a
	# ADDON_LIBS += libs/libssh2/lib/linux64/libssh2.a
	ADDON_LDFLAGS = -lgit2 -lssh2 -L/usr/lib/x86_64-linux-gnu/
	# ADDON_INCLUDES = /usr/include/libssh2.h
vs:
	ADDON_INCLUDES_EXCLUDE = libs/libgit2/include/git2/sys
	ADDON_LIBS += libs/libgit2/lib/vs/x64/git2.lib
	ADDON_DLLS_TO_COPY += libs/libgit2/lib/vs/x64/git2.dll