//
// Created by shahar on 24/01/2025.
//

#ifndef DS1_WET2_TEAM_H
#define DS1_WET2_TEAM_H

struct Team {
    int teamId;
    int record;
    bool active;

    // Union-Find fields
    Team* parent;

    // Next pointers for chaining in hash
    Team* nextTeamInHash;
    Team* nextTeamInRecord;

    // c'tor
    Team(int id);

    // d'tor
    ~Team() = default;
};

#endif //DS1_WET2_TEAM_H
