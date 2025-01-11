#include <iostream>

//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "chttp/httplib.h"
#include "json.hpp"

using namespace std;
bool isNumber(const std::string& str) {
    if(str.empty()) return false;
    bool hasDecimal = false;
    
    for(size_t i = 0; i < str.length(); i++) {
        if(i == 0 && str[i] == '-') continue;
        if(str[i] == '.' && !hasDecimal) {
            hasDecimal = true;
            continue;
        }
        if(!isdigit(str[i])) return false;
    }
    return true;
}
// Client class
class ExchangeClient {
private:
    httplib::Client client;

public:
    ExchangeClient(const std::string& host = "127.0.0.1", int port = 7432) 
    : client(host, port) {}

    std::string createUser(const std::string& username) {
		cout<<"userpost2";
        nlohmann::json j{{"username", username}};
        auto res = client.Post("/user", j.dump(), "application/json");
		cout<<"userpost3";
        return res->body;
    }

 std::string createOrder(const std::string& userKey, int pairId, 
                          float quantity, float price, string isBuy) {
							 // if ((isBuy!="buy")&&(isBuy!="sell")){ return "format error";}
        nlohmann::json j{
            {"pair_id", pairId},
            {"quantity", quantity},
            {"price", price},
            {"type", isBuy} //buy sell
        };
        httplib::Headers headers = {{"X-USER-KEY", userKey}};
        auto res = client.Post("/order", headers, j.dump(), "application/json");
        return res->body;
    }

    std::string getOrders() {
        auto res = client.Get("/order");
        return res->body;
    }

    std::string getLots() {
        auto res = client.Get("/lot");
        return res->body;
    }
	
    std::string getPair() {
        auto res = client.Get("/pair");
        return res->body;
    }
	std::string getBal(const std::string& userKey) {
        httplib::Headers headers = {{"X-USER-KEY", userKey}};
        auto res = client.Get("/balance", headers);
        return res->body;
    }
	  std::string DelOrder(const std::string& userKey, int order_id) {
        httplib::Headers headers = {{"X-USER-KEY", userKey}};
		nlohmann::json j{
            {"order_id", order_id} //order_id
        };
        auto res = client.Delete("/order", headers, j.dump(), "application/json");
        return res->body;
    }
	
};



int main() {
int client_fd;
struct sockaddr_in server_addr;
int server_len = sizeof(server_addr);
char message[1024] = {0};
char reply[1024] = {0};
auto exchange = std::make_unique<ExchangeClient>();


string userkey = "";
std::cout << "Connected to server... maybe" << std::endl;
string restOfLine = "";
bool firstRun = true;

while (true) {
	restOfLine = "";
	std::cout << "Enter message (or QUIT to exit): ";
	
	std::cin.getline(message, 1024);

	if (std::string(message) == "QUIT") {
	break;
	}
	cout<< "1";
	string msg = std::string(message);
	std::stringstream ss(msg);
	cout<< "3";
	string firstWord = "";
	ss>>firstWord;
	cout<< "3";
		if (firstWord == "LOGIN") {
			ss>>restOfLine;
			userkey = restOfLine;
		}
		
		if (firstWord == "REG") {
			ss>>restOfLine;
			cout<<"userpost1\n";
			cout << exchange->createUser(restOfLine);
		}
		if (userkey == "") { cout << "logged out."; continue;}
		if (firstWord == "ORDER") {
			string isBuy;
			int pairId;
			float quantity,price;
			ss>>pairId>>quantity>>price>>isBuy;
			cout << exchange->createOrder(userkey, pairId, quantity, price, isBuy);
		}

		if (firstWord == "ORDERS") {
			cout << exchange->getOrders();
		}
		if (firstWord == "LOTS") {
			cout << exchange->getLots();
		}
		if (firstWord == "BALANCE") {
			cout << exchange->getBal(userkey);
		}
		if (firstWord == "DelOrder") {
			int ordId;
			ss>>ordId;
			cout << exchange->getLots(userkey,ordId);
		}

}

// Close socket

return 0;
}
