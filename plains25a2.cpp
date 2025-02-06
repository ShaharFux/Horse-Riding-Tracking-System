// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a2.h"
#include "PlainsImpl.h"

#include <new> // for std::bad_alloc


Plains::Plains()
{
    // Attempt to allocate the internal implementation
    try {
        m_impl = new PlainsImpl();
    } catch (const std::bad_alloc&) {
        m_impl = nullptr;
    }
}

Plains::~Plains()
{
    if (m_impl) {
        delete m_impl;  // calls ~PlainsImpl()
        m_impl = nullptr;
    }
}

StatusType Plains::add_team(int teamId)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->add_team(teamId);
}

StatusType Plains::add_jockey(int jockeyId, int teamId)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->add_jockey(jockeyId, teamId);
}

StatusType Plains::update_match(int victoriousJockeyId, int losingJockeyId)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->update_match(victoriousJockeyId, losingJockeyId);
}

StatusType Plains::merge_teams(int teamId1, int teamId2)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->merge_teams(teamId1, teamId2);
}

StatusType Plains::unite_by_record(int record)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->unite_by_record(record);
}

output_t<int> Plains::get_jockey_record(int jockeyId)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->get_jockey_record(jockeyId);
}

output_t<int> Plains::get_team_record(int teamId)
{
    if (!m_impl) return StatusType::ALLOCATION_ERROR;
    return m_impl->get_team_record(teamId);
}
