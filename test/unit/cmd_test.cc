#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <string>
#include <iostream>
#include <sstream>

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

#include "cmd.h"

// A list of sample arguments
std::vector<CommandLine::Args> SampleArgs = {
  CommandLine::MakeArgs('h', "help", "help", 0),
  CommandLine::MakeArgs(0, NULL, NULL, 0),
};

// A list of sample commands
std::vector<DispatchWithHelp> SampleCmds = {
  {"succeed", [](CommandLine &){return true;}, "Be successful"},
  {"fail", [](CommandLine &){return false;}, "Fail miserably"},
  {nullptr, nullptr, nullptr},
};

// A sample help template
std::string SampleHelpTemplate =
  "%VERSION%"
  "Usage: foo [options] command\n"
  "\n"
  "%COMMANDS%"
  "\n"
  "See foo(8)\n";

// If invalid arguments are passed, an error is printed.
TEST(RunCommandLineTest, InvalidArgument) {
  testing::internal::CaptureStderr();

  const char argc = 2;
  const char *argv[] = {"aptc", "--foo"};

  EXPECT_EQ(RunCommandLine(SampleArgs, SampleCmds, SampleHelpTemplate, argc, argv),
	    1);

  EXPECT_THAT(testing::internal::GetCapturedStderr(),
	      testing::HasSubstr("E: Command line"));

  _config->Clear();
  _error->Discard();
}

// If help flag is passed, the help text is printed on standard output.
TEST(RunCommandLineTest, HelpFlagOn) {
  testing::internal::CaptureStdout();

  const char argc = 2;
  const char *argv[] = {"aptc", "-h"};

  EXPECT_EQ(RunCommandLine(SampleArgs, SampleCmds, SampleHelpTemplate, argc, argv),
	    0);

  EXPECT_THAT(testing::internal::GetCapturedStdout(),
	      testing::HasSubstr("Usage: foo"));

  _config->Clear();
  _error->Discard();
}

// If no command is specified, the help text is printed on standard output.
TEST(RunCommandLineTest, NoCommand) {
  testing::internal::CaptureStdout();

  const char argc = 1;
  const char *argv[] = {"aptc"};

  EXPECT_EQ(RunCommandLine(SampleArgs, SampleCmds, SampleHelpTemplate, argc, argv),
	    1);

  EXPECT_THAT(testing::internal::GetCapturedStdout(),
	      testing::HasSubstr("Usage: foo"));

  _config->Clear();
  _error->Discard();
}

// If an invalid command is specified, an error is printed.
TEST(RunCommandLineTest, InvalidCommand) {
  testing::internal::CaptureStderr();

  const char argc = 2;
  const char *argv[] = {"aptc", "foo"};

  EXPECT_EQ(RunCommandLine(SampleArgs, SampleCmds, SampleHelpTemplate, argc, argv),
	    100);

  EXPECT_THAT(testing::internal::GetCapturedStderr(),
	      testing::HasSubstr("E: Invalid operation foo"));

  _config->Clear();
  _error->Discard();
}

// If valid command is specified, it's executated.
TEST(RunCommandLineTest, ValidCommand) {
  testing::internal::CaptureStdout();

  const char argc = 2;
  const char *argv[] = {"aptc", "succeed"};

  EXPECT_EQ(RunCommandLine(SampleArgs, SampleCmds, SampleHelpTemplate, argc, argv),
	    0);

  EXPECT_EQ(testing::internal::GetCapturedStdout(), "");

  _config->Clear();
  _error->Discard();
}

// The version help line contains the version and architecture.
TEST(HelpVersionTest, MatchExpectedStructure) {

  const std::string HelpRegex =
    "aptc [[:digit:]]+\\.[[:digit:]]+\\.[[:digit:]]+ \\([[:alnum:]]+\\)\n";

  ASSERT_THAT(HelpVersion(), testing::MatchesRegex(HelpRegex));
}

// The commands help contains a line for each available command.
TEST(HelpCommandsTest, MatchGivenCommands) {

  const std::string Help =
    "Available commands:\n"
    "  succeed - Be successful\n"
    "  fail - Fail miserably\n";

  ASSERT_EQ(HelpCommands(SampleCmds), Help);
}

// The %VERSION% and %COMMANDS% placeholders in the the help template are
// replaced with the appropriate text.
TEST(HelpFormatTest, ReplacePlaceholders) {

  const std::string Help = HelpFormat(SampleHelpTemplate, SampleCmds);

  EXPECT_THAT(Help, testing::StartsWith("aptc "));
  EXPECT_THAT(Help, testing::HasSubstr("Available commands:"));
  EXPECT_THAT(Help, testing::HasSubstr("See foo(8)"));
}
