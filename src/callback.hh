#pragma once

#include <functional>
#include <map>
#include <string>

/// This call holds types about callbacks
/// Callback::t is the callback type for (string) -> bool
/// Result value is whether event should be removed (done handling)
class Callback {
public:
    using argument = std::string;
    using t = std::function<bool (argument)>;
    static bool ignore(argument x);
    static bool ignore_infinite(argument x);
    static bool debug(argument x);
    static bool debug_infinite(argument x);
};

/// What can happen to an async call
enum class AsyncResult { Success, Failure, Timeout };

/// Handle callbacks, associate tokens with callback actions
class CallbackHandler
{
public:
    using token = int;

    /// Register a callback of the given type with a token
    bool register_callback(token id, AsyncResult c, Callback::t cb);

    /// Call a previously registered callback for the id and type
    /// remove means unregister it after we do the call
    void call(token id, AsyncResult c, Callback::argument x);
private:
    std::map<std::pair<token, AsyncResult>, Callback::t> map;
};
