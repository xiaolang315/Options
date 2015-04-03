#ifndef BOOST_CMDLINE_VP_2003_05_19
#define BOOST_CMDLINE_VP_2003_05_19

#include <string>
#include <vector>

#include "../Errors.hpp"
#include "../Option.hpp"
#include "../OptionsDescription.hpp"
#include "../PositionalOptions.hpp"


namespace options { namespace detail {

    /** Command line parser class. Main requirements were:
        - Powerful enough to support all common uses.
        - Simple and easy to learn/use.
        - Minimal code size and external dependencies.
        - Extensible for custom syntaxes.

        First all options are registered. After that, elements of command line
        are extracted using operator++. 

        For each element, user can find
        - if it's an option or an argument
        - name of the option
        - index of the option
        - option value(s), if any
        
        Sometimes the registered option name is not equal to the encountered
        one, for example, because name abbreviation is supported.  Therefore
        two option names can be obtained: 
        - the registered one 
        - the one found at the command line

        There are lot of style options, which can be used to tune the command
        line parsing. In addition, it's possible to install additional parser
        which will process custom option styles.

        @todo mininal match length for guessing?
    */
    struct Cmdline
	{

        /** Constructs a command line parser for (argc, argv) pair. Uses
            style options passed in 'style', which should be binary or'ed values
            of style_t enum. It can also be zero, in which case a "default"
            style will be used. If 'allow_unregistered' is true, then allows 
            unregistered options. They will be assigned index 1 and are
            assumed to have optional parameter.
        */
        Cmdline(const std::vector<std::string>& args);

        /** @overload */
        Cmdline(int argc, const char** argv);

        //int get_canonical_option_prefix();

        void allow_unregistered();

        void set_options_description(const OptionsDescription& desc);

        std::vector<Option> run();

        std::vector<Option> parse_long_option(const string& arg);
        std::vector<Option> parse_short_option(const string& args);

        /** Set additional parser. This will be called for each token
            of command line. If first string in pair is not empty,
            then the token is considered matched by this parser,
            and the first string will be considered an option name
            (which can be long or short), while the second will be
            option's parameter (if not empty). 
            Note that additional parser can match only one token.
        */

        void init(const std::vector<std::string>& args);

	private:
        // Copies of input.
        std::vector<std::string> args;
        bool m_allow_unregistered;

        const OptionsDescription* m_desc;
    };
    
    void test_cmdline_detail();
    
}}


#endif

