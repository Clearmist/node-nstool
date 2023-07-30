#include <chrono>
#include <optional>
#include <regex>
#if defined(NODE_ADDON_API)
#include <napi.h>
#endif
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <json.hpp>
#include "date.h"

using json = nlohmann::json;

json output = {
    {"error", false},
    {"errorMessage", ""},
    {"log", json::array()},
};

bool outputJSON = false;

#if defined(NODE_ADDON_API)
// Napi::Env does not have a default constructor so initialize it with a null pointer.
Napi::Env Environment = nullptr;

bool nodeAddon = true;

void handleError(std::string message) {
    Napi::TypeError::New(Environment, message).ThrowAsJavaScriptException();
}

void handleLog(std::string message, const std::optional<std::string>& level = std::nullopt) {
    // Set the default log level.
    std::string logLevel = level ? level.value() : "info";

    // Remove trailing new lines.
    std::regex pattern("\n+");

    // Get a date time.
    auto now = std::chrono::system_clock::now();
    std::string datetime = date::format("%FT%TZ", date::floor<std::chrono::milliseconds>(now));

    json messageObject = {
        {"level", logLevel},
        {"message", std::regex_replace(message, pattern, "")},
        {"datetime", datetime}
    };

    output["log"].push_back(messageObject);
}
#else
bool nodeAddon = false;

void handleError(std::string message) {
    fmt::print(message);
}

void handleLog(std::string message, const std::optional<std::string>& level = std::nullopt) {
    fmt::print(message);
}
#endif
