#include "callback.hh"

#include <functional>
#include <iostream>
#include <map>

bool Callback::ignore(argument /* x */) {
    return true;
}

bool Callback::ignore_infinite(argument /* x */) {
    return false;
}

bool Callback::debug(argument x) {
    std::cout << "Argument: " << x << std::endl;
    return true;
}

bool Callback::debug_infinite(argument x) {
    std::cout << "Argument: " << x << std::endl;
    return false;
}

bool CallbackHandler::register_callback(token id, AsyncResult c, Callback::t cb)
{
    std::pair<token, AsyncResult> key(id, c);
    bool filled = (map.find(key) != map.end());
    map[key] = cb;
    return filled;
}

void CallbackHandler::call(token id, AsyncResult c, Callback::argument x)
{
    std::pair<token, AsyncResult> key(id, c);
    auto it = map.find(key);
    if (it == map.end()) {
        std::cerr << "CALLBACK KEY NOT FOUND " << id << " - " << static_cast<int>(c) << std::endl;
        return;
    }
    if (map[key](x)) {
        // true result means remove it
        map.erase(it);
    }
}
