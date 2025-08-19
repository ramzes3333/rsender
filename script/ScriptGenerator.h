#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include "../rabbitmq-access-data/RabbitMQAccessData.h"

#include <string>
#include <atomic>

struct ScriptReport {
    std::size_t totalScriptBytes = 0;
    bool exceeds64k = false;
};

struct ScriptResult {
    std::string name;
    std::string content;
    ScriptReport report;

    ScriptResult() = default;
    ScriptResult(std::string name, std::string content, ScriptReport report = {})
        : name(std::move(name)), content(std::move(content)), report(std::move(report)) {}
};

class ScriptGenerator {
public:
    ScriptGenerator(const std::string& tags,
                    const std::string& params,
                    const std::string& propertiesTemplate,
                    const std::string& payloadTemplate,
                    const std::string& routingkey,
                    const std::string& exchange,
                    RabbitMQAccessData* rabbitMQAccessData);

    ScriptResult generateScript();

private:
    std::string tags;
    std::string params;
    std::string propertiesTemplate;
    std::string payloadTemplate;
    std::string routingkey;
    std::string exchange;
    RabbitMQAccessData* rabbitMQAccessData;

    static std::atomic<int> scriptcounter;

    std::string generateScriptPath();
};

#endif //SCRIPTGENERATOR_H