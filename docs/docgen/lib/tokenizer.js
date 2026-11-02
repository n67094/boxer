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
 *
 * @returns {string[]} - The raw lines of the declaration.
 */
function collect_declaration(next_line, has_more) {
  let out = [];
  let depth = 0;

  // Collect lines until we reach the end of the declaration
  while (has_more()) {
    let raw = next_line();
    let l = raw.trim();

    // Break if we reach an empty line and are not inside a block (depth 0)
    if (l === "" && depth === 0) break;
    out.push(raw);

    // If the line starts with a comment, we ignore it and continue to the next line
    if (out[0].trim().startsWith("#")) {
      if (!l.endsWith("\\")) break;
      continue;
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
    };

    // Check for anchor in the first line of the comment block
    // An anchor is defined as a string inside parentheses, e.g. /** (anchor) */
    let m = l.match(/\(([^\(]+)\)/);
    if (m) {
      block.anchor = m[1];
    }

    // Collect the text lines of the documentation block until we reach the end of the comment block
    while ((current_line = next_line())) {
      l = current_line.trim();
      if (l.startsWith("*/") || l.startsWith("**/")) break;
      block.text.push(trim_text(l));
    }

    // Collect the code lines associated with the documentation block
    block.code_line = line_count + 1;
    block.code = trim_code(
      collect_declaration(next_line, () => lines.length > 0).join("\n"),
    );

    // Store the collected documentation block in the array
    doc_blocks.push(block);
  }

  return doc_blocks;
}
