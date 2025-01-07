struct HttpRequest {
    std::string method;
    std::string path;
    std::string userKey;
    std::string body;
};

class HttpServer {
private:
    int serverSocket;
    DatabaseOperation dbOp;
	
	int charToInt(char c) {
		return c;  // Works for digits 0-9 only
	}
	string generateUserId(string user){
		string result;
		for (auto c:user){
			result = result + to_string(charToInt(c));
		}
	}
	string generateKey(string user){ //idk what this is for
		string result;
		for (auto c:user){
			result =   to_string(charToInt(c)) + result;
		}
	}
	
    std::string processRequest(const HttpRequest& request) {
        if (request.path == "/user" && request.method == "POST") {
            auto json = nlohmann::json::parse(request.body);
            std::string username = json["username"];
            std::string key = generateKey(username); // Some hash function
            
            // Insert new user
            std::string query = "INSERT INTO user VALUES ('" + 
                              generateUserId(username) + "', '" + 
                              username + "', '" + 
                              key + "')";
            subp(query);
            
            return "{"key": "" + key + ""}";
        }
        
        else if (request.path == "/order") {
            if (request.method == "POST") {
                auto json = nlohmann::json::parse(request.body);
                std::string orderId = generateOrderId();
                
                std::string query = "INSERT INTO order VALUES ('" +
                                  orderId + "', '" +
                                  getUserIdFromKey(request.userKey) + "', '" +
                                  std::to_string(json["pair_id"].get<int>()) + "', '" +
                                  std::to_string(json["quantity"].get<float>()) + "', '" +
                                  std::to_string(json["price"].get<float>()) + "', '" +
                                  json["type"].get<std::string>() + "', '')";
                subp(query);
                
                return "{"order_id": " + orderId + "}";
            }
            else if (request.method == "GET") {
                return subp("SELECT * FROM order WHERE closed = ''");
            }
            else if (request.method == "DELETE") {
                auto json = nlohmann::json::parse(request.body);
                std::string query = "UPDATE order SET closed = '" + 
                                  getCurrentTimestamp() + 
                                  "' WHERE order_id = '" + 
                                  std::to_string(json["order_id"].get<int>()) + "'";
                return subp(query);
            }
        }
        
        else if (request.path == "/lot" && request.method == "GET") {
            return subp("SELECT * FROM lot");
        }
        
        else if (request.path == "/pair" && request.method == "GET") {
            return subp("SELECT * FROM pair");
        }
        
        else if (request.path == "/balance" && request.method == "GET") {
            std::string userId = getUserIdFromKey(request.userKey);
            return subp("SELECT lot_id, quantity FROM user_lot WHERE user_id = '" + userId + "'");
        }
        
        return "{"error": "Invalid request"}";
    }

public:
    HttpServer(int port) {
        // Standard socket setup code here
    }
    
    void start() {
        while(true) {
            // Accept connection
            // Read request
            HttpRequest request = parseHttpRequest(readRequest());
            std::string response = processRequest(request);
            // Send response
        }
    }
};