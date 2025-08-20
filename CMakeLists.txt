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
        util/ParamsCounter.h
        script/ScriptGenerator.cpp script/ScriptGenerator.h
        script/TagsExtractor.cpp script/TagsExtractor.h
        script/ScriptSaver.cpp script/ScriptSaver.h
        script/ScriptRunner.cpp script/ScriptRunner.h
        script/RunWorkspace.cpp script/RunWorkspace.h
        validator/ParamsValidator.cpp validator/ParamsValidator.h
        validator/JsonValidator.cpp validator/JsonValidator.h
        ui/PrepareScriptDialog.cpp ui/PrepareScriptDialog.h
        ui/common/EnhancedLabel.cpp ui/common/EnhancedLabel.h
        ui/common/EnhancedEditor.cpp ui/common/EnhancedEditor.h
        ui/TemplateEditor.cpp ui/TemplateEditor.h
        ui/RunScriptDialog.cpp ui/RunScriptDialog.h
        ui/common/ReadOnlyEditor.cpp ui/common/ReadOnlyEditor.h
        ui/common/PasswordInputLine.cpp ui/common/PasswordInputLine.h
        ui/TProgressBar.cpp ui/TProgressBar.h
        ui/MessageExtractDialog.cpp ui/MessageExtractDialog.h
        ui/HelpDialog.cpp ui/HelpDialog.h
        ui/ParamsEditor.cpp ui/ParamsEditor.h
        helper-script/RabbitMqAdminScriptDownloader.cpp
        helper-script/RabbitMqAdminScriptDownloader.h
        extractor/MessageExtractor.cpp extractor/MessageExtractor.h
        rabbitmq-access-data/RabbitMQAccessData.cpp rabbitmq-access-data/RabbitMQAccessData.h
        Const.h
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