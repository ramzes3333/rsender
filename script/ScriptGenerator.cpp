#include "ScriptGenerator.h"

#include <ctime>

#include "../util/Utils.h"
#include "../Const.h"
#include "../rabbitmq-access-data/RabbitMQAccessData.h"
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

    std::string tagsStr = tags;
    const std::string prefix = "Tags: ";
    if (tagsStr.rfind(prefix, 0) == 0)
        tagsStr = tagsStr.substr(prefix.length());

    std::vector<std::string> fieldNames;
    {
        std::istringstream tagsStream(tagsStr);
        std::string field;
        while (std::getline(tagsStream, field, '|')) {
            field = Utils::trim(field);
            if (!field.empty())
                fieldNames.push_back(field);
        }
    }

    std::ostringstream output;
    auto append = [&](const std::string& s) {
        output << s;
    };

    // Header
    append("#!/bin/bash\n\n");
    append("RABBITMQ_USERNAME=" + rabbitMQAccessData->username + "\n");
    append("RABBITMQ_PASSWORD=" + rabbitMQAccessData->password + "\n");
    append("RABBITMQ_HOST="     + rabbitMQAccessData->host     + "\n");
    append("RABBITMQ_PORT="     + rabbitMQAccessData->port     + "\n");
    append("RABBITMQ_VIRTUAL_HOST='" + rabbitMQAccessData->vhost + "'\n\n");

    std::string propsTpl   = propertiesTemplate;
    std::string payloadTpl = payloadTemplate;

    while ((pos = propsTpl.find("{{RANDOM_UUID}}")) != std::string::npos)
        propsTpl.replace(pos, 15, Utils::generateUUID());
    while ((pos = payloadTpl.find("{{RANDOM_UUID}}")) != std::string::npos)
        payloadTpl.replace(pos, 15, Utils::generateUUID());

    const std::string now = Utils::getCurrentDateTime();
    while ((pos = propsTpl.find("{{NOW}}")) != std::string::npos)
        propsTpl.replace(pos, 7, now);
    while ((pos = payloadTpl.find("{{NOW}}")) != std::string::npos)
        payloadTpl.replace(pos, 7, now);

    std::istringstream paramStream(params);
    std::string line;
    size_t command_counter = 0;

    while (std::getline(paramStream, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> values;
        {
            std::istringstream valueStream(line);
            std::string value;
            while (std::getline(valueStream, value, '|'))
                values.push_back(Utils::trim(value));
        }

        std::string filledProps   = propsTpl;
        std::string filledPayload = payloadTpl;

        for (size_t i = 0; i < fieldNames.size(); ++i) {
            const std::string tag = "{{" + fieldNames[i] + "}}";
            const std::string& replacement = (i < values.size()) ? values[i] : std::string{};

            size_t at = 0;
            while ((at = filledProps.find(tag, at)) != std::string::npos) {
                filledProps.replace(at, tag.length(), replacement);
                at += replacement.length();
            }
            at = 0;
            while ((at = filledPayload.find(tag, at)) != std::string::npos) {
                filledPayload.replace(at, tag.length(), replacement);
                at += replacement.length();
            }
        }

        append("properties=$(cat <<EOF\n" + filledProps + "\nEOF\n)\n\n");
        append("payload=$(cat <<EOF\n" + filledPayload + "\nEOF\n)\n\n");

        std::ostringstream cmd;
        cmd << "./rabbitmqadmin.py -u\"${RABBITMQ_USERNAME}\" -p\"${RABBITMQ_PASSWORD}\" "
            << "-H\"${RABBITMQ_HOST}\" -P\"${RABBITMQ_PORT}\" "
            << "-V\"${RABBITMQ_VIRTUAL_HOST}\" "
            << "publish routing_key=\"" << routingkey << "\" exchange=" << exchange << " "
            << "properties=\"${properties}\" payload=\"${payload}\"\n";
        append(cmd.str());

        std::ostringstream echo;
        echo << "echo \"Command executed (" << (++command_counter) << ")\"\n\n";
        append(echo.str());
    }

    std::string scriptContent = output.str();

    ScriptReport rep;
    rep.totalScriptBytes   = scriptContent.size();
    rep.exceeds64k = (rep.totalScriptBytes > k64KiB);

    return ScriptResult(generateScriptPath(), std::move(scriptContent), rep);
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
