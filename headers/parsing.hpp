#ifndef PARSING_HPP
#define PARSING_HPP
#include <map>
#include <vector>
#include <string>
namespace parsing
{   
    //JSON is a homemade implementation of a JSON. 
    //It can't handle arbitrary datatypes yes.
    class JSON
    {
    private:
        /* data */
        std::map<std::string, int> stringToInt;
        std::map<std::string, std::string> stringToString;
        std::map<std::string, double> stringToDouble;
    public:
        JSON(/* args */);
        ~JSON();
        int getInt(std::string key);
        void addInt(std::string key, int v);
        std::string getString(std::string key);
        void addString(std::string key, std::string v);
        double getDouble(std::string key);
        void addDouble(std::string key, double v);
        //getAny(std::string key); TODO

    };
    std::vector<std::string> splitString(std::string s, char delimiter);

    JSON stringToJSON(std::string s);
} // namespace parsing

#endif 