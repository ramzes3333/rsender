#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include <string>
#include <atomic>

class RabbitMQAccessData {
public:
    RabbitMQAccessData(const std::string &host, const std::string &port, const std::string &vhost,
        const std::string &username, const std::string &password);

    const std::string host;
    const std::string port;
    const std::string vhost;
    const std::string username;
    const std::string password;
};

struct ScriptResult {
    std::string name;
    std::string content;
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