cmake_minimum_required(VERSION 3.31)
project(rsender)

set(CMAKE_CXX_STANDARD 20)

find_package(tvision REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(/home/aryz@pl.impaq.corp/Documents/Sources/tvision)
add_executable(rsender main.cpp
        util/Logger.cpp
        util/Logger.h
        script/ScriptGenerator.cpp
        script/ScriptGenerator.h
        util/Utils.cpp
        util/Utils.h
        script/TagsExtractor.cpp
        script/TagsExtractor.h
        Const.h
        ui/PrepareScriptDialog.cpp
        ui/PrepareScriptDialog.h
        ui/EnhancedLabel.cpp
        ui/EnhancedLabel.h
        ui/EnhancedEditor.cpp
        ui/EnhancedEditor.h
        ui/TemplateEditor.cpp
        ui/TemplateEditor.h
        script/ScriptSaver.cpp
        script/ScriptSaver.h
        ui/RunScriptDialog.cpp
        ui/RunScriptDialog.h
        ui/ReadOnlyEditor.cpp
        ui/ReadOnlyEditor.h
        ui/PasswordInputLine.cpp
        ui/PasswordInputLine.h
        script/ScriptRunner.cpp
        script/ScriptRunner.h
        ui/TProgressBar.cpp
        ui/TProgressBar.h)

target_link_libraries(rsender PRIVATE tvision::tvision)
target_link_libraries(rsender PRIVATE nlohmann_json::nlohmann_json)