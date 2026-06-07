#include "Random.h"

//Since static members belong to the class itself, they need storage.
//Random engine storage 
std::mt19937 Random::s_RandomEngine;

//Distribution storage
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;