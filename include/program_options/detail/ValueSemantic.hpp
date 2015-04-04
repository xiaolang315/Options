#include "../Any.hpp"

namespace options { 

    extern std::string arg;
    
    template<class T, class charT>
    std::string
    typed_value<T, charT>::name() const
    {
        std::string const& var = (m_value_name.empty() ? arg : m_value_name);
        if (!m_implicit_value.empty() && !m_implicit_value_as_text.empty()) {
            std::string msg = "[=" + var + "(=" + m_implicit_value_as_text + ")]";
            if (!m_default_value.empty() && !m_default_value_as_text.empty())
                msg += " (=" + m_default_value_as_text + ")";
            return msg;
        }
        else if (!m_default_value.empty() && !m_default_value_as_text.empty()) {
            return var + " (=" + m_default_value_as_text + ")";
        } else {
            return var;
        }
    }

    namespace validators {
        template<class charT>
        const std::basic_string<charT>& get_single_string(
            const std::vector<std::basic_string<charT> >& v, 
            bool allow_empty = false)
        {
            static std::basic_string<charT> empty;
            if (v.size() == 1)
                return v.front();
            return empty;
        }
                
    }

    using namespace validators;

    template<class T, class charT>
    void validate(Any& v, 
                  const std::vector< std::basic_string<charT> >& xs, 
                  T*, long)
    {
        std::basic_string<charT> s(validators::get_single_string(xs));
        v = s;
    }

    void validate(Any& v, 
                       const std::vector<std::string>& xs, 
                       bool*,
                       int);

    template<class T, class charT>
    void validate(Any& v, 
                  const std::vector<std::basic_string<charT> >& s, 
                  std::vector<T>*,
                  int)
    {
    }

    template<class T, class charT>
    void 
    typed_value<T, charT>::
    xparse(Any& value_store, 
           const std::vector<std::basic_string<charT> >& new_tokens) const
    {
        if (new_tokens.empty() && !m_implicit_value.empty())
            value_store = m_implicit_value;
        else
            validate(value_store, new_tokens, (T*)0, 0);
    }

    template<class T>
    typed_value<T>*
    value()
    {
        return value<T>(0);
    }

    template<class T>
    typed_value<T>*
    value(T* v)
    {
        typed_value<T>* r = new typed_value<T>(v);

        return r;        
    }

    template<class T>
    typed_value<T, wchar_t>*
    wvalue()
    {
        return wvalue<T>(0);
    }

    template<class T>
    typed_value<T, wchar_t>*
    wvalue(T* v)
    {
        typed_value<T, wchar_t>* r = new typed_value<T, wchar_t>(v);

        return r;        
    }



}
