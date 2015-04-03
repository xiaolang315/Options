#include "program_options/detail/Cmdline.hpp"
#include "program_options/Parsers.hpp"

#include "../include/program_options/VariablesMap.hpp"
#include "program_options/PositionalOptions.hpp"

namespace options {

	VariablesMap  parse_args(int argc, const char* const argv[],
                       const OptionsDescription& desc)
    {
    	VariablesMap vm;
    	store(Basic_command_line_parser(argc, argv).options(desc).run(), vm);
        return vm;
    }

}
