#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>
#include <iostream>

namespace options {

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

        std::string string_key;
        int position_key;
        std::vector< std::string > value;
        std::vector< std::string> original_tokens;
        bool unregistered;
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
