#include <program_options/detail/Cmdline.hpp>
#include "program_options/OptionsDescription.hpp"
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <cstring>
#include <cctype>
#include <climits>

#include <cstdio>

#include <iostream>

#include "../include/program_options/Errors.hpp"
#include "../include/program_options/PositionalOptions.hpp"
#include "../include/program_options/ValueSemantic.hpp"

namespace options {

    using namespace std;
    
    string 
    Invalid_syntax::get_template(kind_t kind)
    {
        // Initially, store the message in 'const char*' variable,
        // to avoid conversion to string in all cases.
        const char* msg;
        switch(kind)
        {
        case empty_adjacent_parameter:
            msg = "the argument for option '%canonical_option%' should follow immediately after the equal sign";
            break;
        case missing_parameter:
            msg = "the required argument for option '%canonical_option%' is missing";
            break;
        case unrecognized_line:
            msg = "the options configuration file contains an invalid line '%invalid_line%'";
            break;
        // none of the following are currently used:
        case long_not_allowed:
            msg = "the unabbreviated option '%canonical_option%' is not valid";
            break;
        case long_adjacent_not_allowed:
            msg = "the unabbreviated option '%canonical_option%' does not take any arguments";
            break;
        case short_adjacent_not_allowed:
            msg = "the abbreviated option '%canonical_option%' does not take any arguments";
            break;
        case extra_parameter:
            msg = "option '%canonical_option%' does not take any arguments";
            break;
        default:
            msg = "unknown command line syntax error for '%s'";
        }
        return msg;
    }


}


namespace options { namespace detail {

    Cmdline::Cmdline(const vector<string>& args)
    {
        init(args);
    }

    Cmdline::Cmdline(int argc, const char** argv)
    {
        init(vector<string>(argv+1, argv+argc+!argc));
    }

    void
    Cmdline::init(const vector<string>& args)
    {
        this->args = args;        
        m_desc = 0;
        m_allow_unregistered = false;
    }
    
    void 
    Cmdline::allow_unregistered()
    {
        this->m_allow_unregistered = true;
    }

    void 
    Cmdline::set_options_description(const OptionsDescription& desc)
    {
        m_desc = &desc;
    }
    typedef std::vector<Option> (options::detail::Cmdline::* style_parser)(const std::string&);

      

