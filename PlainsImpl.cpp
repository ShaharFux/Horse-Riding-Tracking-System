//
// Created by shahar on 24/01/2025.
//

#include "PlainsImpl.h"
#include <new>
#include <exception>

PlainsImpl::PlainsImpl() {
    for (int i = 0; i < TEAM_HASH_SIZE; i++) teamsHash[i] = nullptr;
    for (int i = 0; i < JOCKEY_HASH_SIZE; i++) jockeysHash[i] = nullptr;
    for (int i = 0; i < RECORD_HASH_SIZE; i++) recordHash[i] = nullptr;
    for (int i = 0; i < BANNED_HASH_SIZE; i++) bannedHash[i] = nullptr;
}

PlainsImpl::~PlainsImpl() {
    // 1) Delete all Team objects from teamsHash
    for (int i = 0; i < TEAM_HASH_SIZE; i++) {
        while(teamsHash[i]) {
            Team* tmp = teamsHash[i];
            teamsHash[i] = tmp->nextTeamInHash;
            delete tmp; // safe, since each Team is only in one chain
        }
    }
    // 2) Delete all Jockey objects from jockeysHash
    for (int i = 0; i < JOCKEY_HASH_SIZE; i++){
        while (jockeysHash[i]){
            Jockey* tmp = jockeysHash[i];
            jockeysHash[i] = tmp->nextJockeyInHash;
            delete tmp;
        }
    }
    // 3) recordHash pointers are not separately allocated—no extra deletion needed.

    for (int i = 0; i < BANNED_HASH_SIZE; i++){
        while (bannedHash[i]){
            BannedIdNode* tmp = bannedHash[i];
            bannedHash[i] = tmp->next;
            delete tmp;
        }
    }
}

int PlainsImpl::positiveMod(int key, int size) {
    auto k = (long long)key;
    if (k < 0) k = -k;
    return (int)(k % size);
}

Team* PlainsImpl::ufFind(Team *t) {
    if (!t) return nullptr;
    if (t->parent != t) {
        t->parent = ufFind(t->parent);
    }
    return t->parent;
}

void PlainsImpl::bannedIdHashInsert(int lostId) {
    int bucket = positiveMod(lostId, BANNED_HASH_SIZE);
    BannedIdNode* node = new (std::nothrow) BannedIdNode;
    if (!node) {
        return;
    }
    node->id = lostId;
    node->next = bannedHash[bucket];

    bannedHash[bucket] = node;
}

