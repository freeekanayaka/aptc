#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <string>
#include <iostream>
#include <sstream>

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

#include "cmd.h"

#include "fixture-config.h"
#include "fixture-home.h"

// A list of sample arguments
std::vector<CommandLine::Args> SampleArgs = {
  CommandLine::MakeArgs('f', "foo", "foo", 0),
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

class RunCommandLineTest : public testing::Test {
protected:
  virtual void SetUp() {
    Config.SetUp();
    Home.SetUp();
  }
  virtual void TearDown() {
    Config.TearDown();
    Home.TearDown();
  }
  FixtureConfig Config;
  FixtureHome Home;
};

// If invalid arguments are passed, an error is printed.
TEST_F(RunCommandLineTest, InvalidArgument) {
  testing::internal::CaptureStderr();

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  const char argc = 2;
  const char *argv[] = {"aptc", "--bar"};

  EXPECT_EQ(RunCommandLine(Info, argc, argv), 1);

  EXPECT_THAT(testing::internal::GetCapturedStderr(),
	      testing::HasSubstr("E: Command line"));

}

// If help flag is passed, the help text is printed on standard output.
TEST_F(RunCommandLineTest, HelpFlagOn) {
  testing::internal::CaptureStdout();

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  const char argc = 2;
  const char *argv[] = {"aptc", "-h"};

  EXPECT_EQ(RunCommandLine(Info, argc, argv), 0);

  EXPECT_THAT(testing::internal::GetCapturedStdout(),
	      testing::HasSubstr("Usage: foo"));
}

// If version flag is passed, the version text is printed on standard output.
TEST_F(RunCommandLineTest, VerboseFlagOn) {
  testing::internal::CaptureStdout();

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  const char argc = 2;
  const char *argv[] = {"aptc", "-v"};

  EXPECT_EQ(RunCommandLine(Info, argc, argv), 0);

  auto Output = testing::internal::GetCapturedStdout();
  EXPECT_THAT(Output, testing::StartsWith("aptc "));

  EXPECT_THAT(Output,
	      testing::Not(testing::HasSubstr("Usage: foo")));
}

// If no command is specified, the help text is printed on standard output.
TEST_F(RunCommandLineTest, NoCommand) {
  testing::internal::CaptureStdout();

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  const char argc = 1;
  const char *argv[] = {"aptc"};

  EXPECT_EQ(RunCommandLine(Info, argc, argv), 1);

  EXPECT_THAT(testing::internal::GetCapturedStdout(),
	      testing::HasSubstr("Usage: foo"));
}

// If the special command 'help' is passed, the help text is printed.
TEST_F(RunCommandLineTest, HelpCommand) {
  testing::internal::CaptureStdout();

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  const char argc = 2;
  const char *argv[] = {"aptc", "help"};

  EXPECT_EQ(RunCommandLine(Info, argc, argv), 0);

  EXPECT_THAT(testing::internal::GetCapturedStdout(),
	      testing::HasSubstr("Usage: foo"));
}

// If an invalid command is specified, an error is printed.
TEST_F(RunCommandLineTest, InvalidCommand) {
  testing::internal::CaptureStderr();

  const char argc = 2;
  const char *argv[] = {"aptc", "foo"};

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  EXPECT_EQ(RunCommandLine(Info, argc, argv), 100);

  EXPECT_THAT(testing::internal::GetCapturedStderr(),
	      testing::HasSubstr("E: Invalid operation foo"));
}

// If an init hook is specified and it returns false, an error is printed.
TEST_F(RunCommandLineTest, FailedInit) {
  testing::internal::CaptureStderr();

  const char argc = 2;
  const char *argv[] = {"aptc", "succeed"};

  auto SampleInit = [](CommandLine &){
    _error->Error("Boom");
    return false;
  };
  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, SampleInit};
  EXPECT_EQ(RunCommandLine(Info, argc, argv), 100);

  EXPECT_THAT(testing::internal::GetCapturedStderr(),
	      testing::HasSubstr("E: Boom"));
}

// If valid command is specified, it's executed.
TEST_F(RunCommandLineTest, ValidCommand) {
  testing::internal::CaptureStdout();

  const char argc = 2;
  const char *argv[] = {"aptc", "succeed"};

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  EXPECT_EQ(RunCommandLine(Info, argc, argv), 0);

  EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

// If valid argument is specified, it's saved in the config.
TEST_F(RunCommandLineTest, ValidArgument) {
  testing::internal::CaptureStdout();

  const char argc = 3;
  const char *argv[] = {"aptc", "--foo", "succeed"};

  CommandLineInfo Info = {SampleArgs, SampleCmds, SampleHelpTemplate, nullptr};
  EXPECT_EQ(RunCommandLine(Info, argc, argv), 0);

  EXPECT_TRUE(_config->FindB("foo"));
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
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