    vector<Option>
    Cmdline::run()
    {
        // The parsing is done by having a set of 'style parsers'
        // and trying then in order. Each parser is passed a vector
        // of unparsed tokens and can consume some of them (by
        // removing elements on front) and return a vector of options.
        //
        // We try each style parser in turn, untill some input
        // is consumed. The returned vector of option may contain the
        // result of just syntactic parsing of token, say --foo will
        // be parsed as option with name 'foo', and the style parser
        // is not required to care if that option is defined, and how
        // many tokens the value may take.
        // So, after vector is returned, we validate them.
        assert(m_desc);

         style_parser style_parsers[] = {&Cmdline::parse_long_option, &Cmdline::parse_short_option};//{&Cmdline::parse_long_option, &Cmdline::parse_disguised_long_option, &Cmdline::parse_short_option};

        vector<Option> result;
        while(!args.empty())
        {
            bool unkonwn = false;
            for(auto parser : style_parsers)
            {
                unsigned current_size = static_cast<unsigned>(args.size());
                vector<Option> next = (this->*parser)(args[0]);

                if(!next.empty())
                {
                	for(auto var : next)
                	{
                		var.dump();
                		result.push_back(var);
                	}
                	args.erase(args.begin());
                }
                if (args.size() < current_size) {
                    unkonwn = true;
                    break;                
                }
            }
            
            if (!unkonwn) {
                Option opt;
                opt.value.push_back(args[0]);
                opt.original_tokens.push_back(args[0]);
                result.push_back(opt);
                args.erase(args.begin());
            }
        }
//        for(auto var : result)
//        {
//        	var.dump();
//        }
        /* If an key option is followed by a positional option,
           can can consume more tokens (e.g. it's multitoken option),
           give those tokens to it.  */
        vector<Option> result2;
        for (unsigned i = 0; i < result.size(); ++i)
        {
            result2.push_back(result[i]);
            Option& opt = result2.back();

            if (opt.string_key.empty())
            {
            	continue;
            }

            const OptionDescription* xd = m_desc->find_nothrow(opt.string_key,
                                            true,//is_style_active(allow_guessing),
                                            true, //is_style_active(long_case_insensitive),
                                            true);//is_style_active(short_case_insensitive));

            if (!xd)
            {
            	opt.unregistered = true;
            	result2.pop_back();
                continue;
            }
            unsigned min_tokens = xd->semantic()->min_tokens();
            unsigned max_tokens = xd->semantic()->max_tokens();
            if (min_tokens < max_tokens && opt.value.size() < max_tokens)
            {
                // This option may grab some more tokens.
                // We only allow to grab tokens that are not already
                // recognized as key options.

                int can_take_more = max_tokens - static_cast<int>(opt.value.size());
                unsigned j = i+1;
                for (; can_take_more && j < result.size(); --can_take_more, ++j)
                {
                    Option& opt2 = result[j];
                    if (!opt2.string_key.empty())
                        break;

                    if (opt2.position_key == INT_MAX)
                    {
                        // We use INT_MAX to mark positional options that
                        // were found after the '--' terminator and therefore
                        // should stay positional forever.
                        break;
                    }

                    assert(opt2.value.size() == 1);
                    
                    opt.value.push_back(opt2.value[0]);

                    assert(opt2.original_tokens.size() == 1);

                    opt.original_tokens.push_back(opt2.original_tokens[0]);
                }
                i = j-1;
            }
        }
        result.swap(result2);
        

        // Assign position keys to positional options.
        int position_key = 0;
        for(unsigned i = 0; i < result.size(); ++i) {
            if (result[i].string_key.empty())
                result[i].position_key = position_key++;
        }
        
        // set case sensitive flag
        for (unsigned i = 0; i < result.size(); ++i) {
            if (result[i].string_key.size() > 2 ||
                        (result[i].string_key.size() > 1 && result[i].string_key[0] != '-'))
            {
                // it is a long option
                result[i].case_insensitive = true;//is_style_active(long_case_insensitive);
            }
            else
            {
                // it is a short option
                result[i].case_insensitive = true ;//is_style_active(short_case_insensitive);
            }
        }

        for(auto var: result)
        {
        	var.dump();
        }
        return result;
    }

//    void
//    cmdline::finish_option(option& opt,
//                           vector<string>& other_tokens,
//                           const vector<style_parser>& style_parsers)
//    {
//        if (opt.string_key.empty())
//            return;
//
//        //
//        // Be defensive:
//        // will have no original token if option created by handle_additional_parser()
//        std::string original_token_for_exceptions = opt.string_key;
//        if (opt.original_tokens.size())
//            original_token_for_exceptions = opt.original_tokens[0];
//
//        try
//        {
//            // First check that the option is valid, and get its description.
//            const option_description* xd = m_desc->find_nothrow(opt.string_key,
//                    is_style_active(allow_guessing),
//                    is_style_active(long_case_insensitive),
//                    is_style_active(short_case_insensitive));
//
//            if (!xd)
//            {
//                if (m_allow_unregistered) {
//                    opt.unregistered = true;
//                    return;
//                } else {
//                    boost::throw_exception(unknown_option());
//                }
//            }
//            const option_description& d = *xd;
//
//            // Canonize the name
//            opt.string_key = d.key(opt.string_key);
//
//            // We check that the min/max number of tokens for the option
//            // agrees with the number of tokens we have. The 'adjacent_value'
//            // (the value in --foo=1) counts as a separate token, and if present
//            // must be consumed. The following tokens on the command line may be
//            // left unconsumed.
//
//            // We don't check if those tokens look like option, or not!
//
//            unsigned min_tokens = d.semantic()->min_tokens();
//            unsigned max_tokens = d.semantic()->max_tokens();
//
//            unsigned present_tokens = static_cast<unsigned>(opt.value.size() + other_tokens.size());
//
//            if (present_tokens >= min_tokens)
//            {
//                if (!opt.value.empty() && max_tokens == 0)
//                {
//                    boost::throw_exception(
//                        invalid_command_line_syntax(invalid_command_line_syntax::extra_parameter));
//                }
//
//                // If an option wants, at minimum, N tokens, we grab them there,
//                // when adding these tokens as values to current option we check
//                // if they look like options
//                if (opt.value.size() <= min_tokens)
//                {
//                    min_tokens -= static_cast<unsigned>(opt.value.size());
//                }
//                else
//                {
//                    min_tokens = 0;
//                }
//
//                // Everything's OK, move the values to the result.
//                for(;!other_tokens.empty() && min_tokens--; )
//                {
//                    // check if extra parameter looks like a known option
//                    // we use style parsers to check if it is syntactically an option,
//                    // additionally we check if an option_description exists
//                    vector<option> followed_option;
//                    vector<string> next_token(1, other_tokens[0]);
//                    for (unsigned i = 0; followed_option.empty() && i < style_parsers.size(); ++i)
//                    {
//                        followed_option = style_parsers[i](next_token);
//                    }
//                    if (!followed_option.empty())
//                    {
//                        original_token_for_exceptions = other_tokens[0];
//                        const option_description* od = m_desc->find_nothrow(other_tokens[0],
//                                  is_style_active(allow_guessing),
//                                  is_style_active(long_case_insensitive),
//                                  is_style_active(short_case_insensitive));
//                        if (od)
//                            boost::throw_exception(
//                                invalid_command_line_syntax(invalid_command_line_syntax::missing_parameter));
//                    }
//                    opt.value.push_back(other_tokens[0]);
//                    opt.original_tokens.push_back(other_tokens[0]);
//                    other_tokens.erase(other_tokens.begin());
//                }
//            }
//            else
//            {
//                boost::throw_exception(
//                            invalid_command_line_syntax(invalid_command_line_syntax::missing_parameter));
//
//            }
//        }
//        // use only original token for unknown_option / ambiguous_option since by definition
//        //    they are unrecognised / unparsable
//        catch(error_with_option_name& e)
//        {
//            // add context and rethrow
//            e.add_context(opt.string_key, original_token_for_exceptions, get_canonical_option_prefix());
//            throw;
//        }
//
//    }

