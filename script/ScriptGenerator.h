#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include "../rabbitmq-access-data/RabbitMQAccessData.h"

#include <string>
#include <atomic>

struct ScriptResult {
    std::string name;
    std::string content;

    ScriptResult() = default;
    ScriptResult(std::string name, std::string content)
        : name(std::move(name)), content(std::move(content)) {}
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