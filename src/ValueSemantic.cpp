
#include "program_options/ValueSemantic.hpp"
#include <program_options/detail/Cmdline.hpp>
#include "program_options/Any.hpp"
#include <set>

#include <cctype>

namespace options {

    using namespace std;

    void 
    Value_semantic_codecvt_helper<char>::
    parse(Any& value_store, 
          const std::vector<std::string>& new_tokens) const
    {
       xparse(value_store, new_tokens);
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
        string s(get_single_string(xs, true));

        for (size_t i = 0; i < s.size(); ++i)
            s[i] = char(tolower(s[i]));

        if (s.empty() || s == "on" || s == "yes" || s == "1" || s == "true")
            v = Any(true);
        else if (s == "off" || s == "no" || s == "0" || s == "false")
            v = Any(false);
        else
            {}
    }


    void validate(Any& v, const vector<string>& xs, std::string*, int)
    {
        v = (get_single_string(xs));
    }
       
}
