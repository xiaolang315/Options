// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_PROGRAM_OPTIONS_SOURCE

#include "../include/program_options/ValueSemantic.hpp"
#include <program_options/detail/Cmdline.hpp>
#include "../include/program_options/Any.hpp"
#include <set>

#include <cctype>

namespace options {

    using namespace std;

    void 
    Value_semantic_codecvt_helper<char>::
    parse(Any& value_store, 
          const std::vector<std::string>& new_tokens,
          bool utf8) const
    {
        if (utf8) {
           throw exception(
                std::runtime_error("UTF-8 conversion not supported."));

        } else {
            // Already in local encoding, pass unmodified
            xparse(value_store, new_tokens);
        }        
    }

     std::string arg("arg");

    std::string
    Untyped_value::name() const
    {
        return arg;
    }
    
    unsigned 
    Untyped_value::min_tokens() const
    {
        if (m_zero_tokens)
            return 0;
        else
            return 1;
    }

    unsigned 
    Untyped_value::max_tokens() const
    {
        if (m_zero_tokens)
            return 0;
        else
            return 1;
    }


    void 
    Untyped_value::xparse(Any& value_store,
                          const std::vector<std::string>& new_tokens) const
    {
        if (!value_store.empty()) 
            throw exception(
                multiple_occurrences());
        if (new_tokens.size() > 1)
            throw exception(multiple_values());
        value_store = new_tokens.empty() ? std::string("") : new_tokens.front();
    }

    /* Validates bool value.
        Any of "1", "true", "yes", "on" will be converted to "1".<br>
        Any of "0", "false", "no", "off" will be converted to "0".<br>
        Case is ignored. The 'xs' vector can either be empty, in which
        case the value is 'true', or can contain explicit value.
    */
     void validate(Any& v, const vector<string>& xs,
                       bool*, int)
    {
        check_first_occurrence(v);
        string s(get_single_string(xs, true));

        for (size_t i = 0; i < s.size(); ++i)
            s[i] = char(tolower(s[i]));

        if (s.empty() || s == "on" || s == "yes" || s == "1" || s == "true")
            v = Any(true);
        else if (s == "off" || s == "no" || s == "0" || s == "false")
            v = Any(false);
        else
            throw exception(invalid_bool_value(s));
    }


    void validate(Any& v, const vector<string>& xs, std::string*, int)
    {
        check_first_occurrence(v);
        v = (get_single_string(xs));
    }


    namespace validators {

         
        void check_first_occurrence(const Any& value)
        {
            if (!value.empty())
                throw exception(
                    multiple_occurrences());
        }
    }


    invalid_option_value::
    invalid_option_value(const std::string& bad_value)
    : Validation_error(Validation_error::invalid_option_value)
    {
        set_substitute("value", bad_value);
    }

    invalid_bool_value::
    invalid_bool_value(const std::string& bad_value)
    : Validation_error(Validation_error::invalid_bool_value)
    {
        set_substitute("value", bad_value);
    }






    error_with_option_name::error_with_option_name( const std::string& template_,
                                                  const std::string& option_name,
                                                  const std::string& original_token,
                                                  int option_style) : 
                                        error(template_),
                                        m_option_style(option_style),
                                        m_error_template(template_)
    {
        //                     parameter            |     placeholder               |   value
        //                     ---------            |     -----------               |   -----
        set_substitute_default("canonical_option",  "option '%canonical_option%'",  "option");
        set_substitute_default("value",             "argument ('%value%')",         "argument");
        set_substitute_default("prefix",            "%prefix%",                     "");
        m_substitutions["option"] = option_name;
        m_substitutions["original_token"] = original_token;
    }


    const char* error_with_option_name::what() const throw()
    {
        // will substitute tokens each time what is run()
        substitute_placeholders(m_error_template);

        return m_message.c_str();
    }

    void error_with_option_name::replace_token(const string& from, const string& to) const
    {
        while (1)
        {
            std::size_t pos = m_message.find(from.c_str(), 0, from.length());
            // not found: all replaced
            if (pos == std::string::npos)
                return;
            m_message.replace(pos, from.length(), to);
        }
    }

    string error_with_option_name::get_canonical_option_prefix() const
    {
        return "-";        
    }


    string error_with_option_name::get_canonical_option_name() const
    {
        if (!m_substitutions.find("option")->second.length())
            return m_substitutions.find("original_token")->second;

        string original_token   = strip_prefixes(m_substitutions.find("original_token")->second);
        string option_name      = strip_prefixes(m_substitutions.find("option")->second);

        // no prefix
        return option_name;
    }


    void error_with_option_name::substitute_placeholders(const string& error_template) const
    {
        m_message = error_template;
        std::map<std::string, std::string> substitutions(m_substitutions);
        substitutions["canonical_option"]   = get_canonical_option_name();
        substitutions["prefix"]             = get_canonical_option_prefix();


        //
        //  replace placeholder with defaults if values are missing 
        // 
        for (map<string, string_pair>::const_iterator iter = m_substitution_defaults.begin();
              iter != m_substitution_defaults.end(); ++iter)
        {
            // missing parameter: use default
            if (substitutions.count(iter->first) == 0 ||
                substitutions[iter->first].length() == 0)
                replace_token(iter->second.first, iter->second.second);
        }


        //
        //  replace placeholder with values
        //  placeholder are denoted by surrounding '%'
        // 
        for (map<string, string>::iterator iter = substitutions.begin();
              iter != substitutions.end(); ++iter)
            replace_token('%' + iter->first + '%', iter->second);
    }


    void Ambiguous_option::substitute_placeholders(const string& original_error_template) const
    {
        // For short forms, all alternatives must be identical, by
        //      definition, to the specified option, so we don't need to
        //      display alternatives

        string error_template  = original_error_template;
        // remove duplicates using std::set
        std::set<std::string>   alternatives_set (m_alternatives.begin(), m_alternatives.end());
        std::vector<std::string> alternatives_vec (alternatives_set.begin(), alternatives_set.end());

        error_template += " and matches ";
        // Being very cautious: should be > 1 alternative!
        if (alternatives_vec.size() > 1)
        {
            for (unsigned i = 0; i < alternatives_vec.size() - 1; ++i)
                error_template += "'%prefix%" + alternatives_vec[i] + "', ";
            error_template += "and ";
        }

        // there is a programming error if multiple options have the same name...
        if (m_alternatives.size() > 1 && alternatives_vec.size() == 1)
            error_template += "different versions of ";

        error_template += "'%prefix%" + alternatives_vec.back() + "'";


        // use inherited logic
        error_with_option_name::substitute_placeholders(error_template);
    }






    string 
    Validation_error::get_template(kind_t kind)
    {
        // Initially, store the message in 'const char*' variable,
        // to avoid conversion to std::string in all cases.
        const char* msg;
        switch(kind)
        {
        case invalid_bool_value:
            msg = "the argument ('%value%') for option '%canonical_option%' is invalid. Valid choices are 'on|off', 'yes|no', '1|0' and 'true|false'";
            break;
        case invalid_option_value:
            msg = "the argument ('%value%') for option '%canonical_option%' is invalid";
            break;
        case multiple_values_not_allowed:
            msg = "option '%canonical_option%' only takes a single argument";
            break;
        case at_least_one_value_required:
            msg = "option '%canonical_option%' requires at least one argument";
            break;
        // currently unused
        case invalid_option:
            msg = "option '%canonical_option%' is not valid";
            break;
        default:
            msg = "unknown error";
        }
        return msg;
    }
    
}
