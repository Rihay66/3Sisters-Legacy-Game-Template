#!/bin/sh
  
# check if build folder exists
DIR="../build"

if [ ! -d "$DIR" ]; then
    # print out msg
    echo "BUILD SCRIPT: Build folder missing! Generating Configuration!" 
    
    # Configuration command
    /usr/bin/cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ --no-warn-unused-cli -S. -B./build -G Ninja
    
    # print out msg
    echo "BUILD SCRIPT: Build folder generated!"
fi    

# Build command
/usr/bin/cmake --build ./build --config Debug --target game -- 
# Grab exit code of command above
EXITCODE=$?
    
# run command if build is succesful
if [ "$EXITCODE" -eq 0 ]; then
    echo "BUILD SCRIPT: BUILD successful! Running built program!"
    cd build
    ./game
else
    echo "BUILD SCRIPT: Build failed!"
fi
