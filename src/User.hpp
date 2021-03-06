#ifndef TRAINBOOM_USER_HPP
#define TRAINBOOM_USER_HPP

#include "util/map.hpp"
#include "util/Json.hpp"
#include "util/stupid_ptr.hpp"
#include "util/vector.hpp"
#include "Id.hpp"
#include "DataManager2.hpp"
#include "Order.hpp"
#include <string>
#include <vector>
#include <cassert>

namespace trainBoom {

enum Gender{Man=0, Woman=1, Other=2};

class User {
    //friend class Password;//TODO

private:
    std::string password; // SHA-1 checksum (:password|:salt)
    std::string salt; // 8-bit random string
    std::string username,avatar,realname,phone,email,motto;
    Gender gender;//Man, Woman, Other
    bool isRoot;
    util::map<std::string, Order> orders;
    std::string id;

public:
    class information_missing: public exception {
        public:
            information_missing(const std::string& info):
                exception (
                        "information_missing",
                        "Your " + info + " is missing!!!") {}
    };
    class id_not_exist: public exception {
        public:
            id_not_exist():
                exception (
                        "id_not_exist",
                        "Your id does not exist!!!") {}
    };
    User (): id(Id("User")) {}
	User (const Json& json): gender(Other), isRoot(false) {
        if (json.getId() != "") {
            id = json.getId();
        }
        else {
            id = Id("User");
        }
//        std::cout << "!" << std::endl;
//        std::cout << json.toString() << std::endl;
        if (json.HasMember("password")) password = json["password"].as<std::string>();
        else if (json.HasMember("p")) password = json["p"].as<std::string>();
        else {
            throw information_missing("password");
        }
        if (json.HasMember("salt")) salt = json["salt"].as<std::string>();
        else if (json.HasMember("s")) salt = json["s"].as<std::string>();
        else {
            throw information_missing("salt");
        }
        if (json.HasMember("username")) username = json["username"].as<std::string>();
        else if (json.HasMember("u")) username = json["u"].as<std::string>();
        else {
            throw information_missing("username");
        }
//        std::cout << username << std::endl;
//        assert(!json.HasMember("avatar"));
        if (json.HasMember("avatar")) avatar = json["avatar"].as<std::string>();
        else if (json.HasMember("a")) avatar = json["a"].as<std::string>();
        if (json.HasMember("realname")) realname = json["realname"].as<std::string>();
        else if (json.HasMember("r")) realname = json["r"].as<std::string>();
        if (json.HasMember("phone")) phone = json["phone"].as<std::string>();
        else if (json.HasMember("ph")) phone = json["ph"].as<std::string>();
        if (json.HasMember("email")) email = json["email"].as<std::string>();
        else if (json.HasMember("e")) email = json["e"].as<std::string>();
        if (json.HasMember("motto")) motto = json["motto"].as<std::string>();
        else if (json.HasMember("m")) motto = json["m"].as<std::string>();
        if (json.HasMember("gender")) gender = Gender(json["gender"].as<int>());
        else if (json.HasMember("g")) gender= Gender(json["g"].as<int>());
        if (json.HasMember("isRoot")) isRoot = json["isRoot"].as<bool>();
        else if (json.HasMember("i")) isRoot = json["i"].as<int>();

        if (json.HasMember("ordersId")) {
            std::string ordersId = json["ordersId"];
            orders = util::map<std::string, Order>::load(ordersId);
        }
    }
    // User (std::string id, stupid_ptr<BinaryFile> bfp): User(Json().read(id, bfp)) {}
    static User load(std::string id) {
        return User(DataManager::getJson(id));
    }
	// User operator=(const User &t):id(t.id),username(t.username),password(t.password),avatar(t.avatar),realname(t.realname),phone(t.phone),email(t.email),motto(t.motto),gender(t.gender),root(t.root),order(t.order) {}
    std::string getId() const {return id;}
	void update(const Json& json) {
        if (json.HasMember("username")) username = json["username"].as<std::string>();
        if (json.HasMember("avatar")) avatar = json["avatar"].as<std::string>();
        if (json.HasMember("realname")) realname = json["realname"].as<std::string>();
        if (json.HasMember("phone")) phone = json["phone"].as<std::string>();
        if (json.HasMember("email")) email = json["email"].as<std::string>();
        if (json.HasMember("motto")) motto = json["motto"].as<std::string>();
        if (json.HasMember("gender")) gender = Gender(json["gender"].as<int>());
        if (json.HasMember("isRoot")) isRoot = json["isRoot"].as<bool>();
    }

