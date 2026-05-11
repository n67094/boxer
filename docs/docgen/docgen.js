const fs = require("fs");
const path = require("path");

const tokenizer = require("./lib/tokenizer.js");
const generator = require("./lib/generator.js");
const pattern = require(`./lib/c.js`);

// process.argv contains: ['node', 'script.js', ... and args]
// We slice from index 2 to get only the user-provided args
// e.g: "node script.js -h => args = ['-h']
const args = process.argv.slice(2);

const parsed = {
  help: false,
  line: false,
  file: false,
  output: undefined,
  input: [],
};

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
      .readFileSync(__dirname + "/help.txt")
      .toString()
      .trim(),
  );
  return;
}

// The remaining arguments (those that don't start with - or --) are considered as file paths. We expect exactly one file path.
if (parsed.input.length != 1) {
  console.error("No file.");
  return;
}

// Read the file content
var text = fs.readFileSync(parsed.input[0]).toString();

// Tokenize the file content
var tokens = tokenizer.parseCode(text);

console.log(`Parsed ${tokens.length} tokens from file ${parsed.input[0]}.`);

// Create a generator instance with the provided options
var gen = new generator.Generator({
  addLineNumber: parsed.line,
  addFileHeader: parsed.file,
  file: parsed.input[0],
});

pattern.applyToGenerator(gen);

gen.generate(tokens);

var md = gen.doc.join("\n");

if (parsed.output) {
  fs.writeFileSync(parsed.output, md);
} else {
  console.log(md);
}
