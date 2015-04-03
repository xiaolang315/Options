// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_OPTION_HPP_VP_2004_02_25
#define BOOST_OPTION_HPP_VP_2004_02_25

#include <string>
#include <vector>
#include <iostream>

namespace options {

    /** Option found in input source.
        Contains a key and a value. The key, in turn, can be a string (name of
        an option), or an integer (position in input source) -- in case no name
        is specified. The latter is only possible for command line.
        The template parameter specifies the type of char used for storing the
        option's value.
    */
    struct Basic_option 
    {
        Basic_option() 
            : position_key(-1)
            , unregistered(false) 
            , case_insensitive(false)
        {}

        Basic_option(const std::string& xstring_key, 
               const std::vector< std::string> &xvalue)
            : string_key(xstring_key)
            , value(xvalue)
            , unregistered(false)
            , case_insensitive(false)
        {}

        /** String key of this option. Intentionally independent of the template
            parameter. */
        std::string string_key;
        /** Position key of this option. All options without an explicit name are
            sequentially numbered starting from 0. If an option has explicit name,
            'position_key' is equal to -1. It is possible that both
            position_key and string_key is specified, in case name is implicitly
            added.
         */
        int position_key;
        /** Option's value */
        std::vector< std::string > value;
        /** The original unchanged tokens this option was
            created from. */
        std::vector< std::string> original_tokens;
        /** True if option was not recognized. In that case,
            'string_key' and 'value' are results of purely
            syntactic parsing of source. The original tokens can be
            recovered from the "original_tokens" member.
        */
        bool unregistered;
        /** True if string_key has to be handled
            case insensitive.
        */
        bool case_insensitive;
        bool hasValue;

        void dump() const
        {
        	std::cout<<"option dump start string key = "<<string_key<<std::endl;
        	std::cout<<"option value dump start"<<std::endl;
        	for(auto var : value)
        	{
        		std::cout<<var<<std::endl;
        	}
        	std::cout<<"option tokens dump start"<<std::endl;
        	for(auto var : original_tokens)
        	{
        		std::cout<<var<<std::endl;
        	}
        }
    };

    typedef Basic_option Option;

}

#endif
