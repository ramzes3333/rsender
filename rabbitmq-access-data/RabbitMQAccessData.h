#ifndef RABBITMQACCESSDATA_H
#define RABBITMQACCESSDATA_H

#include <string>
#include <optional>

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

bool saveRabbitParams(const RabbitMQAccessData& data,
                      const std::string& filePath = "rsender-data/last_rabbitmq.json",
                      std::string* errorOut = nullptr);

std::optional<RabbitMQAccessData> loadRabbitParams(
                      const std::string& filePath = "rsender-data/last_rabbitmq.json",
                      std::string* errorOut = nullptr);

#endif //RABBITMQACCESSDATA_H
