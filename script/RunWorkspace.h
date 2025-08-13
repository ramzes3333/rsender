#ifndef RUNWORKSPACE_H
#define RUNWORKSPACE_H

#include <string>

struct RunWorkspaceResult {
    bool ok = false;
    std::string runDir;
    std::string scriptSrc;
    std::string scriptDst;
    std::string helperSrc;
    std::string helperDst;
    std::string error;
};

RunWorkspaceResult prepareRunWorkspace(const std::string& scriptPath);

#endif //RUNWORKSPACE_H
