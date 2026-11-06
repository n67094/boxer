/**
 * # Tokenizer
 *
 * Parse the code into documentation blocks.
 */

/**
 * Trims the specified character from the right end of the string.
 *
 * @param {string} str - The string to be trimmed.
 * @param {string} char - The character to be trimmed from the right end of the string.
 *
 * @returns {string} - The trimmed string.
 */
function trim_right(str, char) {
  while (str.at(-1) === char) str = str.slice(0, -1);
  return str;
}

/**
 * Trims the code string by removing leading and trailing whitespace, as well as any trailing semicolons.
 *
 * @param {string} code - The code string to be trimmed.
 *
 * @returns {string} - The trimmed code string.
 */
function trim_code(code) {
  if (!code) return code;

  return trim_right(code.trim(), ";").trim();
}

/**
 * Trims the text string by removing leading and trailing whitespace, as well as any leading "* " characters.
 *
 * @param {string} text - The text string to be trimmed.
 *
 * @returns {string} - The trimmed text string.
 */
function trim_text(text) {
  if (!text) return text;

  text = text.trim();

  if (text.startsWith("* ")) {
    text = text.slice(2);
  }

  if (text == "*") {
    text = "";
  }

  return text;
}

/**
 * Collects the lines of a C declaration starting at the current position.
 *
 * @param {Function} next_line - Returns the next line, or undefined at EOF.
 * @param {Function} has_more - Returns true while lines remain.
 * @param {Function} lines_peek - Returns the next line without consuming it.
 *
 * @returns {string[]} - The raw lines of the declaration.
 */
function collect_declaration(next_line, has_more, lines_peek) {
  let out = [];
  let depth = 0;

  // Collect lines until we reach the end of the declaration
  while (has_more()) {
    let raw = next_line();
    let l = raw.trim();

    // Break if we reach an empty line and are not inside a block (depth 0)
    if (l === "" && depth === 0) break;
    out.push(raw);

    // Preprocessor lines: follow "\" continuations, and keep consecutive
    // #define lines together so one doc block can cover a group of macros
    if (out[0].trim().startsWith("#")) {
      if (l.endsWith("\\")) continue;
      if (!out[0].trim().startsWith("#define")) break;
      if (lines_peek()?.trim().startsWith("#define")) continue;
      break;
    }

    // Update the depth based on the number of opening and closing braces and parentheses
    for (const c of l) {
      if (c === "{" || c === "(") depth++;
      else if (c === "}" || c === ")") depth--;
    }

    // If we are at depth 0 and the line ends with a semicolon or closing brace, we have reached the end of the declaration
    if (depth <= 0 && (l.endsWith(";") || l.endsWith("}"))) break;
  }

  return out;
}

/**
 * Checks if the declaration lines are a function declaration (prototype or function pointer typedef).
 * Macros and declarations with a body (struct, enum, union, inline function...) are not.
 *
 * @param {string[]} lines - The raw lines of the declaration.
 *
 * @returns {boolean} - True if the declaration is a function declaration.
 */
function is_function_declaration(lines) {
  if (lines.length === 0) return false;
  if (lines[0].trim().startsWith("#")) return false;

  let code = lines.join("\n");

  // Line comments would swallow the rest of the line once joined
  if (code.includes("//")) return false;

  return code.includes("(") && !code.includes("{");
}

/**
 * Joins the declaration lines into a single line.
 * e.g. "int f(int a,\n      int b);" => "int f(int a, int b);"
 *
 * @param {string[]} lines - The raw lines of the declaration.
 *
 * @returns {string} - The declaration on one line.
 */
function join_on_one_line(lines) {
  return lines
    .map((line) => line.trim())
    .filter((line) => line !== "")
    .join(" ")
    .replace(/\*\s+(?=\w)/g, "*")
    .replace(/\(\s+/g, "(")
    .replace(/\s+\)/g, ")");
}

/**
 * Shortens one #define line for display.
 *
 * @param {string} line - A "#define ..." line, continuation lines already joined.
 *
 * @returns {string} - The line to display.
 */
