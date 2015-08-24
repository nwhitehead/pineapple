#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>

/// This call holds types about callbacks
/// Callback::t is the callback type for (string) -> bool
/// Result value is whether event should be removed (done handling)
class Callback {
public:
    using argument = std::string;
    using t = std::function<void (argument)>;
    static void ignore(argument x);
    static void debug(argument x);
};

/// What can happen to an async call
enum class AsyncResult { Success, Failure, Timeout };
enum class CallbackType { Single, Infinite };

/// Handle callbacks, associate tokens with callback actions
class CallbackHandler
{
public:
    using token = int;

    /// Register a callback of the given type with a token (one shot)
    bool register_callback(token id, AsyncResult c, Callback::t cb, CallbackType kind=CallbackType::Single);

    /// Call a previously registered callback for the id and type
    void call(token id, AsyncResult c, Callback::argument x);

    /// Call a previously registered callback for the id and type
    //void call_delayed(token id, AsyncResult c, Callback::argument x, int milliseconds);
    
    /// Get a globally fresh id
    static token fresh_id();
private:
    using key = std::pair<token, AsyncResult>;
    using value = std::pair<Callback::t, CallbackType>;
    std::map<key, value> map;
    std::mutex m_mutex;
    static token gid;
};
