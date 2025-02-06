//
// Created by shahar on 24/01/2025.
//

#ifndef DS1_WET2_PLAINSIMPL_H
#define DS1_WET2_PLAINSIMPL_H

#include "wet2util.h"
#include "Team.h"
#include "Jockey.h"

struct BannedIdNode {
    int id;
    BannedIdNode* next;
};

class PlainsImpl{
private:
    //Hash-table sizes
    static const int TEAM_HASH_SIZE = 40001;
    static const int JOCKEY_HASH_SIZE = 40001;
    static const int RECORD_HASH_SIZE = 40001;

    //Bucket arrays
    Team* teamsHash[TEAM_HASH_SIZE];
    Jockey* jockeysHash[JOCKEY_HASH_SIZE];
    Team* recordHash[RECORD_HASH_SIZE];

    static const int BANNED_HASH_SIZE = 40001;
    BannedIdNode* bannedHash[BANNED_HASH_SIZE];

    //---------Helpers----------
    //Hash function to handle negative keys
    static int positiveMod(int key, int size);

    //1) Union-Find helpers
    static Team* ufFind(Team* t);

    //2) Hash helpers for teams/jockeys
    Team* findTeamById(int teamId);
    bool insertTeam(Team* t);
    Jockey* findJockeyById(int jockeyId);
    bool insertJockey(Jockey* j);

    //3) Record-hash insertion and removal
    void recordHashInsert(Team* t);
    void recordHashRemove(Team* t, int oldRecord);

    //4) merge logic used by merge_teams and unite_by_record
    Team* doTeamMerge(Team* root1, Team* root2);

    // Helper for unite_by_record: check if exactly one root has 'record'
    Team* checkExactlyOneTeamWithRecord(int record);

    void bannedIdHashInsert(int lostId);
    bool bannedIdHashContains(int checkId);

public:
    PlainsImpl();
    ~PlainsImpl();

    //-------------public methods------------
    StatusType add_team(int teamId);
    StatusType add_jockey(int jockeyId, int teamId);
    StatusType update_match(int victoriousJockeyId, int losingJockeyId);
    StatusType merge_teams(int teamId1, int teamId2);
    StatusType unite_by_record(int record);

    output_t<int> get_jockey_record(int jockeyId);
    output_t<int> get_team_record(int teamId);
};

#endif //DS1_WET2_PLAINSIMPL_H
