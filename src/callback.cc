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

#include "callback.hh"

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

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
    std::lock_guard<std::mutex> lock(m_mutex);
    key k(id, c);
    value v(cb, kind);
    bool filled = (map.find(k) != map.end());
    map[k] = v;
    return filled;
}

void CallbackHandler::call(token id, AsyncResult c, Callback::argument x)
{
    // Make sure to lock table while modifying
    key k(id, c);
    m_mutex.lock();
    auto it = map.find(k);
    if (it == map.end()) {
        wxLogDebug("CallbackHandler::call Callback key not found: id = %d  c = %d", id, static_cast<int>(c));
        m_mutex.unlock();
        return;
    }
    value v(map[k]);
    if (v.second == CallbackType::Single) {
        // One-shot so remove it
        map.erase(it);
    }
    m_mutex.unlock();
    // Unlock all mutexes before calling callback, never know what will happen
    v.first(x);
}

// Not fully baked yet
/*
void CallbackHandler::call_delayed(token id, AsyncResult c, Callback::argument x, int milliseconds)
{
    std::thread t([this, id, c, x, milliseconds]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        call(id, c, x);
    });
    t.detach();
    // Return asynchronously
}
*/
