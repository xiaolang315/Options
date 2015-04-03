// Copyright Vladimir Prus 2002-2004.
// Copyright Bertolt Mildner 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_OPTION_DESCRIPTION_VP_2003_05_19
#define BOOST_OPTION_DESCRIPTION_VP_2003_05_19


#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iosfwd>
#include <memory>

#include "Errors.hpp"
#include "ValueSemantic.hpp"

namespace options {

    /** Describes one possible command line/config file option. There are two
        kinds of properties of an option. First describe it syntactically and
        are used only to validate input. Second affect interpretation of the
        option, for example default value for it or function that should be
        called  when the value is finally known. Routines which perform parsing
        never use second kind of properties -- they are side effect free.
        @sa options_description
    */
    struct OptionDescription
	{
        OptionDescription();

        /** Initializes the object with the passed data.

            Note: it would be nice to make the second parameter auto_ptr,
            to explicitly pass ownership. Unfortunately, it's often needed to
            create objects of types derived from 'value_semantic':
               options_description d;
               d.add_options()("a", parameter<int>("n")->default_value(1));
            Here, the static type returned by 'parameter' should be derived
            from value_semantic.

            Alas, derived->base conversion for auto_ptr does not really work,
            see
            http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2000/n1232.pdf
            http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#84

            So, we have to use plain old pointers. Besides, users are not
            expected to use the constructor directly.

            
            The 'name' parameter is interpreted by the following rules:
            - if there's no "," character in 'name', it specifies long name
            - otherwise, the part before "," specifies long name and the part
            after -- short name.
        */
        OptionDescription(const char* name,
                           const Value_semantic* s);

        /** Initializes the class with the passed data. 
         */
        OptionDescription(const char* name,
                           const Value_semantic* s,
                           const char* description);

        virtual ~OptionDescription();

        enum match_result { no_match, full_match, approximate_match };

        /** Given 'option', specified in the input source,
            returns 'true' if 'option' specifies *this.
        */
        match_result match(const std::string& option, bool approx,
                           bool long_ignore_case, bool short_ignore_case) const;

        /** Returns the key that should identify the option, in
            particular in the variables_map class.
            The 'option' parameter is the option spelling from the
            input source.
            If option name contains '*', returns 'option'.
            If long name was specified, it's the long name, otherwise
            it's a short name with prepended '-'.
        */
        const std::string& key(const std::string& option) const;


        /** Returns the canonical name for the option description to enable the user to
            recognised a matching option.
            1) For short options ('-', '/'), returns the short name prefixed.
            2) For long options ('--' / '-') returns the long name prefixed
            3) All other cases, returns the long name (if present) or the short name,
                unprefixed.
        */
        std::string canonical_display_name(int canonical_option_style = 0) const;

        const std::string& long_name() const;

        /// Explanation of this option
        const std::string& description() const;

        /// Semantic of option's value
        std::shared_ptr<const Value_semantic> semantic() const;
        
        /// Returns the option name, formatted suitably for usage message. 
        std::string format_name() const;

        /** Returns the parameter name and properties, formatted suitably for
            usage message. */
        std::string format_parameter() const;

    private:
    
        OptionDescription& set_name(const char* name);

        std::string m_short_name, m_long_name, m_description;
        // shared_ptr is needed to simplify memory management in
        // copy ctor and destructor.
        std::shared_ptr<const Value_semantic> m_value_semantic;
    };

    struct  OptionsDescription;

    /** Class which provides convenient creation syntax to option_description. 
     */        
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


    /** A set of option descriptions. This provides convenient interface for
        adding new option (the add_options) method, and facilities to search
        for options by name.
        
        See @ref a_adding_options "here" for option adding interface discussion.
        @sa option_description
    */
    struct OptionsDescription
    {
        OptionsDescription(unsigned line_length = default_line_length,
                            unsigned min_description_length = default_line_length / 2);

        /** Creates the instance. The 'caption' parameter gives the name of
            this 'options_description' instance. Primarily useful for output.
            The 'description_length' specifies the number of columns that
            should be reserved for the description text; if the option text
            encroaches into this, then the description will start on the next
            line.
        */
        OptionsDescription(const std::string& caption,
                            unsigned line_length = default_line_length,
                            unsigned min_description_length = default_line_length / 2);
        /** Returns an object of implementation-defined type suitable for adding
            options to options_description. The returned object will
            have overloaded operator() with parameter type matching
            'option_description' constructors. Calling the operator will create
            new option_description instance and add it.
        */
        Description_easy_init add_options();
        /** Adds new variable description. Throws duplicate_variable_error if
            either short or long name matches that of already present one. 
        */
        void add(std::shared_ptr<OptionDescription> desc);
        /** Adds a group of option description. This has the same
            effect as adding all option_descriptions in 'desc' 
            individually, except that output operator will show
            a separate group.
            Returns *this.
        */
        OptionsDescription& add(const OptionsDescription& desc);

        /** Find the maximum width of the option column, including options 
            in groups. */
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

        /** Produces a human readable output of 'desc', listing options,
            their descriptions and allowed parameters. Other options_description
            instances previously passed to add will be output separately. */
        friend std::ostream& operator<<(std::ostream& os, 
                                             const OptionsDescription& desc);

        /** Outputs 'desc' to the specified stream, calling 'f' to output each
            option_description element. */
        void print(std::ostream& os, unsigned width = 0) const;

    private:
        typedef std::map<std::string, int>::const_iterator name2index_iterator;
        typedef std::pair<name2index_iterator, name2index_iterator> 
            approximation_range;

        enum { default_line_length = 80};
        //approximation_range find_approximation(const std::string& prefix) const;

        std::string m_caption;
        const unsigned m_line_length;
        const unsigned m_min_description_length;
        
        // Data organization is chosen because:
        // - there could be two names for one option
        // - option_add_proxy needs to know the last added option
        std::vector< std::shared_ptr<OptionDescription> > m_options;

        // Whether the option comes from one of declared groups.

        std::vector<bool> belong_to_group;

        std::vector< std::shared_ptr<OptionsDescription> > groups;

    };
}

#endif
