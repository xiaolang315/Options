#include "program_options/PositionalOptions.hpp"

#include <cassert>
#include <limits>

namespace options {

    PositionalOptionsDescription::PositionalOptionsDescription()
    {}

    PositionalOptionsDescription&
    PositionalOptionsDescription::add(const char* name, int max_count)
    {
        assert(max_count != -1 || m_trailing.empty());

        if (max_count == -1)
            m_trailing = name;
        else {
            m_names.resize(m_names.size() + max_count, name);
        }
        return *this;
    }

    unsigned
    PositionalOptionsDescription::max_total_count() const
    {
        return m_trailing.empty() ? 
          static_cast<unsigned>(m_names.size()) : (std::numeric_limits<unsigned>::max)();
    }
    
    const std::string& 
    PositionalOptionsDescription::name_for_position(unsigned position) const
    {
        assert(position < max_total_count());

        if (position < m_names.size())
            return m_names[position];
        else
            return m_trailing;
    }


}

