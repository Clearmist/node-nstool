#if defined(NODE_ADDON_API)
#include <napi.h>
#endif
#include <optional>
#include <json.hpp>

// The object that will be stringified into JSON.
extern nlohmann::json output;

// True if the output format should be JSON.
extern bool outputJSON;

// True if this program has been compiled as a Node addon module.
extern bool nodeAddon;

/**
 * This function will throw a JavaScript error if the program is compiled as a Node addon module.
 * Otherwise it will print the error message to the command line.
*/
void handleError(std::string message);

void handleLog(std::string message, const std::optional<std::string>& level = std::nullopt);

void handlePrint(std::string message);

#if defined(NODE_ADDON_API)
// Napi::Env does not have a default constructor so initialize it with a null pointer.
extern Napi::Env Environment;
#endif
