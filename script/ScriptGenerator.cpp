#include "ScriptGenerator.h"

#include <ctime>

#include "../util/Logger.h"
#include "../util/Utils.h"
#include <sstream>
#include <vector>

RabbitMQAccessData::RabbitMQAccessData(const std::string &host, const std::string &port, const std::string &vhost,
        const std::string &username, const std::string &password)
        : host(host),
          port(port),
          vhost(vhost),
          username(username),
          password(password) {
}

ScriptGenerator::ScriptGenerator(const std::string& tags,
                                 const std::string& params,
                                 const std::string& propertiesTemplate,
                                 const std::string& payloadTemplate,
                                 const std::string& routingkey,
                                 const std::string& exchange,
                                 RabbitMQAccessData* rabbitMQAccessData)
    : tags(tags), params(params),
    propertiesTemplate(propertiesTemplate), payloadTemplate(payloadTemplate),
    routingkey(routingkey), exchange(exchange),
    rabbitMQAccessData(rabbitMQAccessData)
{
}

ScriptResult ScriptGenerator::generateScript() {
    size_t pos;

    std::string prefix = "Tags: ";
    if (tags.rfind(prefix, 0) == 0) {
        tags = tags.substr(prefix.length());
    }

    std::vector<std::string> fieldNames;
    std::istringstream tagsStream(tags);
    std::string field;
    while (std::getline(tagsStream, field, '|')) {
        field = Utils::trim(field);
        if (!field.empty()) {
            fieldNames.push_back(field);
        }
    }

    std::ostringstream output;

    output << "#!/bin/bash\n\n";
    output << "RABBITMQ_USERNAME=" << rabbitMQAccessData->username << "\n";
    output << "RABBITMQ_PASSWORD=" << rabbitMQAccessData->password << "\n";
    output << "RABBITMQ_HOST=" << rabbitMQAccessData->host << "\n";
    output << "RABBITMQ_PORT=" << rabbitMQAccessData->port << "\n";
    output << "RABBITMQ_VIRTUAL_HOST='" << rabbitMQAccessData->vhost << "'\n\n";

    while ((pos = propertiesTemplate.find("{{RANDOM_UUID}}")) != std::string::npos) {
        std::string randomUuid = Utils::generateUUID();
        propertiesTemplate.replace(pos, 15, randomUuid);
    }
    while ((pos = payloadTemplate.find("{{RANDOM_UUID}}")) != std::string::npos) {
        std::string randomUuid = Utils::generateUUID();
        payloadTemplate.replace(pos, 15, randomUuid);
    }
    std::string now = Utils::getCurrentDateTime();
    while ((pos = propertiesTemplate.find("{{NOW}}")) != std::string::npos)
        propertiesTemplate.replace(pos, 7, now);

    while ((pos = payloadTemplate.find("{{NOW}}")) != std::string::npos)
        payloadTemplate.replace(pos, 7, now);

    std::istringstream paramStream(params);
    std::string line;
    size_t command_counter = 0;

    while (std::getline(paramStream, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> values;
        std::istringstream valueStream(line);
        std::string value;
        while (std::getline(valueStream, value, '|')) {
            values.push_back(Utils::trim(value));
        }

        std::string filledProperties = propertiesTemplate;
        std::string filledPayload = payloadTemplate;

        for (size_t i = 0; i < fieldNames.size(); ++i) {
            std::string tag = "{{" + fieldNames[i] + "}}";
            std::string replacement = (i < values.size()) ? values[i] : "";

            while ((pos = filledProperties.find(tag)) != std::string::npos) {
                filledProperties.replace(pos, tag.length(), replacement);
            }
            while ((pos = filledPayload.find(tag)) != std::string::npos) {
                filledPayload.replace(pos, tag.length(), replacement);
            }
        }

        output << "properties=$(cat <<EOF\n" << filledProperties << "\nEOF\n)\n\n";
        output << "payload=$(cat <<EOF\n" << filledPayload << "\nEOF\n)\n\n";

        output << "./rabbitmqadmin.py -u\"${RABBITMQ_USERNAME}\" -p\"${RABBITMQ_PASSWORD}\" "
               << "-H\"${RABBITMQ_HOST}\" -P\"${RABBITMQ_PORT}\" "
               << "publish routing_key=\"" << routingkey << "\" exchange=" << exchange << " "
               << "properties=\"${properties}\" payload=\"${payload}\"\n";
        //output << "sleep 1\n";
        output << "echo \"Command executed (" << ++command_counter << ")\"\n\n";
    }
    return ScriptResult(generateScriptPath(), output.str());
}

std::atomic<int> ScriptGenerator::scriptcounter{0};

std::string ScriptGenerator::generateScriptPath() {
    std::time_t t = std::time(nullptr);
    std::tm localTm;
#ifdef _WIN32
    localtime_s(&localTm, &t);
#else
    localtime_r(&t, &localTm);
#endif

    char buf[25]; // "YYYY_MM_DD_HH_MM_SS"
    std::strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &localTm);

    ++scriptcounter;
    std::string scriptpath("rsender-data/generated/" + std::string(buf) + "_" + std::to_string(scriptcounter.load()) + ".sh");
    return scriptpath;
}