	std::string getUsername() const {return username;}
	// Password getPassword() {return password;}//eererewre
    bool verifyPassword(const std::string& _password) const {
        return _password == password;
    }
	std::string getAvatar() const {return avatar;}
	std::string getRealname() const {return realname;}
	std::string getPhone() const {return phone;}
	std::string getEmail() const {return email;}
	std::string getMotto() const {return motto;}
	Gender getGender() const {return gender;}
	bool getIsRoot() const {return isRoot;}
    const Order& order(const std::string& id) const {
        if (!orders.count(id)) {
            throw id_not_exist();
        }
        else {
            return orders.at(id);
        }
    }
	util::vector<std::string> getOrders() const {
        util::vector<std::string> ordersVec;
        for (const auto& item: orders)
            ordersVec.push_back(item.first);
        return ordersVec;
    }
/*	void modifyPassword(Password t) {password=t;}
	void modifyUsername(std::string t) {username=t;}
	void modifyAvatar(std::string t) {avatar=t;}
	void modifyRealname(std::string t) {realname=t;}
	void modifyPhone(std::string t) {phone=t;}
	void modifyEmail(std::string t) {email=t;}
	void modifyMotto(std::string t) {motto=t;}
	void modifyGender(Gender t) {gender=t;}
	void modifyIsRoot(bool t) {isRoot=t;}*/
    void addOrder(const Order& order) {
        orders.insert(util::make_pair(order.getId(), order));
    }
	/*void bookTicket(Train train, Station from, Station to, size_t lowPrice, size_t highPrice, int num) {}
	void refundTicket(Train train, Station from, Station to, size_t lowPrice, size_t highPrice, int num) {}
	void queryTicket(Station from, Station to, Datetime::Datetime date) {}
	void addRoute(Route route) {}//ROOT
	void delRoute(Id routeID) {}//ROOT
	void startRoutes(Train train, Datetime::Datetime date) {}//ROOT
	void stopRoutes(Train train, Datetime::Datetime date) {} //ROOT
	std::string getUsername(Id UserId) {}
	Password getPassword(Id UserId) {}//ROOT
	std::string getAvatar(Id UserId) {}
	std::string getRealname(Id UserId) {}//ROOT
	std::string getPhone(Id UserId) {}
	std::string getEmail(Id UserId) {}
	Gender getGender(Id UserId) {}
	std::string getMotto(Id UserId) {}
	bool getRoot(Id UserId) {}
	std::vector<Id> getOrder(Id UserId) {}
	void getLog(Id UserId) {}//ROOT*/

    util::Json toJson(bool simple = false) const {
        util::Json json("user", id);
        json[simple ? "u" : "username"] = username;
        json[simple ? "s" : "salt"] = salt;
        json[simple ? "p" : "password"] = password;
        if (avatar.size()) json[simple ? "a" : "avatar"] = avatar;
        if (realname.size()) json[simple ? "r" : "realname"] = realname;
        if (phone.size()) json[simple ? "ph" : "phone"] = phone;
        if (email.size()) json[simple ? "e" : "email"] = email;
        if (motto.size()) json[simple ? "m" : "motto"] = motto;
        json[simple ? "g" : "gender"] = gender;
        if (simple) {
            json["i"] = int(isRoot);
        }
        else {
            json["isRoot"] = isRoot;
        }
        return json;
    }

    void save() {
        Json tmp = toJson(true);
        if (!orders.empty()) {
            orders.save();
            tmp["ordersId"] = orders.getId();
        }
        // tmp.write(DataManager::getFile(id));
        DataManager::save(tmp);
    }
/*
    util::Json getOrderJson() const {
        util::Json json("order");
        json["UserId"] = id;
        json["order"].SetArray();
        for (const auto& item: order) {
            json["order"].PushBack(item);
        }
        return json;
    }
    */
};

}

#endif
