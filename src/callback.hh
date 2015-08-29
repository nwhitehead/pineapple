// Copyright 2015 Nathan Whitehead
// Distributed under the terms of the GNU General Public License

// This file is part of Pineapple.
//
// Pineapple is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Pineapple is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Pineapple.  If not, see <http://www.gnu.org/licenses/>.

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
