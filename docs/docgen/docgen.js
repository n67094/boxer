import fs from "fs";

import * as tokenizer from "./lib/tokenizer.js";
import * as Generator from "./lib/generator.js";
import * as pattern from "./lib/c.js";

// Get the args
const args = process.argv.slice(2);

// Default parsed args
const parsed = {
  help: false,
  line: false,
  file: false,
  output: undefined,
  input: [],
};

// Aliases for flags
const aliases = {
  h: "help",
  o: "output",
  l: "line",
  f: "file",
};

// Loop through the args and parse them
for (let i = 0; i < args.length; i++) {
  const arg = args[i];

  if (arg.startsWith("--")) {
    // Remove the --
    const key = arg.slice(2);

    // e.g: help => help or h => help
    const resolved = aliases[key] ?? key;

    if (parsed[resolved] === false || parsed[resolved] === true) {
      // It's a boolean flag
      parsed[resolved] = true;
    } else {
      // It's a value flag, take next arg as value
      parsed[resolved] = args[++i];
    }
  } else if (arg.startsWith("-")) {
    // Remove the --
    const key = arg.slice(1);

    // e.g: h => help or p => pattern
    const resolved = aliases[key] ?? key;

    if (parsed[resolved] === false || parsed[resolved] === true) {
      // It's a boolean flag
      parsed[resolved] = true;
    } else {
      // It's a value flag, take next arg as value
      parsed[resolved] = args[++i];
    }
  } else {
    // Arguments that don't start with - or -- are considered as file paths
    parsed.input.push(arg);
  }
}

// If --help or -h is provided, print help and exit
if (parsed.help) {
  console.log(
    fs
      .readFileSync(new URL("help.txt", import.meta.url))
      .toString()
      .trim(),
  );
  process.exit();
}

// The remaining arguments (those that don't start with - or --) are considered as file paths. We expect exactly one file path.
if (parsed.input.length != 1) {
  console.error("No file.");
  process.exit();
}

// Read the file content
var text = fs.readFileSync(parsed.input[0]).toString();

// Tokenize the file content
var tokens = tokenizer.parse_code(text);

console.log(`Parsed ${tokens.length} tokens from file ${parsed.input[0]}.`);

// Create a generator instance with the provided options
var generator = new Generator.Generator({
  add_line_number: parsed.line,
  add_file_header: parsed.file,
  file: parsed.input[0],
});

pattern.apply_to_generator(generator);

generator.generate(tokens);

var md = generator.doc.join("\n");

if (parsed.output) {
  fs.writeFileSync(parsed.output, md);
} else {
  console.log(md);
}
