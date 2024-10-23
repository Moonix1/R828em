#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Log/Log.h"
#include "Assemble.h"

static std::string GetFileContents(std::string path) {
	std::ifstream file(path);
	std::stringstream buffer;

	buffer << file.rdbuf();
	return buffer.str();
}

static void Usage(char *programFile) {
	WARN("Usage: {0} <SUBCOMMAND> [ARGS]", programFile);
}

static char *Shift(int &argc, char ***argv) {
	char *result = **argv;
	argc -= 1;
	*argv += 1;
	return result;
}

int main(int argc, char **argv) {
	RASM::Log::Init();
	char *program = Shift(argc, &argv);
	if (argc < 1) {
		Usage(program);
		ERROR("Invalid Arguments");
		exit(1);
	}

	char *subcommand = Shift(argc, &argv);
	if (std::string(subcommand) == "build") {
		if (argc < 1) {
			Usage(program);
			ERROR("Missing subcommand arguments!");
			exit(1);
		}
		char *input = Shift(argc, &argv);
		RASM::Assemble assemble(GetFileContents(input));
	} else {
		ERROR("Invalid subcommand: {0}", subcommand);
		exit(1);
	}
}