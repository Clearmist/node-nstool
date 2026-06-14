#include <iostream>
#include <napi.h>
#include <sstream>
#include <string>
#include <vector>
#define FMT_HEADER_ONLY
#include <fmt/core.h>

int umain(const std::vector<std::string> &args, const std::vector<std::string> &env);

std::vector<std::string> BuildParameters(const Napi::CallbackInfo &info)
{
    std::vector<std::string> parameters = {};

    for (size_t i = 0; i < info.Length(); ++i)
    {
        parameters.push_back(info[i].ToString().Utf8Value());
    }

    return parameters;
}

std::string start(const std::vector<std::string> &args, Napi::Env env)
{
    const std::vector<std::string> runtimeEnvironment = {"prod"};
    std::ostringstream output;
    auto *originalBuffer = std::cout.rdbuf(output.rdbuf());

    int result = 0;

    try
    {
        result = umain(args, runtimeEnvironment);
    }
    catch (const std::exception &error)
    {
        std::cout.rdbuf(originalBuffer);
        Napi::Error::New(env, error.what()).ThrowAsJavaScriptException();
        return "";
    }

    std::cout.rdbuf(originalBuffer);

    if (result != 0)
    {
        const auto message = output.str();
        Napi::Error::New(env, message.empty() ? "nstool exited with a non-zero status." : message)
            .ThrowAsJavaScriptException();
        return "";
    }

    return output.str();
}

Napi::String Run(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), start(BuildParameters(info), info.Env()));
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    exports.Set("run", Napi::Function::New(env, Run));

    return exports;
}

NODE_API_MODULE(addon, InitAll);
