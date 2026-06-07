#pragma once 
#include <random>

class Random {
  public:
    static void Init()
    {
      s_RandomEngine.seed(std::random_device()()); //generates a random seed from the operating system and initializes the Mersenne Twister engine with that seed.
    }

    static float Float() // returns 0.0 <= value <= 1.0
    {
      return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
      //Generate a random integer divide by maximum value possible to normalize and get a float between 0 to 1
    }
  private:
    static std::mt19937 s_RandomEngine; //A very popular pseudo-random number generator called Mersenne Twister.
	  static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution; //A distribution decides how numbers are produced.
};
