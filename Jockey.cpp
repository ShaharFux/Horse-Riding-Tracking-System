//
// Created by shahar on 24/01/2025.
//

#include "Jockey.h"

Jockey::Jockey(int id, Team *t) :
    jockeyId(id),
    record(0),
    teamPtr(t),
    nextJockeyInHash(nullptr)
{}
