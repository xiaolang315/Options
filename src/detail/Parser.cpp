#include <program_options/detail/Cmdline.hpp>
#include "program_options/OptionsDescription.hpp"
#include "program_options/Parsers.hpp"
#include <iterator>
#include "program_options/OptionEnum.h"

namespace options {

    namespace detail {
        template<class charT, class Iterator>
        std::vector<std::basic_string<charT> >
        make_vector(Iterator i, Iterator e)
        {
            std::vector<std::basic_string<charT> > result;
            for(; i != e; ++i)
                result.push_back(*i);
            return result;
        }
    }

    Basic_command_line_parser::
    Basic_command_line_parser(const std::vector<std::basic_string<char>>& xargs)
       : detail::Cmdline(xargs)
    {}

    Basic_command_line_parser::
	Basic_command_line_parser(int argc, const char* const argv[])
        : detail::Cmdline(
                      detail::make_vector<char, const char* const*>(argv+1, argv+argc+!argc))
	{

	}

	Basic_command_line_parser&
	Basic_command_line_parser::options(const OptionsDescription& desc)
	{
		detail::Cmdline::set_options_description(desc);
		m_desc = &desc;
		return *this;
	}

    Basic_command_line_parser&
    Basic_command_line_parser::allow_unregistered()
    {
        detail::Cmdline::allow_unregistered();
        return *this;
    }

    ParsedOptions
    Basic_command_line_parser::run()
    {
        ParsedOptions result(m_desc, 0);
        result.options = detail::Cmdline::run();

        return ParsedOptions(result);
    }

    std::vector< std::basic_string<char> >
    collect_unrecognized(const std::vector< Basic_option>& options,
                         enum collect_unrecognized_mode mode)
    {
        std::vector< std::basic_string<char> >  result;
        for(unsigned i = 0; i < options.size(); ++i)
        {
            if (options[i].unregistered ||
                (mode == include_positional && options[i].position_key != -1))
            {
                copy(options[i].original_tokens.begin(),
                     options[i].original_tokens.end(),
                     back_inserter(result));
            }
        }
        return result;
    }
}

