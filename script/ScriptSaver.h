#ifndef SCRIPTSAVER_H
#define SCRIPTSAVER_H

#include <filesystem>

class ScriptSaver {
public:
    static bool saveScriptToFile(const std::string& path, const std::string& content);
};


#endif //SCRIPTSAVER_H
