#ifndef PARSERS_H
#define PARSERS_H

#include <program_options/detail/Cmdline.hpp>
#include "program_options/OptionsDescription.hpp"

#include <iosfwd>
#include <vector>
#include <utility>
#include "Option.hpp"
#include "OptionEnum.h"

namespace options {

    struct OptionsDescription;
    struct PositionalOptionsDescription;
    struct VariablesMap;

    struct ParsedOptions
	{
        explicit ParsedOptions(const OptionsDescription* xdescription, int options_prefix = 0)
        : description(xdescription), m_options_prefix(options_prefix) {}

        std::vector< Basic_option > options;

        const OptionsDescription* description;

        int m_options_prefix;
    };

    struct Basic_command_line_parser : private detail::Cmdline{

        Basic_command_line_parser(const std::vector<
                                  std::basic_string<char> >& args);
        Basic_command_line_parser(int argc, const char* const argv[]);

        Basic_command_line_parser& options(const OptionsDescription& desc);
        Basic_command_line_parser& positional(
            const PositionalOptionsDescription& desc);

        Basic_command_line_parser& style(int);

        ParsedOptions run();

        Basic_command_line_parser& allow_unregistered();

    private:
        const OptionsDescription* m_desc;
    };

    typedef Basic_command_line_parser command_line_parser;

    template<class charT>
    std::vector< std::basic_string<char> >
    collect_unrecognized(const std::vector< Basic_option>& options,
                         enum collect_unrecognized_mode mode);


    std::vector<std::string>
    split_unix(const std::string& cmdline, const std::string& seperator = " \t", 
         const std::string& quote = "'\"", const std::string& escape = "\\");

    VariablesMap
    parse_args(int argc, const char* const argv[],
                        const OptionsDescription& desc);
}
#endif
