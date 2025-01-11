//#include <net/if.h>
//#define CPPHTTPLIB_OPENSSL_SUPPORT
//#include <sys/ioctl.h>
#include <iostream>
#include <string>
	#include <filesystem>
#include <iostream>
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
std::cout << std::filesystem::current_path() << std::endl;
	ifstream pkSF("birja/order/order_pk_sequence.txt"); 
	

if(!pkSF.is_open()) {
    return "error";  // Should handle file errors!
	}
	
		string SF;
		pkSF >> SF;
		pkSF.close(); 
		return SF;
}
std::string genKey(const std::string& seed) {
    std::hash<std::string> hasher;
    std::mt19937 generator(hasher(seed));
    std::string result;
    
    for(int i = 0; i < 4; i++) {
        char randomChar = 'A' + (generator() % 26); //I don't want the entire character range tbh
        result += randomChar;
    }
    return result;
}

		string getUserid(string userKey){
			string userid = subp("SELECT user.user_id FROM user WHERE user.user_key = '"+userKey +"'");
			userid.erase(std::remove_if(str.begin(), str.end(), 
              [](unsigned char c) { return std::isspace(c); }),
              str.end());
			return userid;
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

std::string tToJsonLot(const std::string& tableStr) {
    std::stringstream ss(tableStr);
    std::string result = "[\n"; //idk if I should add \r
    std::string line;
    bool firstRow = true;

    while(std::getline(ss, line)) {
        if(!firstRow) result += ",\n";
        firstRow = false;

        std::stringstream lineStream(line);
        std::string name, lot_id;
        lineStream >> lot_id >> name;

        result += "  {\n";
        result += "\t\"lot_id\": " + lot_id + ",\n";
        result += "\t\"name\": \"" + name + "\"\n";
        result += "  }";
    }
    
    result += "\n]";
    return result;
}

std::string tToJsonPair(const std::string& tableStr) {
    std::stringstream ss(tableStr);
    std::string result = "[\n"; //idk if I should add \r
    std::string line;
    bool firstRow = true;

    while(std::getline(ss, line)) {
        if(!firstRow) result += ",\n";
        firstRow = false;

        std::stringstream lineStream(line);
        std::string salepi, buypi, lot_id;
        lineStream >> pair_id >> salepi >> buypi;

        result += "  {\n";
        result += "\t\"pair_id\": " + pair_id + ",\n";
        result += "\t\"sale_lot_id\": \"" + salepi + "\"\n";
        result += "\t\"buy_lot_id\": \"" + buypi + "\"\n";
        result += "  }";
    }
    
    result += "\n]";
    return result;
}

std::string tToJsonBalance(const std::string& tableStr) {
    std::stringstream ss(tableStr);
    std::string result = "[\n"; //idk if I should add \r
    std::string line;
    bool firstRow = true;

    while(std::getline(ss, line)) {
        if(!firstRow) result += ",\n";
        firstRow = false;

        std::stringstream lineStream(line);
        std::string salepi, buypi, lot_id;
        lineStream >> pair_id >> salepi;

        result += "  {\n";
        result += "\t\"lot_id\": " + pair_id + ",\n";
        result += "\t\"quantity\": \"" + salepi + "\"\n";
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


void giveInitialBalance() {
    // Получаем все лоты
    // std::string lotsQuery = "SELECT lot.lot_id, lot.name FROM lot";
    // auto lots = subp(lotsQuery);
	
	string userIds = subp("SELECT user.user_id FROM user");
	string lots = subp("SELECT lot.lot_id FROM lot");
	
    
    std::vector<std::string> lotIds;
    std::istringstream lotStream(lots);
    std::string line;
    subp("DELETE FROM user_lot WHERE user_lot.user_id = user_lot.user_id");
    // Собираем все lot_id
    while(std::getline(lotStream, line)) {
        std::stringstream lineStream(line);
        std::string lotId;
        lineStream >> lotId;
        lotIds.push_back(lotId);
    }
    
    // Для каждого пользователя
    std::istringstream userStream(userIds);
    while(std::getline(userStream, line)) {
        std::stringstream lineStream(line);
        std::string userId;
        lineStream >> userId;
        
        // Даем 1000 каждого лота
        for(const auto& lotId : lotIds) {
            subp("INSERT INTO user_lot VALUES ('" + userId + "', '" + 
                 lotId + "', '1000')");
        }
    }
}
void initializeLots(std::vector<std::string> configlots) {
	
    int pairid = 0;
	subp("DELETE FROM pair WHERE pair.pair_id = pair.pair_id");
	subp("DELETE FROM lot WHERE lot.lot_id = lot.lot_id");
    for(const auto& lot : configlots) {
        subp("INSERT INTO lot VALUES ('"+ str(pairid) + "', '"  + lot + "')");
		pairid++;
    }
	pairid = 0;
    for(size_t i = 0; i < configlots.size(); i++) {
        for(size_t j = i + 1; j < configlots.size(); j++) {
            subp("INSERT INTO pair VALUES ('"+ str(pairid) + "', '"  + 
                 configlots[i] + "', '" + configlots[j] + "')");
        }
		pairid++;
	}
	giveInitialBalance();
}



int main() {
    std::ifstream configContent("config.json");
	json config = json::parse(configContent);
		
	std::vector<std::string> configlots = data["lots"].get<std::vector<std::string>>();
    std::string dIp = data["database_ip"];
    int  dPort = data["database_port"];
	
int db;
cin>>db;
if(db){subp("newdb");}

		httplib::Server server;

        // Create user endpoint
        server.Post("/user", [](const httplib::Request &req, httplib::Response &res) {
            cout<<"POST USER\n";
			auto j = nlohmann::json::parse(req.body);
            std::string username = j["username"];
            string generated_key = genKey(username); 			//idk no user key specification so idc
			string generatedid = tocharints(username); 
            // Database operation
             cout<<"POST USER2\n";
			cout<<subp("INSERT INTO user VALUES ('" + ("id"+generated_key)+//id
                 "', '" + username + "', '" 								//name
				 + generated_key + "')");									//key
             cout<<"POST USER3\n";
            res.set_content("{\n\"key\": "+generated_key+"\n}", "application/json");
        });

        // Create order endpoint
        server.Post("/order", [](const httplib::Request &req, httplib::Response &res) {
            auto userKey = req.get_header_value("X-USER-KEY");
            auto j = nlohmann::json::parse(req.body);					//hi json babe
            string order_id = generateOrderId();
            std::string query = "INSERT INTO order VALUES ('" +
                order_id + "', '" +
                userKey + "', '" +
                std::to_string(j["pair_id"].get<int>()) + "', '" + 		//idk why handle these as int and float when they become string again,
                std::to_string(j["quantity"].get<float>()) + "', '" + 	//so like you could just check if they were numbers or idk
                std::to_string(j["price"].get<float>()) + "', '" +
                j["type"].get<std::string>() + "', '')";
            
            subp(query);
            res.set_content("{\norder_id: "+order_id+"\n}", "application/json");
        });

        // Get orders endpoint
        server.Get("/order", [](const httplib::Request &, httplib::Response &res) {
			cout<<subp("SELECT order.order_id,order.user_id,order.lot_id,order.quantity,order.type,order.price,order.closed FROM order");
            std::string result = tToJsonOrder(subp("SELECT order.order_id,order.user_id,order.pair_id,order.quantity,order.type,order.price,order.closed FROM order"));
            res.set_content(result, "application/json");
        });

        // Get lots endpoint
        server.Get("/lot", [](const httplib::Request &, httplib::Response &res) {
            std::string result = tToJsonLot(subp("SELECT lot.lot_id,lot.name FROM lot"));
            res.set_content(result, "application/json");
        });
		cout<<"Listening\n";
        server.listen("0.0.0.0", PORT);
		// get rid of order
		server.Delete("/order", [](const httplib::Request &, httplib::Response &res) {
            auto userKey = req.get_header_value("X-USER-KEY");
            auto j = nlohmann::json::parse(req.body);
			string userid = getUserid(userKey);
            subp("DELETE FROM order WHERE order.order_id = '"+ std::to_string(j["order_id"].get<int>()) + "' AND order.user_id = '" + userid +"'");
            res.set_content(result, "application/json");
        });
		//get pairs
		 server.Get("/pair", [](const httplib::Request &, httplib::Response &res) {
            std::string result = tToJsonPair(subp("SELECT pair.pair_id,pair.sale_lot_id,pair.buy_lot_id FROM pair"));
            res.set_content(result, "application/json");
        });
		//get balance
		server.Get("/balance", [](const httplib::Request &, httplib::Response &res) {
			auto userKey = req.get_header_value("X-USER-KEY");
            auto j = nlohmann::json::parse(req.body);
			string userid = getUserid(userKey);
            std::string result = tToJsonBalance(subp("SELECT user_lot.lot_id,user_lot.quantity FROM user_lot WHERE user_lot.user_id = '"+userid+"'"));
            res.set_content(result, "application/json");
        });


//server.start();  // "ctrl + c" my goat!

return 0;
}
