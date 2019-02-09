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

#ifndef LOOPP_NET_NETWORKERRORS_HPP
#define LOOPP_NET_NETWORKERRORS_HPP

#include <system_error>

namespace loopp
{
  namespace net
  {
    enum class NetworkErrc
    {
      // no 0
      Timeout = 1,
      Cancelled,
      InternalError,
      NameResolutionFailed,
      InvalidAddress,
      TLSProtocolError,
      ConnectionRefused,
      ConnectionClosed,
      ReadError,
      WriteError,
    };

    std::error_code make_error_code(NetworkErrc);
  } // namespace net
} // namespace loopp

namespace std
{
  template<>
  struct is_error_code_enum<loopp::net::NetworkErrc> : true_type
  {
  };
} // namespace std

#endif // LOOPP_NET_NETWORKERRORS_HPP
