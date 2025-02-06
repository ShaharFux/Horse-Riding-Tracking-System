//
// Created by shahar on 24/01/2025.
//

#include "Team.h"

Team::Team(int id) :
    teamId(id),
    record(0),
    active(true),
    parent(this),
    nextTeamInHash(nullptr),
    nextTeamInRecord(nullptr)
{}