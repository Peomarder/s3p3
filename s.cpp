#include <net/if.h>
#include <sys/ioctl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include "subp.h"
#include "chttp/httplib.h" //kino lib btw

#include <thread>
#include <mutex>


using namespace std;

std::mutex mtx;
const int PORT = 7432;
string generateOrderId(){
	ifstream pkSF("scheme1/order_pk_sequence.txt"); 
	if(!pkSF.is_open()) {
    return "error";  // Should handle file errors!
	}
		string SF;
		pkSF >> SF;
		pkSF.close(); 
		return "id"+SF;
}

std::string tToJsonOrder(const std::string& tableStr) {
    std::stringstream ss(tableStr);
    std::string result = "[\n"; //idk if I should add \r
    std::string line;
    bool firstRow = true;

    while(std::getline(ss, line)) {
        if(!firstRow) result += ",\n";
        firstRow = false;

        std::stringstream lineStream(line);
        std::string orderId, userId, lotId, quantity, type, price, closed;
        lineStream >> orderId >> userId >> lotId >> quantity >> type >> price >> closed;

        result += "  {\n";
        result += "\t\"order_id\": " + orderId + ",\n";
        result += "\t\"user_id\": " + userId + ",\n";
        result += "\t\"lot_id\": " + lotId + ",\n";
        result += "\t\"quantity\": " + quantity + ",\n";
        result += "\t\"type\": \"" + type + "\",\n";
        result += "\t\"price\": " + price + ",\n";
        result += "\t\"closed\": "" + closed + ""\n";
        result += "  }";
    }
    
    result += "\n]";
    return result;
}

// Server class
class ExchangeServer {
private:
    httplib::Server server;

public:
ExchangeServer() {
std::cout << "Server initialized on port " << PORT << "\ n";
}
    void start() {
        // Create user endpoint
        server.Post("/user", [](const httplib::Request& req, httplib::Response& res) {
            auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            string generated_key = tocharints(username); 					//idk no user key specification so idc
            // Database operation
            subp("INSERT INTO user VALUES ('" + ("id"+tocharints(username))+//id
                 "', '" + username + "', '" 								//name
				 + generated_key + "')");									//key
            
            res.set_content("{"key": "generated_key"}", "application/json");
        });

        // Create order endpoint
        server.Post("/order", [](const httplib::Request& req, httplib::Response& res) {
            auto userKey = req.get_header_value("X-USER-KEY");
            auto j = nlohmann::json::parse(req.body);					//hi json babe
            string order_id = generateOrderId();
            std::string query = "INSERT INTO order VALUES ('" +
                order_id + "', '" +
                "id"+userKey + "', '" +
                std::to_string(j["pair_id"].get<int>()) + "', '" + 		//idk why handle these as int and float when they become string again,
                std::to_string(j["quantity"].get<float>()) + "', '" + 	//so like you could just check if they were numbers or idk
                std::to_string(j["price"].get<float>()) + "', '" +
                j["type"].get<std::string>() + "', '')";
            
            subp(query);
            res.set_content("{order_id: "order_id"}", "application/json");
        });

        // Get orders endpoint
        server.Get("/order", [](const httplib::Request&, httplib::Response& res) {
            std::string result = tToJsonOrder(subp("SELECT order.order_id,order.user_id,order.lot_id,order.quantity,order.type,order.price,order.closed FROM order"));
            res.set_content(result, "application/json");
        });

        // Get lots endpoint
        server.Get("/lot", [](const httplib::Request&, httplib::Response& res) {
            std::string result = subp("SELECT lot.lot_id,lot.name FROM lot");
            res.set_content(result, "application/json");
        });

        server.listen("0.0.0.0", PORT);
    }
};








std::string testBackendFunction(const std::string& username, const std::string& password, const std::string& message) {
	cout<<"\n test";
return "User: " + username + " sent message: \n" + subp(message);
}

std::string processRequest(const char* request) {
std::string req(request);
size_t firstSpace = req.find(' ');
size_t secondSpace = req.find(' ', firstSpace + 1);

if(firstSpace == std::string::npos || secondSpace == std::string::npos) {
return "Invalid format";
}

std::string username = req.substr(0, firstSpace);
std::string password = req.substr(firstSpace + 1, secondSpace - firstSpace - 1);
std::string message = req.substr(secondSpace + 1);

return testBackendFunction(username, password, message);
}

void handleClient(int clientSocket) {
{
std::lock_guard<std::mutex> lock(mtx);
std::cout << "Handling client in thread... Socket: " << clientSocket << std::endl;
}

while(true) {
std::cout << "Waiting for message..." << std::endl;  // Debug print
char buffer[1024] = {0};
int bytesRead = read(clientSocket, buffer, 1024);

std::cout << "Bytes read: " << bytesRead << std::endl;  // Debug print

if(bytesRead <= 0) {
std::lock_guard<std::mutex> lock(mtx);
std::cout << "Client disconnected (bytes: " << bytesRead << ")" << std::endl;
break;
}

{
std::lock_guard<std::mutex> lock(mtx);
std::cout << "Received: " << buffer << std::endl;
}

std::string response = processRequest(buffer);
std::cout << "Sending response..." << std::endl;  // Debug print
send(clientSocket, response.c_str(), response.length(), 0);
}

close(clientSocket);
}

int main() {
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
ExchangeServer server;
server.start();  // "ctrl + c" my goat!
return 0;
}
