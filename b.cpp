// Интерфейс для всех запросов
class IExchangeClient {
public:
    virtual std::string createUser(const std::string& username) = 0;
    virtual std::string createOrder(const std::string& userKey, int pairId, 
                                  float quantity, float price, bool isBuy) = 0;
    virtual std::string getOrders() = 0;
    virtual std::string deleteOrder(const std::string& userKey, int orderId) = 0;
    virtual std::string getLots() = 0;
    virtual std::string getPairs() = 0;
    virtual std::string getBalance(const std::string& userKey) = 0;
    virtual ~IExchangeClient() = default;
};

// Реализация через curl
class ExchangeClient : public IExchangeClient {
private:
    HttpClient client;

public:
    ExchangeClient(const std::string& host = "127.0.0.1", int port = 8080)
        : client(host, port) {}

    std::string createUser(const std::string& username) override {
        nlohmann::json data{{"username", username}};
        return client.sendRequest("/user", "POST", data.dump());
    }

    std::string createOrder(const std::string& userKey, string pairId,
                          string quantity, string price, string isBuy) override {
        nlohmann::json data{
            {"pair_id", pairId},
            {"quantity", quantity},
            {"price", price},
            {"type", isBuy}
        };
        return client.sendRequest("/order", "POST", data.dump(), userKey);
    }

    std::string getOrders() override {
        return client.sendRequest("/order", "GET");
    }

    std::string deleteOrder(const std::string& userKey, int orderId) override {
        nlohmann::json data{{"order_id", orderId}};
        return client.sendRequest("/order", "DELETE", data.dump(), userKey);
    }

    std::string getLots() override {
        return client.sendRequest("/lot", "GET");
    }

    std::string getPairs() override {
        return client.sendRequest("/pair", "GET");
    }
    std::string getBalance(const std::string& userKey) override {
        return client.sendRequest("/balance", "GET", "", userKey);
    }
};

auto exchange = std::make_unique<ExchangeClient>();
auto userKey = exchange->createUser("AFD");
auto orderInfo = exchange->createOrder(userKey, 1, 100.0f, 50.0f, true);