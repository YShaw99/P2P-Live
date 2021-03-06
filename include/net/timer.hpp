/**
* \file timer.hpp
* \author kadds (itmyxyf@gmail.com)
* \brief Timer queue generated by minimum heap
* \version 0.1
* \date 2020-03-13
*
* @copyright Copyright (c) 2020.
This file is part of P2P-Live.

P2P-Live is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

P2P-Live is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P2P-Live. If not, see <http: //www.gnu.org/licenses/>.
*
*/
#pragma once
#include "net.hpp"
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <queue>
#include <vector>

namespace net
{
using microsecond_t = u64;
using timer_callback_t = std::function<void()>;
// 1ms
inline constexpr microsecond_t timer_min_precision = 1000;
using timer_id = int64_t;

struct timer_t
{
    microsecond_t timepoint;
    timer_callback_t callback;
    timer_t(microsecond_t timepoint, std::function<void()> callback)
        : timepoint(timepoint)
        , callback(callback)
    {
    }
};

struct timer_slot_t
{
    microsecond_t timepoint;
    std::vector<std::pair<timer_callback_t, bool>> callbacks;
    timer_slot_t(microsecond_t tp)
        : timepoint(tp)
    {
    }
};

using map_t = std::unordered_map<microsecond_t, timer_slot_t *>;

timer_t make_timer(microsecond_t span, timer_callback_t callback);

struct timer_cmp
{
    bool operator()(timer_slot_t *lh, timer_slot_t *rh) const { return lh->timepoint > rh->timepoint; }
};

struct timer_registered_t
{
    timer_id id;
    microsecond_t timepoint;
};

/// No thread safety. Don't add timers from other threads
struct time_manager_t
{
    microsecond_t precision;
    /// a minimum heap for timers
    std::priority_queue<timer_slot_t *, std::vector<timer_slot_t *>, timer_cmp> queue;
    map_t map;

    time_manager_t() {}

    void tick();
    /// add new timer
    timer_registered_t insert(timer_t timer);

    /// remove timer
    void cancel(timer_registered_t reg);

    /// get the time should be called at next tick 'timepoint'
    microsecond_t next_tick_timepoint();
};

std::unique_ptr<time_manager_t> create_time_manager(microsecond_t precision = timer_min_precision);

microsecond_t get_current_time();

/// Inner use
microsecond_t get_timestamp();

constexpr microsecond_t make_timespan(int second, int ms = 0, int us = 0)
{
    return (u64)second * 1000000 + (u64)ms * 1000 + us;
}

constexpr microsecond_t make_timespan_full() { return 0xFFFFFFFFFFFFFFFFULL; }

} // namespace net