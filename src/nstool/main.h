#include <napi.h>
#include <string>
#include "json.hpp"

extern nlohmann::json output;

extern Napi::Env Environment;

std::string start(const std::vector<std::string>& args, Napi::Env Env);
