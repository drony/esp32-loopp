// Copyright (C) 2015 Rob Caelers <rob.caelers@gmail.com>
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

#ifndef LOOPP_HTTP_HEADERS_HPP
#define LOOPP_HTTP_HEADERS_HPP

#include <string>
#include <map>

namespace loopp
{
  namespace http
  {
    namespace detail
    {
      struct case_insensitive_compare : std::binary_function<std::string, std::string, bool>
      {
        struct nocase_compare : public std::binary_function<unsigned char, unsigned char, bool>
        {
          bool operator()(const unsigned char &c1, const unsigned char &c2) const
          {
            return tolower(c1) < tolower(c2);
          }
        };
        bool operator()(const std::string &s1, const std::string &s2) const
        {
          return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), nocase_compare());
        }
      };
    } // namespace detail

    class Headers
    {
    public:
      typedef std::map<std::string, std::string, detail::case_insensitive_compare> HeadersMap;

      Headers() = default;
      Headers(const Headers &other) = default;
      Headers(Headers &&other) = default;
      Headers &operator=(const Headers &other) = default;
      Headers &operator=(Headers &&other) = default;

      bool has(const std::string &name) const;
      void set(const std::string &name, const std::string &value);
      void emplace(const std::string &name, const std::string &value);
      void remove(const std::string &name);
      void clear();
      std::string &operator[](const std::string &name);
      const std::string &operator[](const std::string &name) const;

      friend std::ostream &operator<<(std::ostream &stream, const Headers &headers);

      HeadersMap::iterator begin();
      HeadersMap::const_iterator begin() const;
      HeadersMap::iterator end();
      HeadersMap::const_iterator end() const;

      void parse(std::istream &stream);

    private:
      HeadersMap headers;
    };
  } // namespace http
} // namespace loopp

#endif // LOOPP_HTTP_HEADERS_HPP
