#include <iostream>
#include <random>
#include "utils/Random.h"

namespace utils
{
    std::string GenerateShortCode(int length)
    {
        const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::random_device rd; // obtain a random seed from hardware
        std::mt19937 generator(rd()); // generate random numbers
        std::uniform_int_distribution<> distribution(0, characters.size() - 1); // every number has equal probability of appearing

        std::string shortcode;
        for (int i = 0; i < length; ++i)
        {
            shortcode += characters[distribution(generator)];
        }
        return shortcode;
    }

}