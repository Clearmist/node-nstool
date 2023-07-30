#if defined(NODE_ADDON_API)
#include <napi.h>
#include <string>

std::string start(const std::vector<std::string>& args, Napi::Env Env);
#endif
