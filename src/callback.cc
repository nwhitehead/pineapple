#include "callback.hh"

#include <functional>
#include <iostream>
#include <map>

#include <wx/log.h>

void Callback::ignore(argument /* x */) {
    // nop
}

void Callback::debug(argument x) {
    wxLogDebug("Callback::debug Argument is [%s]", x);
}

CallbackHandler::token CallbackHandler::gid = 0;

CallbackHandler::token CallbackHandler::fresh_id()
{
    return gid++; // need to protect this if we have multiple threads
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
        wxLogDebug("CallbackHandler::call Callback key not found: id = %d  c = %d", id, static_cast<int>(c));
        return;
    }
    value v(map[k]);
    v.first(x);
    if (v.second == CallbackType::Single) {
        // One-shot so remove it
        map.erase(it);
    }
}
