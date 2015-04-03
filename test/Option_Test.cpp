#include "magellan/magellan.hpp"
#include "program_options.hpp"

using namespace std;
using namespace options;
using namespace hamcrest;

FIXTURE(OptionTest)
{
	SETUP()
	{

	}

	TEARDOWN()
	{

	}

	TEST("can parse 'help' right when input like '--help'")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()("help", "produce help message")
						("filter", "set filter");

		const char* argv[] = {"","--help"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);

		ASSERT_THAT(varMap.count("help"), is(1));
	}

	TEST("can parse 'filter' value = 1 when input like '--filter=1'")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()("filter", "set filter");

		const char* argv[] = {"", "--filter=1"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);

		ASSERT_THAT(varMap.count("filter"), is(1));
		ASSERT_THAT(varMap["filter"].value().str(), is(string("1")));
	}

	TEST("should parse not when given wrong input ")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()("filter", "set filter");

		const char* argv[] = {"", "--hello"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);

		ASSERT_THAT(varMap.count("filter"), is(0));
		ASSERT_THAT(varMap.count("hello"), is(0));
	}

	TEST("should parse variables right when input has short prefix with no value")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()
		        		("help,h", "produce help message")
						("filter,f", "set filter")
						("date", "set date");

		const char* argv[] = {"","-h", "-f", "-D"};
		const int argc = 3;

		VariablesMap varMap = parse_args(argc, argv, desc);
		ASSERT_THAT(varMap.count("help"), is(1));
		ASSERT_THAT(varMap.count("filter"), is(1));
		ASSERT_THAT(varMap.count("data"), is(0));
	}

	TEST("can parse '-ab' variables as -a and -b ")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()
		        		("help,h", "produce help message")
						("filter,f", "set filter");

		const char* argv[] = {"","-hf"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);

		ASSERT_THAT(varMap.count("help"), is(1));
		ASSERT_THAT(varMap.count("filter"), is(1));
	}

	TEST("can parse '-f=1' as 'filter' with value '1'")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()
			        				("help,h", "produce help message")
									("filter,f", "set filter");

		const char* argv[] = {"","-f=1"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);
		ASSERT_THAT(varMap.count("filter"), is(1));
		ASSERT_THAT(varMap["filter"].value().str(), is(string("1")));
	}

	TEST("should not parse '--f' as 'filter'")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()
			        				("help,h", "produce help message")
									("filter,f", "set filter");

		const char* argv[] = {"","--f"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);
		ASSERT_THAT(varMap.count("filter"), is(0));
	}

	TEST("can parse '-hf=1' as 'filter' with value '1' and 'help'")
	{
		OptionsDescription desc("Allowed options");
		desc.add_options()
			        				("help,h", "produce help message")
									("filter,f", "set filter");

		const char* argv[] = {"","-hf=1"};
		const int argc = 2;

		VariablesMap varMap = parse_args(argc, argv, desc);

		ASSERT_THAT(varMap.count("help"), is(1));
		ASSERT_THAT(varMap.count("filter"), is(1));
		ASSERT_THAT(varMap["filter"].value().str(), is(string("1")));
	}

};
