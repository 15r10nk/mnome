/// Mnome - A metronome program

#include "BeatPlayer.hpp"
#include "Repl.hpp"

#include <memory>
#include <mutex>

#include <csignal>


using namespace std;


namespace mnome {

/// The metronome object
class Mnome
{
   BeatPlayer bp;
   Repl repl;

public:
   /// Ctor
   Mnome() : bp{80}
   {
      // generate the beat
      vector<TBeatDataType> beatData;
      generateInt16Sine(beatData, 500, 0.050);
      vector<TBeatDataType> accentuatedBeat;
      generateInt16Sine(accentuatedBeat, 750, 0.050);

      bp.setBeat(beatData);
      bp.setAccentuatedBeat(accentuatedBeat);
      bp.setAccentuatedPattern(vector<bool>{true, false, false, false});

      // set commands for the repl
      repl.addCommand("exit", [this]() { this->repl.stop(); });
      repl.addCommand("start", [this]() { this->bp.start(); });
      repl.addCommand("stop", [this]() {
         this->bp.stop();
         this->bp.waitForStop();
      });

      repl.addCommand("bpm", [this](string s) {
         size_t bpm      = !s.empty() ? stoul(s, nullptr, 10) : 80;
         bool wasRunning = bp.isRunning();
         if (wasRunning) {
            this->bp.stop();
            this->bp.waitForStop();
         }
         this->bp.setBPM(bpm);
         if (wasRunning) {
            this->bp.start();
         }
      });

      repl.addCommand("pattern", [this](string s) {
         if ((s.find('*') != string::npos) || (s.find('+') != string::npos)) {
            vector<bool> pattern;
            for (char c : s) {
               if (c == '*') {
                  pattern.push_back(true);
               }
               else if (c == '+') {
                  pattern.push_back(false);
               }
            }
            bool wasRunning = bp.isRunning();
            if (wasRunning) {
               this->bp.stop();
               this->bp.waitForStop();
            }
            this->bp.setAccentuatedPattern(pattern);
            if (wasRunning) {
               this->bp.start();
            }
         }
      });


      repl.addCommand("test", [](string a, string b) {
         std::cout << "a " << a;
         std::cout << "b " << b;
      });

      repl.addCommand("test2", [](std::vector<std::tuple<string, string>> strings) {
         int i = 0;
         for (auto [a, b] : strings) {
            cout << i++ << ": " << a << " & " << b << endl;
         }
      });

      repl.start();
   }

   void stop()
   {
      bp.stop();
      repl.stop();
   }

   /// Wait for the read evaluate loop to finish
   void waitForStop() const
   {
      repl.waitForStop();
   }
};

}  // namespace mnome


mutex AppMutex;
unique_ptr<mnome::Mnome> App;


void shutDownAppHandler(int)
{
   lock_guard<mutex> lg{AppMutex};
   if (App) {
      App->stop();
   }
}

int main()
{
   signal(SIGINT, shutDownAppHandler);
   signal(SIGTERM, shutDownAppHandler);
   signal(SIGABRT, shutDownAppHandler);

   {
      lock_guard<mutex> lg{AppMutex};
      App = make_unique<mnome::Mnome>();
   }

   // no mutex needed, member function is const
   App->waitForStop();

   return 0;
}
