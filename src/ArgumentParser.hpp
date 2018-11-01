#pragma once

#include <optional>
#include <string>
#include <vector>

namespace mnome {


template<typename F>
struct signature : signature<decltype(&F::operator())>{
};

template<typename F>
struct signature<F*> : signature<F>{
};


template <typename C, typename R, typename... P>
struct signature<R (C::*)(P...)> : signature<R(P...)>
{
};

template<typename C, typename R, typename... P>
struct signature<R(C::*)(P...) const>: signature<R(P...)>{
};

template<typename R, typename... P>
struct signature<R(P...)>{
    using result_t=R;
    using param_t=std::tuple<P...>;
    using param_values_t=std::tuple<std::decay_t<P>...>;
};


template<typename T>
struct ArgumentParser;

class Arguments
{
   std::vector<std::string> m_args;

public:
   Arguments(std::vector<std::string> args) : m_args{args} {}
   Arguments(const Arguments&) = default;
   Arguments& operator=(const Arguments&) = default;

   std::string pop()
   {
      if (!m_args.empty()) {
         auto result = m_args[0];
         m_args.erase(m_args.begin());
         return result;
      }
      else {
         return "";
      }
   }

   bool empty() { return m_args.empty(); }

   std::size_t size() { return m_args.size(); }
};


template <typename T>
using ParserResult = std::optional<std::pair<T, Arguments>>;


template <>
struct ArgumentParser<std::string>
{
   static ParserResult<std::string> parse(Arguments args)
   {
      if (!args.empty()) {
         auto string = args.pop();
         return ParserResult<std::string>{std::in_place_t(), string, args};
      }
      return std::nullopt;
   }

   static std::string helpSignature() { return "STRING"; }
};

template <typename T, typename... Ts>
struct ArgumentParser<std::tuple<T, Ts...>>
{
   static ParserResult<std::tuple<T, Ts...>> parse(Arguments args)
   {
      auto headResult = ArgumentParser<T>::parse(args);
      if (headResult) {
         auto [headValue, tailArgs] = *headResult;

         auto tailResult = ArgumentParser<std::tuple<Ts...>>::parse(tailArgs);
         if (tailResult) {
            auto [tailValue, restArgs] = *tailResult;

            auto result = std::tuple_cat(std::make_tuple(headValue), tailValue);
            return ParserResult<std::tuple<T, Ts...>>{std::in_place_t(), result, restArgs};
         }
      }

      return std::nullopt;
   }

   static std::string helpSignature()
   {
      std::string result = ArgumentParser<T>::helpSignature();
      std::vector<std::string> signatures = {ArgumentParser<Ts>::helpSignature()...};
      for (auto s : signatures) {
         result += " " + s;
      }
      return "[" + result + "]";
   }
};
template <>
struct ArgumentParser<std::tuple<>>
{
   static ParserResult<std::tuple<>> parse(Arguments args)
   {
      return ParserResult<std::tuple<>>{std::in_place_t(), std::make_tuple(), args};
   }

   static std::string helpSignature() { return ""; }
};

template <typename T>
struct ArgumentParser<std::vector<T>>
{
   static ParserResult<std::vector<T>> parse(Arguments args)
   {
      std::vector<T> result;
      while (args.size()) {
         auto elementResult = ArgumentParser<T>::parse(args);
         if (!elementResult) {
            break;
         }
         result.push_back(elementResult->first);
         args = elementResult->second;
      }
      return ParserResult<std::vector<T>>{std::in_place_t(), result, args};
   }
   static std::string helpSignature() { return ArgumentParser<T>::helpSignature() + "..."; }
};

// todo implement support for the following types optional<T>, int, bool, variant<Ts...>


}  // namespace mnome
