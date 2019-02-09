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

#include <string.h>
#include "loopp/core/Trigger.hpp"

#include "loopp/core/ScopedLock.hpp"

#include <system_error>

#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "esp_log.h"

using namespace loopp;
using namespace loopp::core;

Trigger::Trigger()
  : pipe_read(-1)
  , pipe_write(-1)
  , count(0)
{
  init_pipe();
}

Trigger::~Trigger()
{
  if (pipe_read >= 0)
    {
      close(pipe_read);
    }
  if (pipe_write >= 0)
    {
      close(pipe_write);
    }
}

void
Trigger::init_pipe()
{
  pipe_read = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (pipe_read < 0)
    {
      throw std::runtime_error("socket");
    }

  pipe_write = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (pipe_write < 0)
    {
      throw std::runtime_error("socket");
    }

  struct sockaddr_in addr { };
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(0);
  addr.sin_addr.s_addr = htonl(0x7f000001);
  socklen_t addr_len = sizeof(addr);

  int rc = bind(pipe_read, reinterpret_cast<struct sockaddr *>(&addr), addr_len);
  if (rc < 0)
    {
      throw std::runtime_error("bind");
    }
  rc = getsockname(pipe_read, reinterpret_cast<struct sockaddr *>(&addr), &addr_len);
  if (rc < 0)
    {
      throw std::runtime_error("getsockname");
    }
  addr.sin_addr.s_addr = htonl(0x7f000001);
  rc = connect(pipe_write, reinterpret_cast<struct sockaddr *>(&addr), addr_len);
  if (rc < 0)
    {
      throw std::runtime_error("connect");
    }
  rc = getsockname(pipe_write, reinterpret_cast<struct sockaddr *>(&addr), &addr_len);
  if (rc < 0)
    {
      throw std::runtime_error("getsockname");
    }
  addr.sin_addr.s_addr = htonl(0x7f000001);
  rc = connect(pipe_read, reinterpret_cast<struct sockaddr *>(&addr), addr_len);
  if (rc < 0)
    {
      throw std::runtime_error("connect");
    }
  int flags = fcntl(pipe_read, F_GETFL, 0);
  fcntl(pipe_read, F_SETFL, flags | O_NONBLOCK);
  flags = fcntl(pipe_write, F_GETFL, 0);
  fcntl(pipe_write, F_SETFL, flags | O_NONBLOCK);
}

void
Trigger::signal()
{
  loopp::core::ScopedLock l(mutex);
  count++;
  if (count == 1)
    {
      uint8_t dummy = 0;
      int written = write(pipe_write, &dummy, 1);
      assert(written == 1 && "Failed to trigger");
    }
}

int
Trigger::confirm()
{
  loopp::core::ScopedLock l(mutex);
  int ret = count;
  if (count > 0)
    {
      uint8_t dummy;
      int written = read(pipe_read, &dummy, 1);
      assert(written == 1 && "Failed to confirm trigger");
    }
  count = 0;
  return ret;
}

int
Trigger::get_poll_fd() const
{
  return pipe_read;
}