bool PlainsImpl::bannedIdHashContains(int checkId) {
    int bucket = positiveMod(checkId, BANNED_HASH_SIZE);
    BannedIdNode* curr = bannedHash[bucket];
    while (curr) {
        if (curr->id == checkId) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

Team* PlainsImpl::findTeamById(int teamId) {
    int bucket = positiveMod(teamId, TEAM_HASH_SIZE);
    Team* curr = teamsHash[bucket];
    while (curr){
        if (curr->teamId == teamId && curr->active) {
            return curr;
        }
        curr = curr->nextTeamInHash;
    }
    return nullptr;
}

bool PlainsImpl::insertTeam(Team *t) {
    if (!t) return false;
    int bucket = positiveMod(t->teamId, TEAM_HASH_SIZE);
    t->nextTeamInHash = teamsHash[bucket];
    teamsHash[bucket] = t;
    return true;
}

Jockey* PlainsImpl::findJockeyById(int jockeyId)
{
    int bucket = positiveMod(jockeyId, JOCKEY_HASH_SIZE);
    Jockey* curr = jockeysHash[bucket];
    while (curr){
        if (curr->jockeyId == jockeyId) {
            return curr;
        }
        curr = curr->nextJockeyInHash;
    }
    return nullptr;
}

bool PlainsImpl::insertJockey(Jockey* j)
{
    if (!j) return false;
    int bucket = positiveMod(j->jockeyId, JOCKEY_HASH_SIZE);
    j->nextJockeyInHash = jockeysHash[bucket];
    jockeysHash[bucket] = j;
    return true;
}

void PlainsImpl::recordHashInsert(Team* t)
{
    if (!t) return;
    int bucket = positiveMod(t->record, RECORD_HASH_SIZE);
    t->nextTeamInRecord = recordHash[bucket];
    recordHash[bucket] = t;
}

void PlainsImpl::recordHashRemove(Team* t, int oldRecord)
{
    if (!t) return;
    int bucket = positiveMod(oldRecord, RECORD_HASH_SIZE);

    Team* prev = nullptr;
    Team* curr = recordHash[bucket];
    while (curr) {
        if (curr == t) {
            // remove from chain
            if (prev) {
                prev->nextTeamInRecord = curr->nextTeamInRecord;
            } else {
                recordHash[bucket] = curr->nextTeamInRecord;
            }
            Team* toDelete = curr;
            curr = curr->nextTeamInRecord;
            toDelete->nextTeamInRecord = nullptr;
        } else {
            prev = curr;
            curr = curr->nextTeamInRecord;
        }
    }
}

Team* PlainsImpl::doTeamMerge(Team* winner, Team* loser)
{
    if (!winner || !loser) return nullptr;

    // Remove both from recordHash
    recordHashRemove(winner, winner->record);
    recordHashRemove(loser, loser->record);

    int combined = winner->record + loser->record;

    loser->parent = winner;

    // Mark loser as inactive
    loser->active = false;

    //Ban the loser's old teamId, so it can't be used again
    bannedIdHashInsert(loser->teamId);

    // winner gets new record
    winner->record = combined;

    // Insert back with updated record
    recordHashInsert(winner);

    return winner;
}

Team* PlainsImpl::checkExactlyOneTeamWithRecord(int record)
{
    int bucket = positiveMod(record, RECORD_HASH_SIZE);
    Team* curr = recordHash[bucket];
    Team* foundRoot = nullptr;
    int distinctCount = 0;

    while (curr){
        Team* r = ufFind(curr);
        if (r && r->active && r->record == record && (r == ufFind(r))) {
            // Found an active root with this record
            if (!foundRoot) {
                //first time
                foundRoot = r;
                distinctCount = 1;
            } else if (r != foundRoot) {
                //second distinct root =>fail
                return nullptr;
            }
        }
        curr = curr->nextTeamInRecord;
    }
    if (distinctCount == 1) return foundRoot;
    return nullptr;
}

//----------Public methods---------
StatusType PlainsImpl::add_team(int teamId) {
    if (teamId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    //If it's banned, we must fail
    if (bannedIdHashContains(teamId)) {
        return StatusType::FAILURE;
    }

    //Check if exists
    Team* existing = findTeamById(teamId);
    if (existing) {
        //Already present
        return StatusType::FAILURE;
    }
    //Create new
    Team* newT = nullptr;
    try {
        newT = new Team(teamId);
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    if (!insertTeam(newT)) { //Shouldn't happen
        delete newT;
        return StatusType::ALLOCATION_ERROR;
    }

    //Insert into record-hash
    recordHashInsert(newT);

    return StatusType::SUCCESS;
}

StatusType PlainsImpl::add_jockey(int jockeyId, int teamId)
{
    if (jockeyId <= 0 || teamId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    // Check if jockey exists
    if (findJockeyById(jockeyId)) {
        return StatusType::FAILURE;
    }
    // Find the team
    Team* t = findTeamById(teamId);
    if (!t) {
        return StatusType::FAILURE;
    }
    // Create new jockey
    Jockey* newJ = nullptr;
    try {
        newJ = new Jockey(jockeyId, t);
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    if (!insertJockey(newJ)) { //Shouldn't happen
        delete newJ;
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType PlainsImpl::update_match(int victoriousJockeyId, int losingJockeyId)
{
    // checks
    if (victoriousJockeyId <= 0 || losingJockeyId <= 0
        || victoriousJockeyId == losingJockeyId) {
        return StatusType::INVALID_INPUT;
    }
    Jockey* winner = findJockeyById(victoriousJockeyId);
    Jockey* loser  = findJockeyById(losingJockeyId);
    if (!winner || !loser) {
        return StatusType::FAILURE;
    }

    Team* rootW = ufFind(winner->teamPtr);
    Team* rootL = ufFind(loser->teamPtr);
    if (!rootW || !rootL || rootW == rootL) {
        return StatusType::FAILURE; // same team or missing
    }

    if (!rootW->active || !rootL->active) {
        return StatusType::FAILURE;
    }

    // Update jockey records
    winner->record++;
    loser->record--;

    // Update teams’ records
    recordHashRemove(rootW, rootW->record);
    recordHashRemove(rootL, rootL->record);

    rootW->record++;
    rootL->record--;

    recordHashInsert(rootW);
    recordHashInsert(rootL);

    return StatusType::SUCCESS;
}

StatusType PlainsImpl::merge_teams(int teamId1, int teamId2)
{
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) {
        return StatusType::INVALID_INPUT;
    }
    Team* t1 = findTeamById(teamId1);
    Team* t2 = findTeamById(teamId2);
    if (!t1 || !t2) {
        return StatusType::FAILURE;
    }
    Team* root1 = ufFind(t1);
    Team* root2 = ufFind(t2);
    if (root1 == root2) {
        return StatusType::FAILURE; // same set
    }

    //Compare their records
    int rec1 = root1->record;
    int rec2 = root2->record;

    Team* winner;
    Team* loser;

    if (rec1 > rec2) {
        winner = root1;
        loser = root2;
    } else if (rec1 < rec2) {
        winner = root2;
        loser = root1;
    } else {
        winner = root1;
        loser = root2;
        winner->teamId = teamId1;
    }

    doTeamMerge(winner, loser);
    return StatusType::SUCCESS;
}

StatusType PlainsImpl::unite_by_record(int record)
{
    if (record <= 0) {
        return StatusType::INVALID_INPUT;
    }
    Team* posTeam = checkExactlyOneTeamWithRecord(record);
    Team* negTeam = checkExactlyOneTeamWithRecord(-record);
    if (!posTeam || !negTeam || posTeam == negTeam) {
        return StatusType::FAILURE;
    }
    doTeamMerge(posTeam, negTeam);
    return StatusType::SUCCESS;
}

output_t<int> PlainsImpl::get_jockey_record(int jockeyId)
{
    if (jockeyId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    Jockey* j = findJockeyById(jockeyId);
    if (!j) {
        return StatusType::FAILURE;
    }
    return  j->record;
}

output_t<int> PlainsImpl::get_team_record(int teamId)
{
    if (teamId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    Team* t = findTeamById(teamId);
    if (!t) {
        return StatusType::FAILURE;
    }
    Team* root = ufFind(t);
    if (!root || !root->active) {
        return StatusType::FAILURE;
    }
    return root->record;
}




