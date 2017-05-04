#!/bin/bash

echo Listing dependencies

echo
echo *.dylib
find . -name *.dylib -exec echo {} \; -exec /var/lib/osxcross/target/bin/x86_64h-apple-darwin15-otool -L {} \;

echo
echo *.so
find . -name *.so -exec echo {} \; -exec ldd {} \;

echo
echo *.dll
find . -name *.dll -exec echo {} \; -exec bash -c "x86_64-linux-gnu-objdump -p {} | grep 'DLL Name:'" \;
