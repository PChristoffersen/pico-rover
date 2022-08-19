#pragma once

#include <vector>
#include <functional>

template<typename... ARGS>
class Callback {
    public:
        using call_type = std::function<void(ARGS...)>;

        void add(call_type cb)    { m_callbacks.push_back(cb); }
        void remove(call_type cb) { m_callbacks.remove(m_callbacks.begin(), m_callbacks.end(), cb); }

        void operator ()(ARGS ... args) const
        {
            for (auto cb : m_callbacks) {
                cb(std::forward<ARGS>(args)...);
            }
        }
    private:
        using callback_list = std::vector<call_type>;
        callback_list m_callbacks;
};

