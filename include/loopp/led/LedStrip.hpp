// Copyright (C) 2018, 2019 Rob Caelers <rob.caelers@gmail.com>
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

#ifndef LOOPP_LED_LEDSTRIP_HPP
#define LOOPP_LED_LEDSTRIP_HPP

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <experimental/type_traits>

#include "esp_log.h"

#include "boost/format.hpp"
#include "boost/preprocessor/repetition/repeat.hpp"

#include "loopp/core/Task.hpp"
#include "loopp/core/HighResTimer.hpp"
#include "loopp/led/Color.hpp"

// TODO: Move generic parts to non-template base class
// TODO: Support multiple led strips per thread
// TODO: Support multiple led strips per current limitter/power supply

namespace loopp
{
  namespace led
  {
    namespace detail
    {
      template<typename T>
      using apply_hook_t = decltype(std::declval<T &>().apply_hook());

      template<typename T>
      constexpr bool has_apply_hook = std::experimental::is_detected_v<apply_hook_t, T>;
    } // namespace detail

    using LedColorsType = std::vector<loopp::led::Color>;

    class LedStripBase
    {
    public:
      LedStripBase(uint16_t number_of_leds)
        : number_of_leds(number_of_leds)
        , task("ledstrip", std::bind(&LedStripBase::led_task, this), loopp::core::Task::CoreId::NoAffinity, 2048, configMAX_PRIORITIES - 7)
        , timer("ledstrip", std::bind(&LedStripBase::on_timer, this), std::chrono::microseconds(1000000 / 50), loopp::core::HighResTimer::Type::Periodic)
      {
        led_colors.reserve(number_of_leds);
      }

      ~LedStripBase() = default;

      void set_color(uint16_t idx, const loopp::led::Color &color)
      {
        if (idx >= number_of_leds)
          {
            throw std::runtime_error((boost::format("Led %1% out of range") % idx).str());
          }

        led_colors[idx] = color;
      }

      void clear()
      {
        led_colors.assign(number_of_leds, Color());
      }

      void set_brighness_scale(double scale)
      {
        this->scale = scale;
      }

      void on_timer() {}

      void led_task()
      {
        while (true)
          {
          }
      }

    protected:
      uint16_t number_of_leds = 0;
      loopp::core::Task task;
      loopp::core::HighResTimer timer;
      LedColorsType led_colors;
      double scale = 1.0;
    };

    template<template<typename> typename... Mixin>
    class LedStrip
      : public LedStripBase
      , public Mixin<LedStrip<Mixin...>>...
    {
#define FRIEND(z, n, text)                                                                                                                 \
  friend std::tuple_element_t<std::min((std::size_t)n, sizeof...(Mixin)), std::tuple<void, Mixin<LedStrip<Mixin...>>...>>;
      BOOST_PP_REPEAT(100, FRIEND, 1)
#undef FRIEND

    public:
      template<typename... U>
      LedStrip(uint16_t number_of_leds, U &&... v)
        : LedStripBase(number_of_leds)
        , Mixin<LedStrip<Mixin...>>(std::forward<U>(v))...
      {
      }

      ~LedStrip() = default;

      void apply()
      {
        (invoke_apply_hook<Mixin<LedStrip<Mixin...>>>(), ...);
        this->execute(led_colors, scale);
      }

    private:
      template<typename T>
      void invoke_apply_hook()
      {
        if constexpr (detail::has_apply_hook<T>)
          {
            T::apply_hook();
          }
      }
    };

  } // namespace led
} // namespace loopp

#endif // LOOPP_LED_LEDSTRIP_HPP