    vector<Option> 
    Cmdline::parse_long_option(const string& tok)
    {
        vector<Option> result;

        if (tok.size() >= 3 && tok[0] == '-' && tok[1] == '-')
        {   
            string name, adjacent;
            string::size_type p = tok.find('=');
            if (p != tok.npos)
            {
                name = tok.substr(2, p-2);
                adjacent = tok.substr(p+1);
                
                if (adjacent.empty())
                    throw exception( Invalid_command_line_syntax(
                                                      Invalid_command_line_syntax::empty_adjacent_parameter, 
                                                      name,
                                                      name,
                                                      0));//get_canonical_option_prefix()) );
            }
            else
            {
                name = tok.substr(2);
            }
            Option opt;
            opt.string_key = name;
            if (!adjacent.empty())
            {
                opt.value.push_back(adjacent);
                opt.hasValue = true;
            }
            opt.original_tokens.push_back(tok);
            result.push_back(opt);
        }
        return result;
    }


    vector<Option> 
    Cmdline::parse_short_option(const string& tok)
    {
        if (tok.size() >= 2 && tok[0] == '-' && tok[1] != '-')
        {   
            vector<Option> result;

            string name = tok.substr(0,2);
            string adjacent = tok.substr(2);

            // Short options can be 'grouped', so that
            // "-d -a" becomes "-da". Loop, processing one
            // option at a time. We exit the loop when either
            // we've processed all the token, or when the remainder
            // of token is considered to be value, not further grouped
            // option.
            for(;;) {
                const OptionDescription* d = m_desc->find_nothrow(name, false, false, true);

                if(d && !adjacent.empty()){
                    // 'adjacent' is in fact further option.
                    Option opt;
                    opt.string_key = d->long_name();
                    opt.original_tokens.push_back(tok);
                    if(adjacent[0] == '=')
                    {
                    	adjacent.erase(adjacent.begin());
                    	opt.value.push_back(adjacent);
                    	result.push_back(opt);
                    	return result;
                    }
                    result.push_back(opt);
                    
                    if (adjacent.empty())
                    {
                        break;
                    }

                    name = string("-") + adjacent[0];
                    adjacent.erase(adjacent.begin());
                }
                else
                {
                    
                    Option opt;
                    opt.string_key = d?d->long_name():name;
                    opt.original_tokens.push_back(tok);
                    if (!adjacent.empty())
                        opt.value.push_back(adjacent);
                    result.push_back(opt);

                    break;
                }
            }
            return result;
        }
        return vector<Option>();
    }

}}
