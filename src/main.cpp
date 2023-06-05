#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <fstream>

#include "conv/codegen.hpp"
#include "conv/lexer.hpp"
#include "conv/parser.hpp"
#include "conv/ssa.hpp"
#include "conv/typeck.hpp"
#include "passes/pass_manager.hpp"

int main(int argc, char* argv[]) {
  bool opt = false, print_usage = false, print_pass = false, dump_token = false;
  char *src = nullptr, *output = nullptr, *ir_file = nullptr;

  // parse command line options and check
  for (int ch; (ch = getopt(argc, argv, "Sdpl:o:O:ht")) != -1;) {
    switch (ch) {
      case 'S':
        // do nothing
        break;
      case 'd':
        debug_mode = true;
        break;
      case 'p':
        print_pass = true;
        break;
      case 'l':
        ir_file = strdup(optarg);
        break;
      case 'o':
        output = strdup(optarg);
        break;
      case 'O':
        opt = atoi(optarg) > 0;
        break;
      case 'h':
        print_usage = true;
        break;
      case 't':
        dump_token = true;
        break;
      default:
        break;
    }
  }

  if (optind <= argc) {
    src = argv[optind];
  }

  dbg(src, output, ir_file, opt, print_usage, print_pass, debug_mode);

  if (print_pass) {
    print_passes();
    return 0;
  }

  if (src == nullptr || print_usage) {
    fprintf(stderr,
            "Usage: %s [-l ir_file] [-S] [-p (print passes)] [-d (debug mode)] "
            "[-o output_file] [-O level] input_file\n",
            argv[0]);
    return !print_usage && SYSTEM_ERROR;
  }

  std::ifstream ifs(src);
  Lexer lexer(&ifs);

  if (dump_token) {
    lexer.DumpTokens();
    return 0;
  }

  Parser parser(lexer);
  Program program = parser.ParseProgram();

  dbg("parsing success");
  type_check(program);  // 失败时直接就 exit(1) 了
  dbg("type_check success");
  auto* ir = convert_ssa(program);
  run_passes(ir, opt);
  if (ir_file != nullptr) {
    std::ofstream(ir_file) << *ir;
  }
  if (output != nullptr) {
    auto* code = machine_code_generation(ir);
    run_passes(code, opt);
    std::ofstream(output) << *code;
  }

  free(output);
  free(ir_file);

  return 0;
}
