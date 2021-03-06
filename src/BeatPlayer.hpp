/// BeatPlayer
///
/// Plays a beat

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>


using namespace std;


namespace mnome {

using TBeatDataType = int16_t;


/// Plays a beat at a certain number of times per minute
class BeatPlayer
{
   size_t beatRate;
   mutex setterMutex;
   vector<TBeatDataType> beat;
   vector<TBeatDataType> playBackBuffer;
   std::unique_ptr<thread> myThread;
   atomic_bool requestStop;

public:
   /// Constructor
   /// \param  beatRate  rate of the beat in [bpm]
   /// \param  beat      raw data of one beat
   explicit BeatPlayer(size_t beatRate);

   ~BeatPlayer();

   /// Set the sound of the beat that is played back
   void setBeat(const vector<TBeatDataType>& newBeat);

   /// Start the BeatPlayer
   void start();

   /// Stop the beat playback
   /// \note Does not block
   void stop();

   /// Change the beat per minute
   /// @param  bpm  beats per minute
   void setBPM(size_t bpm);

   size_t getBPM() const;

   /// Change the beat that is played back
   /// @param  beatData  The beat that is played back
   void setData(const vector<TBeatDataType>& beatData);

   /// Change the beat and the beats per minute, for convenience
   /// @param  beatData  The beat that is played back
   /// @param  bpm  beats per minute
   void setDataAndBPM(const vector<TBeatDataType>& beatData, size_t bpm);

   /// Indicates whether the thread is running
   bool isRunning() const;

   /// Wait for the REPL thread to finish
   void waitForStop() const;
};


/// Generate a sine signal
/// \param[out]  data     samples to be generated
/// \param[in]   freq     tone frequency
/// \param[in]   lengthS  length in seconds
void generateInt16Sine(vector<int16_t>& data, size_t freq, double lengthS);

}  // namespace mnome
