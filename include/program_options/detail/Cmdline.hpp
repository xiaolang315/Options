#ifndef CMDLINE_H
#define CMDLINE_H

#include <string>
#include <vector>

#include "../Option.hpp"
#include "../OptionsDescription.hpp"
#include "../PositionalOptions.hpp"


namespace options { namespace detail {

    struct Cmdline
	{

        Cmdline(const std::vector<std::string>& args);

        Cmdline(int argc, const char** argv);

        void allow_unregistered();

        void set_options_description(const OptionsDescription& desc);

        std::vector<Option> run();

        std::vector<Option> parse_long_option(const string& arg);
        std::vector<Option> parse_short_option(const string& args);

        void init(const std::vector<std::string>& args);

	private:
        std::vector<std::string> args;
        bool m_allow_unregistered;

        const OptionsDescription* m_desc;
    };
    
    void test_cmdline_detail();
    
}}


#endif

