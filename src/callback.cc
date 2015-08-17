#include "callback.hh"

#include <functional>
#include <iostream>
#include <map>

void Callback::ignore(argument /* x */) {
    // nop
}

void Callback::debug(argument x) {
    std::cout << "Argument: " << x << std::endl;
}

bool CallbackHandler::register_callback(token id, AsyncResult c, Callback::t cb, CallbackType kind)
{
    key k(id, c);
    value v(cb, kind);
    bool filled = (map.find(k) != map.end());
    map[k] = v;
    return filled;
}

void CallbackHandler::call(token id, AsyncResult c, Callback::argument x)
{
    key k(id, c);
    auto it = map.find(k);
    if (it == map.end()) {
        std::cerr << "CALLBACK KEY NOT FOUND " << id << " - " << static_cast<int>(c) << std::endl;
        return;
    }
    std::cout << "CALLING CALLBACK id=" << id << std::endl;
    value v(map[k]);
    v.first(x);
    if (v.second == CallbackType::Single) {
        // One-shot so remove it
        map.erase(it);
    }
}
