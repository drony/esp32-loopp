// Copyright (C) 2017 Rob Caelers <rob.caelers@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef LOOPP_CORE_THREADLOCAL_HPP
#define LOOPP_CORE_THREADLOCAL_HPP

#include <unordered_map>

#include "loopp/core/Mutex.hpp"
#include "loopp/core/ScopedLock.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace loopp
{
  namespace core
  {
    template<typename T>
    class ThreadLocal
    {
    public:
      void set(T obj)
      {
        ScopedLock l(mutex);
        TaskHandle_t handle = xTaskGetCurrentTaskHandle();
        objects[handle] = obj;
      }

      T get()
      {
        ScopedLock l(mutex);
        TaskHandle_t handle = xTaskGetCurrentTaskHandle();
        if (objects.find(handle) != objects.end())
          {
            return objects[handle];
          }
        return nullptr;
      }

      void remove()
      {
        ScopedLock l(mutex);
        objects.erase(xTaskGetCurrentTaskHandle());
      }

    private:
      loopp::core::Mutex mutex;
      std::unordered_map<TaskHandle_t, T> objects;
    };
  } // namespace core
} // namespace loopp
#endif // LOOPP_CORE_THREADLOCAL_HPP
