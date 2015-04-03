// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_VARIABLES_MAP_VP_2003_05_19
#define BOOST_VARIABLES_MAP_VP_2003_05_19

#include <string>
#include <map>
#include <set>
#include "Any.hpp"
#include <memory>

namespace options {

    struct  ParsedOptions;

    struct Value_semantic;
    struct VariablesMap;

    // forward declaration

    /** Stores in 'm' all options that are defined in 'options'. 
        If 'm' already has a non-defaulted value of an option, that value
        is not changed, even if 'options' specify some value.        
    */
     
    void store(const ParsedOptions& options, VariablesMap& m,
                    bool utf8 = false);

    /** Stores in 'm' all options that are defined in 'options'. 
        If 'm' already has a non-defaulted value of an option, that value
        is not changed, even if 'options' specify some value.        
        This is wide character variant.
    */

    /** Runs all 'notify' function for options in 'm'. */
     void notify(VariablesMap& m);

    /** Class holding value of option. Contains details about how the 
        value is set and allows to conveniently obtain the value.
    */
    struct  VariableValue
    {
        VariableValue() : defaulted(false) {}
        VariableValue(const Any& xv, bool xdefaulted) 
        : v(xv), defaulted(xdefaulted) 
        {}

        /// Returns true if no value is stored.
        bool empty() const;
        /** Returns true if the value was not explicitly
            given, but has default value. */
        bool isDefaulted() const;
        /** Returns the contained value. */
        const Any& value() const;

        /** Returns the contained value. */
        Any& value();
    private:
        Any v;
        bool defaulted;
        // Internal reference to value semantic. We need to run
        // notifications when *final* values of options are known, and
        // they are known only after all sources are stored. By that
        // time options_description for the first source might not
        // be easily accessible, so we need to store semantic here.
        std::shared_ptr<const Value_semantic> m_value_semantic;

        friend 
        void store(const ParsedOptions& options, 
              VariablesMap& m, bool);

        friend class  VariablesMap;
    };

    /** Implements string->string mapping with convenient value casting
        facilities. */
    struct  AbstractVariablesMap {

        AbstractVariablesMap();
        AbstractVariablesMap(const AbstractVariablesMap* next);

        virtual ~AbstractVariablesMap() {}

        /** Obtains the value of variable 'name', from *this and
            possibly from the chain of variable maps.

            - if there's no value in *this.
                - if there's next variable map, returns value from it
                - otherwise, returns empty value

            - if there's defaulted value
                - if there's next varaible map, which has a non-defauled
                  value, return that
                - otherwise, return value from *this

            - if there's a non-defauled value, returns it.
        */
        const VariableValue& operator[](const std::string& name) const;

        /** Sets next variable map, which will be used to find
           variables not found in *this. */
        void next(AbstractVariablesMap* next);

    private:
        /** Returns value of variable 'name' stored in *this, or
            empty value otherwise. */
        virtual const VariableValue& get(const std::string& name) const = 0;

        const AbstractVariablesMap* m_next;
    };

    /** Concrete variables map which store variables in real map. 
        
        This class is derived from std::map<std::string, variable_value>,
        so you can use all map operators to examine its content.
    */
    struct  VariablesMap : public AbstractVariablesMap,
                               public std::map<std::string, VariableValue>
    {
        VariablesMap();
        VariablesMap(const AbstractVariablesMap* next);

        const VariableValue& operator[](const std::string& name) const
        { return AbstractVariablesMap::operator[](name); }

        void clear(); 
        
        void notify();

    private:
        /** Implementation of abstract_variables_map::get
            which does 'find' in *this. */
        const VariableValue& get(const std::string& name) const;

        /** Names of option with 'final' values -- which should not
            be changed by subsequence assignments. */
        std::set<std::string> m_final;

        friend 
        void store(const ParsedOptions& options, 
                          VariablesMap& xm,
                          bool utf8);
        
        /** Names of required options, filled by parser which has
            access to options_description.
            The map values are the "canonical" names for each corresponding option.
            This is useful in creating diagnostic messages when the option is absent. */
        std::map<std::string, std::string> m_required;
    };

    inline bool
    VariableValue::empty() const
    {
        return v.empty();
    }

    inline bool
    VariableValue::isDefaulted() const
    {
        return defaulted;
    }

    inline
    const Any&
    VariableValue::value() const
    {
        return v;
    }

    inline
    Any&
    VariableValue::value()
    {
        return v;
    }

}

#endif
