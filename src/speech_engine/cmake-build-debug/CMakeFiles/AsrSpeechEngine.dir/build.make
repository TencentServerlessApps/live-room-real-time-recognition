# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/AsrSpeechEngine.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/AsrSpeechEngine.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/AsrSpeechEngine.dir/flags.make

CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o: CMakeFiles/AsrSpeechEngine.dir/flags.make
CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o: ../asr/speech_recognizer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o -c /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/speech_recognizer.cpp

CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/speech_recognizer.cpp > CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.i

CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/speech_recognizer.cpp -o CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.s

CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o: CMakeFiles/AsrSpeechEngine.dir/flags.make
CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o: ../asr/tcloud_public_request_build.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o -c /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/tcloud_public_request_build.cpp

CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/tcloud_public_request_build.cpp > CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.i

CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/asr/tcloud_public_request_build.cpp -o CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.s

CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o: CMakeFiles/AsrSpeechEngine.dir/flags.make
CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o: ../common/tcloud_util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o -c /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/common/tcloud_util.cpp

CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/common/tcloud_util.cpp > CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.i

CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/common/tcloud_util.cpp -o CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.s

CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o: CMakeFiles/AsrSpeechEngine.dir/flags.make
CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o: ../business/usages/TencentSDKWapperRecord.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o -c /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/usages/TencentSDKWapperRecord.cpp

CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/usages/TencentSDKWapperRecord.cpp > CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.i

CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/usages/TencentSDKWapperRecord.cpp -o CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.s

CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o: CMakeFiles/AsrSpeechEngine.dir/flags.make
CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o: ../business/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o -c /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/main.cpp

CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/main.cpp > CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.i

CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/business/main.cpp -o CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.s

# Object files for target AsrSpeechEngine
AsrSpeechEngine_OBJECTS = \
"CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o" \
"CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o" \
"CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o" \
"CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o" \
"CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o"

# External object files for target AsrSpeechEngine
AsrSpeechEngine_EXTERNAL_OBJECTS =

../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/asr/speech_recognizer.cpp.o
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/asr/tcloud_public_request_build.cpp.o
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/common/tcloud_util.cpp.o
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/business/usages/TencentSDKWapperRecord.cpp.o
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/business/main.cpp.o
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/build.make
../bin/AsrSpeechEngined: CMakeFiles/AsrSpeechEngine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable ../bin/AsrSpeechEngined"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/AsrSpeechEngine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/AsrSpeechEngine.dir/build: ../bin/AsrSpeechEngined
.PHONY : CMakeFiles/AsrSpeechEngine.dir/build

CMakeFiles/AsrSpeechEngine.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/AsrSpeechEngine.dir/cmake_clean.cmake
.PHONY : CMakeFiles/AsrSpeechEngine.dir/clean

CMakeFiles/AsrSpeechEngine.dir/depend:
	cd /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug /Users/abel/Documents/PROJECTS/SCF/py/Media/trtc_audio_recognition/speech_engine/cmake-build-debug/CMakeFiles/AsrSpeechEngine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/AsrSpeechEngine.dir/depend
