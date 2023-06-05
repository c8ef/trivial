#include <fstream>
#include <string>

#include "argparse/argparse.hpp"
#include "conv/codegen.hpp"
#include "conv/lexer.hpp"
#include "conv/parser.hpp"
#include "conv/ssa.hpp"
#include "conv/typeck.hpp"
#include "passes/pass_manager.hpp"

int main(int argc, char* argv[]) {
  argparse::ArgumentParser cli("trivial", "20230605");
  cli.add_argument("-d", "--debug")
      .help("debug mode")
      .default_value(false)
      .implicit_value(true);
  cli.add_argument("-p", "--print-pass")
      .help("print passes")
      .default_value(false)
      .implicit_value(true);
  cli.add_argument("-l", "--ir-file").help("output ir file").default_value("");
  cli.add_argument("-i", "--input").help("input source file").required();
  cli.add_argument("-o", "--output").help("output assembly").default_value("");
  cli.add_argument("--dump-token")
      .help("dump lexer output")
      .default_value(false)
      .implicit_value(true);

  try {
    cli.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << '\n';
    exit(1);
  }

  bool print_pass = false;
  bool dump_token = false;
  std::string src;
  std::string output;
  std::string ir_file;

  dump_token = cli["--dump-token"] == true;
  debug_mode = cli["--debug"] == true;
  print_pass = cli["--print-pass"] == true;
  src = cli.get<std::string>("--input");
  output = cli.get<std::string>("--output");
  ir_file = cli.get<std::string>("--ir-file");

  // dbg(src, output, ir_file, opt, print_usage, print_pass, debug_mode);

  if (print_pass) {
    print_passes();
    return 0;
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
  run_passes(ir);
  if (!ir_file.empty()) {
    std::ofstream(ir_file) << *ir;
  }
  if (!output.empty()) {
    auto* code = machine_code_generation(ir);
    run_passes(code);
    std::ofstream(output) << *code;
  }

  return 0;
}
