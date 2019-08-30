#include "../headers/parsing.hpp"
#include <map>
#include <string>
#include "../headers/math.hpp"
#include <sstream>
#include <iostream>
namespace parsing
{
    const int NOT_FOUND = -1;
    const char ambersand = '&';
    const char equalSign = '=';
    const std::string DEFAULT_USER = "user";
    const std::string UID = "uid";
    const std::string EMAIL = "email";
    const std::string ROLE = "role";
    const std::string ROLE_USER = "user";
    JSON::JSON() {
        this->stringToString = std::map<std::string, std::string>();
        this->stringToInt = std::map<std::string, int>();
        this->stringToDouble = std::map<std::string, double>();
    }
    JSON::~JSON() {}
    double JSON::getDouble(std::string key) {
        if (this->stringToDouble.find(key) == this->stringToDouble.end()) {
            //Throw -1 if the thing was not in the map
            throw NOT_FOUND;
        }
        return this->stringToDouble[key];
    }
    void JSON::addDouble(std::string key, double v) {
        this->stringToDouble[key] = v;
    }
    int JSON::getInt(std::string key) {
        if (this->stringToInt.find(key) == this->stringToInt.end()) {
            throw NOT_FOUND;
        }
        return this->stringToInt[key];
    }
    void JSON::addInt(std::string key, int v) {
        this->stringToInt[key] = v;
    }

    std::string JSON::getString(std::string key) {
        if (this->stringToString.find(key) == this->stringToString.end()) {
            throw NOT_FOUND;
        }
        return this->stringToString[key];
    }

    void JSON::addString(std::string key, std::string v) {
        this->stringToString[key] = v;
    }


    //splitString takes in a string and a character and splits the string along that delimiter.
    std::vector<std::string> splitString(std::string s, char delimiter) {
        std::vector<std::string> vec{};
        size_t currIndex = 0;
        size_t l = s.length();
        size_t lastIndex = 0;
        for (size_t i = 0; i < l;i++) {
            if (s[i] == delimiter) {
                std::string sub = s.substr(lastIndex, currIndex - lastIndex);
                if(sub.length() != 0) {
                    vec.push_back(sub);
                }
                lastIndex = i + 1;
                currIndex = i + 1;
            }
            else {
                currIndex++;
            }
        }
        std::string sub = s.substr(lastIndex, currIndex - lastIndex);
        if (!sub.empty()) {
            vec.push_back(sub);
        }
        return vec;
    }

    //stringToJSON takes in a string and returns a JSON object
    //following the k=v parsing routine, seperated by & and does
    //not make any assumption regarding what the underlying datatypes
    //of the values may be and defaults assumes they are strings
    JSON stringToJSON(std::string s) {
        JSON j{};
        std::vector<std::string> splitByAmbersand = splitString(s, ambersand);
        for (std::string kv: splitByAmbersand) {
            std::vector<std::string> keyValuePair = splitString(kv, equalSign);
            if (keyValuePair.size() < 2) {
                continue;
            }
            j.addString(keyValuePair[0], keyValuePair[1]);
        }
        return j;
    }

    JSON profile_for(std::string email) {
        std::vector<std::string> rm_ambersand = splitString(email, ambersand);
        if (rm_ambersand.empty()) {
            throw -1;
        }
        std::vector<std::string> rm_equal = splitString(rm_ambersand[0], equalSign);
        if (rm_equal.empty()) {
            throw -1;
        }
        int uid = 10;//currenlty just a constant but should be an incrementive thing
        JSON j{};
        j.addInt(UID, uid);
        j.addString(EMAIL, rm_equal[0]);
        j.addString(ROLE, ROLE_USER);
        return j;
    }

    std::string printProfile(JSON j) {
        std::stringstream ss;
        ss << "email=" << j.getString(EMAIL) << ambersand << "uid=" << j.getInt(UID) << ambersand << "role=" << j.getString(ROLE);
        return ss.str();
    }

    size_t get_uid_length(std::string profile) {
        //Split by ambersand
        std::vector<std::string> rm_ambersand = splitString(profile, ambersand);
        std::string uid_equals = rm_ambersand[1];
        std::vector<std::string> vec = splitString(uid_equals, equalSign);
        std::string uid = vec[1];
        return uid.length();
    }

} // namespace parsing
