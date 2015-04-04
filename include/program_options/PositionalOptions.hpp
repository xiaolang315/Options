#ifndef POSITIONALOPTIONS_H
#define POSITIONALOPTIONS_H

#include <vector>
#include <string>

namespace options {

    struct  PositionalOptionsDescription
	{
        PositionalOptionsDescription();

        PositionalOptionsDescription&
        add(const char* name, int max_count);

        unsigned max_total_count() const;

        const std::string& name_for_position(unsigned position) const;

    private:
        std::vector<std::string> m_names;
        std::string m_trailing;
    };

}

#endif

