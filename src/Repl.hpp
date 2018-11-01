/// Repl: Read evaluate print loop
///
/// Recognizes given commands and executes the according functionality

#ifndef MNOME_REPL_H
#define MNOME_REPL_H

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <sstream>

#include "ArgumentParser.hpp"
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace std;


namespace mnome {

using commandlist_t = unordered_map<string, function<void(string&)>>;

using paramlist_t = std::vector<std::string>;


/// Read evaluate print loop
class Repl
{
   commandlist_t commands;
   std::unordered_map<std::string, std::string> commandHelp;

   std::unique_ptr<thread> myThread;
   atomic_bool requestStop;

public:
   /// Ctor with empty command list
   Repl();

   /// Ctor with command list
   Repl(commandlist_t& cmds);

   ~Repl();

   template<typename F>
   void addCommand(std::string name, F function){
      using Parser = ArgumentParser<typename signature<F>::param_t>;
      commands[name] =
         [function](const string& params) {
            std::istringstream iss(params);
            paramlist_t paramList{std::istream_iterator<std::string>(iss),
                                             std::istream_iterator<std::string>()};

            Arguments args{paramList};

            auto parsedParams = Parser::parse(args);
            if (!parsedParams) {
               std::cerr << "error during parameter parsing" << std::endl;
            }
            else {
               if (!parsedParams->second.empty()) {
                  std::cerr << "error during parameter parsing" << std::endl;
               }
               else {
                  std::apply(function, parsedParams->first);
               }
            }
         };
      commandHelp[name] = Parser::helpSignature();
   }

   /// Set the commands that the REPL should recognize
   /// \note Do not forget the exit/quit command
   void setCommands(const commandlist_t& cmds);

   /// Start the read evaluate print loop
   void start();

   /// Stops the read evaluate print loop
   /// \note Does not block
   void stop();

   /// Indicates whether the thread is running
   bool isRunning() const;

   /// Make sure the thread has stopped
   /// \note blocks until thread is finished
   void waitForStop() const;

private:
   /// The method that the thread runs
   void run();
};


}  // namespace mnome


#endif  //  MNOME_REPL_H
