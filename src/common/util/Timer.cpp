// Copyright (c) 2014-2020 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "Timer.h"

#include "util/Logger.hpp"

int Timer::_expired(void* ctx)
{
    Timer *timer = (Timer *)ctx;
    timer->clear();
    timer->m_isExpired = true;
    return G_SOURCE_REMOVE;
}

Timer::Timer()
    : m_isWaiting(false),
      m_isExpired(false),
      m_timerId(-1),
      m_timeout(-1)
{
    clear();
}

Timer::~Timer()
{
    clear();
}

bool Timer::wait(int ms)
{
    if (m_isWaiting)
        return false;
    if (m_isExpired)
        return false;

    m_isWaiting = true;
    m_timeout = ms;
    m_timerId = g_timeout_add(ms, &Timer::_expired, this);
    m_isExpired = false;

    while (m_isWaiting) {
        if (TRUE != g_main_context_iteration(g_main_context_default(), true)) {
            Logger::warning(MSGID_CONFIGDSERVICE, LOG_PREPIX_FORMAT "Error in g_main_context_iteration", LOG_PREPIX_ARGS);
        }
    }
    return m_isExpired;
}

bool Timer::isWaiting()
{
    return m_isWaiting;
}

bool Timer::isExpired()
{
    return m_isExpired;
}

void Timer::clear()
{
    if (m_isWaiting) {
        m_isWaiting = false;
        if (TRUE != g_source_remove(m_timerId)) {
            Logger::warning(MSGID_CONFIGDSERVICE, LOG_PREPIX_FORMAT "Error in g_source_remove", LOG_PREPIX_ARGS);
        }
    }
    m_isExpired = false;
    m_timerId = -1;
    m_timeout = -1;
}
