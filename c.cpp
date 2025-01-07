#include <iostream>
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
    ExchangeClient(const std::string& host = "127.0.0.1", int port = 8080) 
    : client(host, port) {}

    std::string createUser(const std::string& username) {
        nlohmann::json j{{"username", username}};
        auto res = client.Post("/user", j.dump(), "application/json");
        return res->body;
    }

 std::string createOrder(const std::string& userKey, int pairId, 
                          float quantity, float price, string isBuy) {
							  if ((isBuy!="buy")&&(isBuy!="sell")){ return "format error";}
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
};



int main() {
int client_fd;
struct sockaddr_in server_addr;
int server_len = sizeof(server_addr);
char message[1024] = {0};
char reply[1024] = {0};
auto exchange = std::make_unique<ExchangeClient>();

// Create socket
if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
std::cerr << "Failed to create socket" << std::endl;
return -1;
}

// Set server address
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(7432);
string ipcon = "";
cout << "\nEnter ip: ";
cin >> ipcon; //"127.0.0.1"
inet_pton(AF_INET, ipcon.c_str(), &server_addr.sin_addr);

// Connect to server
if (connect(client_fd, (struct sockaddr*)&server_addr, server_len) < 0) {
std::cerr << "Failed to connect to server" << std::endl;
return -1;
}
string userkey = "";
std::cout << "Connected to server..." << std::endl;
string restOfLine = "";

while (true) {
	restOfLine = "";
	std::cout << "Enter message (or QUIT to exit): ";
		if (firstRun) {
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			firstRun = false;
		}
		
	std::cin.getline(message, 1024);

	if (std::string(message) == "QUIT") {
	break;
	}

	std::stringstream ss(string(message));
	string firstWord = "";
	ss>>firstWord;
		if (firstWord == "LOGIN") {
			ss>>restOfLine;
			userkey = restOfLine;
		}
		if (firstWord == "REG") {
			ss>>restOfLine;
			cout << exchange->createUser(restOfLine);
		}

		if (firstWord == "ORDER") {
			string isBuy;
			int pairId;
			float quantity,price;
			ss>>pairId>>quantity>>price>>isBuy;
			cout << exchange->createOrder(userkey, pairId, quantity, price, isBuy)
		}

		if (firstWord == "ORDERS") {
			cout << exchange->getOrders();
		}






	// Send message to server
	char paddedMessage[1024] = {0};  // Create padded buffer
	strcpy(paddedMessage, message);   // Copy message into it
	send(client_fd, paddedMessage, 1024, MSG_WAITALL);  // Send full buffer

	// Receive reply from server
	read(client_fd, reply, 1024);

	std::cout << "Server replied: " << reply << std::endl;
}

// Close socket
close(client_fd);

return 0;
}
