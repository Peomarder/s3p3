#include <net/if.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT
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
#include "json.hpp"

#include <thread>
#include <mutex>

using namespace std;
using json = nlohmann::json;

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
        result += "\t\"closed\": \"" + closed + "\"\n";
        result += "  }";
    }
    
    result += "\n]";
    return result;
}
void stringToCharArray(const std::string& input, char output[], size_t maxSize) {
    size_t length = std::min(input.length(), maxSize - 1);
    std::strncpy(output, input.c_str(), length);
 //   output[length] = '';
}

/*
// Server class
class ExchangeServer {
private:
    httplib::Server server;

public:
ExchangeServer() {
std::cout << "Server initialized on port " << PORT << "\n";
}
    void start() {
        // Create user endpoint
        server.Post("/user", [](const httplib::Request &req, httplib::Response &res) {
            auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            string generated_key = tocharints(username); 					//idk no user key specification so idc
            // Database operation
            subp("INSERT INTO user VALUES ('" + ("id"+tocharints(username))+//id
                 "', '" + username + "', '" 								//name
				 + generated_key + "')");									//key
            
            res.set_content("{\n\"key\": "+generated_key+"\n}", "application/json");
        });

        // Create order endpoint
        server.Post("/order", [](const httplib::Request &req, httplib::Response &res) {
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
            res.set_content("{\norder_id: "+order_id+"\n}", "application/json");
        });

        // Get orders endpoint
        server.Get("/order", [](const httplib::Request &, httplib::Response &res) {
            std::string result = tToJsonOrder(subp("SELECT order.order_id,order.user_id,order.lot_id,order.quantity,order.type,order.price,order.closed FROM order"));
            res.set_content(result, "application/json");
        });

        // Get lots endpoint
        server.Get("/lot", [](const httplib::Request &, httplib::Response &res) {
            std::string result = subp("SELECT lot.lot_id,lot.name FROM lot");
            res.set_content(result, "application/json");
        });

while(1){
        server.listen("0.0.0.0", PORT);
}
    }
};

*/





int main() {
//int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//ExchangeServer server;
int db;
cin>>db;
if(db){subp("newdb");}

		httplib::Server server;

        // Create user endpoint
        server.Post("/user", [](const httplib::Request &req, httplib::Response &res) {
            cout<<"POST USER\n";
			auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            string generated_key = tocharints(username); 					//idk no user key specification so idc
            // Database operation
            subp("INSERT INTO user VALUES ('" + ("id"+tocharints(username))+//id
                 "', '" + username + "', '" 								//name
				 + generated_key + "')");									//key
            
            res.set_content("{\n\"key\": "+generated_key+"\n}", "application/json");
        });

        // Create order endpoint
        server.Post("/order", [](const httplib::Request &req, httplib::Response &res) {
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
            res.set_content("{\norder_id: "+order_id+"\n}", "application/json");
        });

        // Get orders endpoint
        server.Get("/order", [](const httplib::Request &, httplib::Response &res) {
            std::string result = tToJsonOrder(subp("SELECT order.order_id,order.user_id,order.lot_id,order.quantity,order.type,order.price,order.closed FROM order"));
            res.set_content(result, "application/json");
        });

        // Get lots endpoint
        server.Get("/lot", [](const httplib::Request &, httplib::Response &res) {
            std::string result = subp("SELECT lot.lot_id,lot.name FROM lot");
            res.set_content(result, "application/json");
        });
		cout<<"Listening\n";
        server.listen("0.0.0.0", PORT);


//server.start();  // "ctrl + c" my goat!

return 0;
}
