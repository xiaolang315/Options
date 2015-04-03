// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PARSERS_VP_2003_05_19
#define BOOST_PARSERS_VP_2003_05_19

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


    /** Results of parsing an input source. 
        The primary use of this class is passing information from parsers 
        component to value storage component. This class does not makes
        much sense itself.        
    */
    struct ParsedOptions
	{
        explicit ParsedOptions(const OptionsDescription* xdescription, int options_prefix = 0)
        : description(xdescription), m_options_prefix(options_prefix) {}
        /** Options found in the source. */
        std::vector< Basic_option > options;
        /** Options description that was used for parsing. 
            Parsers should return pointer to the instance of 
            option_description passed to them, and issues of lifetime are
            up to the caller. Can be NULL.
         */
        const OptionsDescription* description;

        /** Mainly used for the diagnostic messages in exceptions.
         *  The canonical option prefix  for the parser which generated these results,
         *  depending on the settings for basic_command_line_parser::style() or
         *  cmdline::style(). In order of precedence of command_line_style enums:
         *      allow_long
         *      allow_long_disguise
         *      allow_dash_for_short
         *      allow_slash_for_short
        */ 
        int m_options_prefix;
    };

    /** Augments basic_parsed_options<wchar_t> with conversion from
        'parsed_options' */


    //    typedef function1<std::pair<std::string, std::string>, const std::string&> ext_parser;

    /** Command line parser.

        The class allows one to specify all the information needed for parsing
        and to parse the command line. It is primarily needed to
        emulate named function parameters -- a regular function with 5
        parameters will be hard to use and creating overloads with a smaller
        nuber of parameters will be confusing.

        For the most common case, the function parse_command_line is a better 
        alternative.        

        There are two typedefs -- command_line_parser and wcommand_line_parser,
        for charT == char and charT == wchar_t cases.
    */
    struct Basic_command_line_parser : private detail::Cmdline{

        /** Creates a command line parser for the specified arguments
            list. The 'args' parameter should not include program name.
        */
        Basic_command_line_parser(const std::vector<
                                  std::basic_string<char> >& args);
        /** Creates a command line parser for the specified arguments
            list. The parameters should be the same as passed to 'main'.
        */
        Basic_command_line_parser(int argc, const char* const argv[]);

        /** Sets options descriptions to use. */
        Basic_command_line_parser& options(const OptionsDescription& desc);
        /** Sets positional options description to use. */
        Basic_command_line_parser& positional(
            const PositionalOptionsDescription& desc);

        /** Sets the command line style. */
        Basic_command_line_parser& style(int);

        /** Parses the options and returns the result of parsing.
            Throws on error.
        */
        ParsedOptions run();

        /** Specifies that unregistered options are allowed and should
            be passed though. For each command like token that looks
            like an option but does not contain a recognized name, an
            instance of basic_option<charT> will be added to result,
            with 'unrecognized' field set to 'true'. It's possible to
            collect all unrecognized options with the 'collect_unrecognized'
            funciton. 
        */
        Basic_command_line_parser& allow_unregistered();

    private:
        const OptionsDescription* m_desc;
    };

    typedef Basic_command_line_parser command_line_parser;


    /** Collects the original tokens for all named options with
        'unregistered' flag set. If 'mode' is 'include_positional'
        also collects all positional options.
        Returns the vector of origianl tokens for all collected
        options.
    */
    template<class charT>
    std::vector< std::basic_string<char> >
    collect_unrecognized(const std::vector< Basic_option>& options,
                         enum collect_unrecognized_mode mode);

    /** Parse environment.

        Takes all environment variables which start with 'prefix'. The option
        name is obtained from variable name by removing the prefix and 
        converting the remaining string into lower case.
    */
//     parsed_options
//    parse_environment(const Options_description&, const std::string& prefix);

    /** @overload
        This function exists to resolve ambiguity between the two above 
        functions when second argument is of 'char*' type. There's implicit
        conversion to both function1 and string.
    */
//     parsed_options
//    parse_environment(const Options_description&, const char* prefix);

    /** Splits a given string to a collection of single strings which
        can be passed to command_line_parser. The second parameter is
        used to specify a collection of possible seperator chars used
        for splitting. The seperator is defaulted to space " ".
        Splitting is done in a unix style way, with respect to quotes '"'
        and escape characters '\'
    */
     std::vector<std::string>
    split_unix(const std::string& cmdline, const std::string& seperator = " \t", 
         const std::string& quote = "'\"", const std::string& escape = "\\");

     VariablesMap
     parse_args(int argc, const char* const argv[],
                        const OptionsDescription& desc);
}
#endif