function summarize_macro(line) {
  let m = line.trim().match(/^#\s*define\s+(\w+)(\([^)]*\))?\s*(.*)$/s);
  if (!m) return line.trim();

  let [, name, params, value] = m;

  // Function-like: "(" right after the name
  if (params) {
    return `#define ${name}${params}`;
  }

  // Drop a trailing line comment
  value = value.replace(/\s*\/\/.*$/, "").trim();

  const simple =
    value.length <= 40 &&
    !value.includes("\\") &&
    /^[\w\s"().+\-*/<|]*$/.test(value);

  if (value === "" || !simple) {
    return `#define ${name}`;
  }

  return `#define ${name} ${value}`;
}

/**
 * Shortens a group of #define lines, following "\" continuations.
 *
 * @param {string[]} lines - The raw lines of the declaration.
 *
 * @returns {string} - One summarized line per macro.
 */
function summarize_macros(lines) {
  let out = [];
  let current = "";

  for (const raw of lines) {
    let l = raw.trim();
    if (l.endsWith("\\")) {
      current += l.slice(0, -1) + " ";
      continue;
    }
    current += l;
    out.push(summarize_macro(current));
    current = "";
  }

  if (current) out.push(summarize_macro(current));

  return out.join("\n");
}

/**
 * Checks if the declaration lines are a function definition with a body, as opposed to a struct, enum or union.
 *
 * @param {string[]} lines - The raw lines of the declaration.
 *
 * @returns {boolean} - True if the declaration is a function definition.
 */
function is_function_definition(lines) {
  if (lines.length === 0) return false;
  if (lines[0].trim().startsWith("#")) return false;

  let code = lines.join("\n");
  let brace = code.indexOf("{");
  if (brace < 0) return false;

  let head = code.slice(0, brace);
  if (/^\s*(typedef|struct|enum|union)\b/.test(head)) return false;

  return head.includes("(");
}

/**
 * Parses the provided code text and extracts documentation blocks along with their associated code lines.
 *
 * @param {string} text - The code text to be parsed.
 *
 * @returns {Array} - An array of documentation blocks, each containing the text, anchor (if any), code, and line numbers.
 */
export function parse_code(text) {
  let lines = text.split("\n");

  let current_line;
  let line_count = 0;
  let doc_blocks = []; // Store collected documentation blocks

  function next_line() {
    line_count++;
    return lines.shift();
  }

  // Iterate through the lines of code to find documentation blocks
  while (lines.length > 0) {
    current_line = next_line();

    // Check for documentation block start
    let l = current_line.trim();
    if (!l.startsWith("/**")) {
      continue;
    }

    // Create a documentation block to store data
    let block = {
      text: [],
      code: [],
      text_line: line_count,
    };

    // Check for anchor in the first line of the comment block
    // An anchor is defined as a string inside parentheses, e.g. /** (anchor) */
    let m = l.match(/\(([^\(]+)\)/);
    if (m) {
      block.anchor = m[1];
    }

    // The "@self" tag marks a declaration whose name is self-explanatory: it is
    // documented without a description, e.g. /** @self */
    if (/(^|\s)@self(\s|\*\/|$)/.test(l.slice(3))) {
      block.self = true;
    }

    // One-line comment, e.g. /** @self */: no text lines to collect
    if (!l.slice(3).includes("*/")) {
      // Collect the text lines of the documentation block until we reach the end of the comment block
      while ((current_line = next_line())) {
        l = current_line.trim();
        if (l.startsWith("*/") || l.startsWith("**/")) break;
        block.text.push(trim_text(l));
      }
    }

    // "@self" may also be on its own line inside the comment block
    if (block.text.some((t) => t === "@self")) {
      block.self = true;
      block.text = block.text.filter((t) => t !== "@self");
    }

    // Collect the code lines associated with the documentation block
    block.code_line = line_count + 1;
    let declaration = collect_declaration(
      next_line,
      () => lines.length > 0,
      () => lines[0],
    );

    // Functions are shown on one line, macros without their body, other code
    // blocks are kept as is
    if (is_function_declaration(declaration)) {
      block.code = trim_code(join_on_one_line(declaration));
    } else if (is_function_definition(declaration)) {
      let code = declaration.join("\n");
      block.code = trim_code(
        join_on_one_line(code.slice(0, code.indexOf("{")).split("\n")),
      );
    } else if (declaration[0]?.trim().startsWith("#define")) {
      block.code = summarize_macros(declaration);
    } else {
      block.code = trim_code(declaration.join("\n"));
    }

    // Store the collected documentation block in the array
    doc_blocks.push(block);
  }

  return doc_blocks;
}
