cmake_minimum_required(VERSION 3.26)
#project(rsender)
project(rsender LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(tvision REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

add_executable(rsender
        main.cpp
        util/Logger.cpp util/Logger.h
        util/Utils.cpp util/Utils.h
        script/ScriptGenerator.cpp script/ScriptGenerator.h
        script/TagsExtractor.cpp script/TagsExtractor.h
        script/ScriptSaver.cpp script/ScriptSaver.h
        script/ScriptRunner.cpp script/ScriptRunner.h
        ui/PrepareScriptDialog.cpp ui/PrepareScriptDialog.h
        ui/EnhancedLabel.cpp ui/EnhancedLabel.h
        ui/EnhancedEditor.cpp ui/EnhancedEditor.h
        ui/TemplateEditor.cpp ui/TemplateEditor.h
        ui/RunScriptDialog.cpp ui/RunScriptDialog.h
        ui/ReadOnlyEditor.cpp ui/ReadOnlyEditor.h
        ui/PasswordInputLine.cpp ui/PasswordInputLine.h
        ui/TProgressBar.cpp ui/TProgressBar.h
        Const.h
        script/ParamsValidator.cpp
        script/ParamsValidator.h
        script/JsonValidator.cpp
        script/JsonValidator.h
        helper-script/RabbitMqAdminScriptDownloader.cpp
        helper-script/RabbitMqAdminScriptDownloader.h
        script/RunWorkspace.cpp
        script/RunWorkspace.h
)

# --- Link libraries ---
target_link_libraries(rsender
        PRIVATE
        tvision::tvision
        nlohmann_json::nlohmann_json
        Threads::Threads    # pthread
        util                # for forkpty (libutil)
        CURL::libcurl
)

# --- GCC 8 filesystem quirk ---
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(rsender PRIVATE stdc++fs)
endif()

# --- Static libstdc++/libgcc to avoid GLIBCXX mismatches ---
target_link_options(rsender PRIVATE -static-libstdc++ -static-libgcc)

# --- RPATH if you bundle libs next to the binary ---
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

install(TARGETS rsender RUNTIME DESTINATION bin)