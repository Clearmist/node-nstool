#include <napi.h>
#include <string>
#include "json.hpp"

extern nlohmann::json output;

std::string start(const std::vector<std::string>& args, Napi::Env Env);
