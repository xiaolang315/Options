#ifndef OPTIONSDESCRIPTION_H
#define OPTIONSDESCRIPTION_H

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iosfwd>
#include <memory>

#include "ValueSemantic.hpp"

namespace options {

    struct OptionDescription
	{
        OptionDescription();

        OptionDescription(const char* name,
                           const Value_semantic* s);

        OptionDescription(const char* name,
                           const Value_semantic* s,
                           const char* description);

        virtual ~OptionDescription();

        enum match_result { no_match, full_match, approximate_match };

        match_result match(const std::string& option, bool approx,
                           bool long_ignore_case, bool short_ignore_case) const;
        
        const std::string& key(const std::string& option) const;
        
        std::string canonical_display_name(int canonical_option_style = 0) const;

        const std::string& long_name() const;

        const std::string& description() const;

        std::shared_ptr<const Value_semantic> semantic() const;
        
        std::string format_name() const;

        std::string format_parameter() const;

    private:
    
        OptionDescription& set_name(const char* name);

        std::string m_short_name, m_long_name, m_description;
        std::shared_ptr<const Value_semantic> m_value_semantic;
    };

    struct  OptionsDescription;
    struct  Description_easy_init {

        Description_easy_init(OptionsDescription* owner);

        Description_easy_init&
        operator()(const char* name,
                   const char* description);

        Description_easy_init&
        operator()(const char* name,
                   const Value_semantic* s);
        
        Description_easy_init&
        operator()(const char* name,
                   const Value_semantic* s,
                   const char* description);
       
    private:
        OptionsDescription* owner;
    };


    struct OptionsDescription
    {
        OptionsDescription(unsigned line_length = default_line_length,
                            unsigned min_description_length = default_line_length / 2);

        OptionsDescription(const std::string& caption,
                            unsigned line_length = default_line_length,
                            unsigned min_description_length = default_line_length / 2);
        Description_easy_init add_options();
        void add(std::shared_ptr<OptionDescription> desc);
        OptionsDescription& add(const OptionsDescription& desc);

        unsigned get_option_column_width() const;


        const OptionDescription* find(const std::string& name,
                                       bool approx, 
                                       bool long_ignore_case = false,
                                       bool short_ignore_case = false) const;

        const OptionDescription* find_nothrow(const std::string& name,
                                               bool approx,
                                               bool long_ignore_case = false,
                                               bool short_ignore_case = false) const;


        const std::vector< std::shared_ptr<OptionDescription> >& options() const;

        friend std::ostream& operator<<(std::ostream& os, 
                                             const OptionsDescription& desc);

        void print(std::ostream& os, unsigned width = 0) const;

    private:
        typedef std::map<std::string, int>::const_iterator name2index_iterator;
        typedef std::pair<name2index_iterator, name2index_iterator> 
            approximation_range;

        enum { default_line_length = 80};

        std::string m_caption;
        const unsigned m_line_length;
        const unsigned m_min_description_length;

        std::vector< std::shared_ptr<OptionDescription> > m_options;

        std::vector<bool> belong_to_group;

        std::vector< std::shared_ptr<OptionsDescription> > groups;

    };
}

#endif
