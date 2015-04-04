#include "program_options/Parsers.hpp"
#include "program_options/OptionsDescription.hpp"
#include "program_options/ValueSemantic.hpp"
#include "program_options/VariablesMap.hpp"

#include <cassert>
#include <iostream>

namespace  options {

    using namespace std;

    void store(const ParsedOptions& options, VariablesMap& map)
    {       
        assert(options.description);

        const OptionsDescription& desc = *options.description;

        std::map<std::string, VariableValue>& m = map;

        std::set<std::string> new_final;

        string option_name;
        string original_token;

        for (auto var : options.options)
        {
            option_name = var.string_key;
            original_token = var.original_tokens.size() ?
                var.original_tokens[0] :
                option_name;

            if (option_name.empty())
                continue;

            if (var.unregistered)
                continue;

            if (map.m_final.count(option_name))
                continue;

            string original_token = var.original_tokens.size() ?
                var.original_tokens[0]     : "";

            const OptionDescription* d = desc.find(option_name, var.hasValue,
                                                    false, false);

            if(!d) continue;
            VariableValue& v = m[option_name];            
            if (v.isDefaulted()) {
                v = VariableValue();
            }
                
            d->semantic()->parse(v.value(), var.value);

            v.m_value_semantic = d->semantic();
                
            if (!d->semantic()->is_composing())
                new_final.insert(option_name);
        }

        map.m_final.insert(new_final.begin(), new_final.end());

        // Second, apply default values and store required options.
        const vector<std::shared_ptr<OptionDescription> >& all = desc.options();
        for(auto var : all)
        {
            const OptionDescription& d = *var;
            string key = d.key("");
            if (key.empty())
            {
                continue;
            }
            if (m.count(key) == 0) {
            
                Any def;
                if (d.semantic()->apply_default(def)) {
                    m[key] = VariableValue(def, true);
                    m[key].m_value_semantic = d.semantic();
                }
            }  

            // add empty value if this is an required option
            if (d.semantic()->is_required()) {
                string canonical_name = d.canonical_display_name(options.m_options_prefix);
                if (canonical_name.length() > map.m_required[key].length())
                    map.m_required[key] = canonical_name;
            }
        }

    }
     
    void notify(VariablesMap& vm)
    {        
        vm.notify();               
    }

    AbstractVariablesMap::AbstractVariablesMap()
    : m_next(0)
    {}

    AbstractVariablesMap::
    AbstractVariablesMap(const AbstractVariablesMap* next)
    : m_next(next)
    {}

    const VariableValue& 
    AbstractVariablesMap::operator[](const std::string& name) const
    {
        const VariableValue& v = get(name);
        if (v.empty() && m_next)
            return (*m_next)[name];
        else if (v.isDefaulted() && m_next) {
            const VariableValue& v2 = (*m_next)[name];
            if (!v2.empty() && !v2.isDefaulted())
                return v2;
            else return v;
        } else {
            return v;
        }
    }

    void 
    AbstractVariablesMap::next(AbstractVariablesMap* next)
    {
        m_next = next;
    }

    VariablesMap::VariablesMap()
    {}

    VariablesMap::VariablesMap(const AbstractVariablesMap* next)
    : AbstractVariablesMap(next)
    {}

    void VariablesMap::clear()
    {
        std::map<std::string, VariableValue>::clear();
        m_final.clear();
        m_required.clear();
    }

    const VariableValue&
    VariablesMap::get(const std::string& name) const
    {
        static VariableValue empty;
        const_iterator i = this->find(name);
        if (i == this->end())
            return empty;
        else
            return i->second;
    }
    
    void
    VariablesMap::notify()
    {
        for (map<string, string>::const_iterator r = m_required.begin();
             r != m_required.end();
             ++r)
        {
            const string& opt = r->first;
            const string& display_opt = r->second;
            map<string, VariableValue>::const_iterator iter = find(opt);
            if (iter == end() || iter->second.empty()) 
            {
                return ;
            }
        }

        for (map<string, VariableValue>::iterator k = begin(); 
             k != end(); 
             ++k) 
        {
            /* Users might wish to use variables_map to store their own values
               that are not parsed, and therefore will not have value_semantics
               defined. Do no crash on such values. In multi-module programs,
               one module might add custom values, and the 'notify' function
               will be called after that, so we check that value_sematics is 
               not NULL. See:
                   https://svn.boost.org/trac/boost/ticket/2782
            */
            if (k->second.m_value_semantic)
                k->second.m_value_semantic->notify(k->second.value());
        }               
    }
    
    bool VariablesMap::has(const std::string& name) const
    {
    	return std::map<std::string, VariableValue>::count(name) >= 1;
    }

}
