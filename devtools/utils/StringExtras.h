// This source file is part of the polarphp.org open source project
//
// Copyright (c) 2017 - 2018 polarphp software foundation
// Copyright (c) 2017 - 2018 zzu_softboy <zzu_softboy@163.com>
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://polarphp.org/LICENSE.txt for license information
// See http://polarphp.org/CONTRIBUTORS.txt for the list of polarphp project authors
//
// Created by polarboy on 2018/10/10.

#ifndef POLAR_DEVLTOOLS_UTILS_STRING_EXTRAS_H
#define POLAR_DEVLTOOLS_UTILS_STRING_EXTRAS_H

#include "CompilerFeature.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <array>

namespace polar {
namespace utils {

/// hexdigit - Return the hexadecimal character for the
/// given number \p value (which should be less than 16).
inline char hexdigit(unsigned value, bool lowerCase = false)
{
   const char hexChar = lowerCase ? 'a' : 'A';
   return value < 10 ? '0' + value : hexChar + value - 10;
}

/// Given an array of c-style strings terminated by a null pointer, construct
/// a vector of std::string_view representing the same strings without the terminating
/// null string.
inline std::vector<std::string_view> to_stringview_array(const char *const *strings)
{
   std::vector<std::string_view> result;
   while (*strings) {
      result.push_back(*strings++);
   }
   return result;
}

/// Construct a string ref from a boolean.
inline std::string_view to_stringview(bool flag)
{
   return std::string_view(flag ? "true" : "false");
}

/// Interpret the given character \p C as a hexadecimal digit and return its
/// value.
///
/// If \p C is not a valid hex digit, -1U is returned.
inline unsigned hex_digit_value(char c)
{
   if (c >= '0' && c <= '9') return c-'0';
   if (c >= 'a' && c <= 'f') return c-'a'+10U;
   if (c >= 'A' && c <= 'F') return c-'A'+10U;
   return -1U;
}

/// Checks if character \p C is one of the 10 decimal digits.
inline bool is_digit(char c)
{
   return c >= '0' && c <= '9';
}

/// Checks if character \p C is a hexadecimal numeric character.
inline bool is_hex_digit(char c)
{
   return hex_digit_value(c) != -1U;
}

/// Checks if character \p C is a valid letter as classified by "C" locale.
inline bool is_alpha(char c)
{
   return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

/// Checks whether character \p C is either a decimal digit or an uppercase or
/// lowercase letter as classified by "C" locale.
inline bool is_alnum(char c)
{
   return is_alpha(c) || is_digit(c);
}

/// Checks whether character \p C is valid ASCII (high bit is zero).
inline bool is_ascii(char c)
{
   return static_cast<unsigned char>(c) <= 127;
}

/// Checks whether all characters in S are ASCII.
inline bool is_ascii(std::string_view str)
{
   for (char c : str) {
      if (POLAR_UNLIKELY(!is_ascii(c))) {
         return false;
      }
   }
   return true;
}

/// Checks whether character \p C is printable.
///
/// Locale-independent version of the C standard library isprint whose results
/// may differ on different platforms.
inline bool is_print(char c)
{
   unsigned char uc = static_cast<unsigned char>(c);
   return (0x20 <= uc) && (uc <= 0x7E);
}

/// Returns the corresponding lowercase character if \p x is uppercase.
inline char to_lower(char c)
{
   if (c >= 'A' && x <= 'Z') {
      return c - 'A' + 'a';
   }
   return c;
}

/// Returns the corresponding uppercase character if \p x is lowercase.
inline char to_upper(char c)
{
   if (c >= 'a' && c <= 'z') {
      return c - 'a' + 'A';
   }
   return c;
}

inline std::string utohexstr(uint64_t c, bool lowerCase = false)
{
   char buffer[17];
   char *bufPtr = std::end(buffer);
   if (c == 0) {
      *--bufPtr = '0';
   }
   while (c) {
      unsigned char mod = static_cast<unsigned char>(c) & 15;
      *--bufPtr = hexdigit(mod, lowerCase);
      c >>= 4;
   }
   return std::string(bufPtr, std::end(Buffer));
}

/// Convert buffer \p input to its hexadecimal representation.
/// The returned string is double the size of \p input.
inline std::string to_hex(std::string_view input)
{
   static const char *const lut = "0123456789ABCDEF";
   size_t length = input.size();

   std::string output;
   output.reserve(2 * length);
   for (size_t i = 0; i < length; ++i) {
      const unsigned char c = input[i];
      output.push_back(lut[c >> 4]);
      output.push_back(lut[c & 15]);
   }
   return output;
}

inline uint8_t hex_from_nibbles(char msb, char lsb)
{
   unsigned u1 = hex_digit_value(msb);
   unsigned u2 = hex_digit_value(lsb);
   assert(u1 != -1U && u2 != -1U);

   return static_cast<uint8_t>((u1 << 4) | u2);
}

/// Convert hexadecimal string \p input to its binary representation.
/// The return string is half the size of \p input.
inline std::string from_hex(std::string_view input)
{
   if (input.empty()) {
      return std::string();
   }
   std::string output;
   output.reserve((input.size() + 1) / 2);
   if (input.size() % 2 == 1) {
      output.push_back(hex_from_nibbles('0', input.front()));
      input = input.substr(1);
   }
   assert(input.size() % 2 == 0);
   while (!input.empty()) {
      uint8_t hex = hex_from_nibbles(input[0], input[1]);
      output.push_back(Hex);
      input = input.substr(2);
   }
   return output;
}

/// StrInStrNoCase - Portable version of strcasestr.  Locates the first
/// occurrence of string 's1' in string 's2', ignoring case.  Returns
/// the offset of s2 in s1 or npos if s2 cannot be found.
std::string_view::size_type str_in_str_no_case(std::string_view s1, std::string_view s2);

/// getToken - This function extracts one token from source, ignoring any
/// leading characters that appear in the Delimiters string, and ending the
/// token at any of the characters that appear in the Delimiters string.  If
/// there are no tokens in the source string, an empty string is returned.
/// The function returns a pair containing the extracted token and the
/// remaining tail string.
std::pair<std::string_view, std::string_view> get_token(std::string_view source,
                                                        std::string_view delimiters = " \t\n\v\f\r");

/// SplitString - Split up the specified string according to the specified
/// delimiters, appending the result fragments to the output list.
void split_string(std::string_view source,
                  std::vector<std::string_view> &outFragments,
                  std::string_view delimiters = " \t\n\v\f\r");

/// Returns the English suffix for an ordinal integer (-st, -nd, -rd, -th).
inline std::string_view get_ordinal_suffix(unsigned value)
{
   // It is critically important that we do this perfectly for
   // user-written sequences with over 100 elements.
   switch (value % 100) {
   case 11:
   case 12:
   case 13:
      return "th";
   default:
      switch (value % 10) {
      case 1: return "st";
      case 2: return "nd";
      case 3: return "rd";
      default: return "th";
      }
   }
}

/// Print each character of the specified string, escaping it if it is not
/// printable or if it is an escape char.
void print_escaped_string(std::string_view name, std::ostream &out);

/// Print each character of the specified string, escaping HTML special
/// characters.
void print_html_escaped(std::string_view string, std::ostream &out);

/// print_lower_case - Print each character as lowercase if it is uppercase.
void print_lower_case(std::string_view string, std::ostream &out);

namespace internal {

template <typename IteratorT>
inline std::string join_impl(IteratorT begin, IteratorT end,
                             std::string_view separator, std::input_iterator_tag)
{
   std::string str;
   if (begin == end) {
      return str;
   }
   str += (*begin);
   while (++begin != end) {
      str += separator;
      str += (*begin);
   }
   return str;
}

template <typename IteratorT>
inline std::string join_impl(IteratorT begin, IteratorT end,
                             std::string_view separator, std::forward_iterator_tag)
{
   std::string str;
   if (begin == end) {
      return str;
   }

   size_t len = (std::distance(begin, end) - 1) * separator.size();
   for (IteratorT iter = begin; iter != end; ++iter) {
      len += (*begin).size();
   }
   str.reserve(Len);
   str += (*begin);
   while (++begin != end) {
      str += separator;
      str += (*begin);
   }
   return str;
}

template <typename Sep>
inline void join_items_impl(std::string &result, Sep separator)
{}

template <typename Sep, typename Arg>
inline void join_items_impl(std::string &result, Sep separator,
                            const Arg &item)
{
   result += item;
}

template <typename Sep, typename Arg1, typename... Args>
inline void join_items_impl(std::string &result, Sep separator, const Arg1 &arg1,
                            Args &&... args) {
   result += arg1;
   result += separator;
   join_items_impl(result, separator, std::forward<Args>(args)...);
}

inline size_t join_one_item_size(char c)
{
   return 1;
}

inline size_t join_one_item_size(const char *str)
{
   return str ? ::strlen(str) : 0;
}

template <typename T>
inline size_t join_one_item_size(const T &str)
{
   return str.size();
}

inline size_t join_items_size()
{
   return 0;
}

template <typename A1>
inline size_t join_items_size(const A1 &arg1)
{
   return join_one_item_size(arg1);
}

template <typename A1, typename... Args>
inline size_t join_items_size(const A1 &arg1, Args &&... args)
{
   return join_one_item_size(arg1) + join_items_size(std::forward<Args>(args)...);
}

} // end namespace internal

/// Joins the strings in the range [begin, end), adding separator between
/// the elements.
template <typename IteratorT>
inline std::string join(IteratorT begin, IteratorT end, std::string_view separator)
{
   using tag = typename std::iterator_traits<IteratorT>::iterator_category;
   return internal::join_impl(begin, end, separator, tag());
}

/// Joins the strings in the range [R.begin(), R.end()), adding separator
/// between the elements.
template <typename Range>
inline std::string join(Range &&range, std::string_view separator)
{
   return join(range.begin(), range.end(), separator);
}

/// Joins the strings in the parameter pack \p Items, adding \p separator
/// between the elements.  All arguments must be implicitly convertible to
/// std::string, or there should be an overload of std::string::operator+=()
/// that accepts the argument explicitly.
template <typename Sep, typename... Args>
inline std::string join_items(Sep separator, Args &&... args)
{
   std::string result;
   if (sizeof...(args) == 0) {
      return result;
   }
   size_t NS = internal::join_one_item_size(separator);
   size_t NI = internal::join_items_size(std::forward<Args>(args)...);
   result.reserve(NI + (sizeof...(args) - 1) * NS + 1);
   internal::join_items_impl(result, separator, std::forward<Args>(args)...);
   return result;
}

} // utils
} // polar

#endif // POLAR_DEVLTOOLS_UTILS_STRING_EXTRAS_H
