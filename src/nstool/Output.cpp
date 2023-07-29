#include <json.hpp>

using json = nlohmann::json;

json output = {
    {"error", false},
    {"errorMessage", ""},
    {"warnings", json::array()},
    {"log", json::array()},
};
