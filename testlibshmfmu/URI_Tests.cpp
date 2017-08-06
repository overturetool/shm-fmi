#include "gtest/gtest.h"

#include <string>
#include <iostream>

#include "uri.h"
/*

 Linux/Mac paths:
file:/Users/grs/temp => /Users/grs/temp/
file:///Users/user%20name/temp/ => /Users/user name/temp/

Windows paths:
file:/c:/Users/user%20name/temp => c:\Users\user name\temp\
file:///c:/Users/user%20name/temp => c:\Users\user name\temp\
file://server/Users/user%20name/temp => \\server\Users\user name\temp\
file:/c:/server/Users/user%20name/temp => \\server\Users\user name\temp\

  */

#ifdef _WIN32

TEST(URI, win_abs) {
  EXPECT_STREQ("c:\\Users\\user name\\temp\\",
               URIToNativePath("file:/c:/Users/user%20name/temp"));
}

TEST(URI, win_absSlashSlash) {
  EXPECT_STREQ("c:\\Users\\user name\\temp\\",
               URIToNativePath("file:///c:/Users/user%20name/temp"));
}

#elif __APPLE__ || __linux__

TEST(URI, nix_abs) {
  EXPECT_STREQ("/Users/grs/temp/", URIToNativePath("file:/Users/grs/temp"));
}

TEST(URI, nix_absSlashSlash) {
  EXPECT_STREQ("/Users/user name/temp/",
               URIToNativePath("file:///Users/user%20name/temp/"));
}
#endif
