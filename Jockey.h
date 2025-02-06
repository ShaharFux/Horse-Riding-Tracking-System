//
// Created by shaha on 24/01/2025.
//

#ifndef DS1_WET2_JOCKEY_H
#define DS1_WET2_JOCKEY_H

#include "Team.h"

struct Jockey {
    int jockeyId;
    int record;
    Team* teamPtr;

    Jockey* nextJockeyInHash; // Chaining in the jockey-hash

    // c'tor
    Jockey(int id, Team* t);

    // d'tor
    ~Jockey() = default;
};

#endif //DS1_WET2_JOCKEY_H
