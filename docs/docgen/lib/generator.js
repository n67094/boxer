/**
 * # GENERATOR
 *
 * Turns documentation blocks produced by the tokenizer into markdown lines.
 */

/**
 * Builds an HTML anchor.
 *
 * @param {string} anchor - Anchor name.
 *
 * @returns {string} - e.g. <a name="line-42"></a>
 */
function get_html_anchor(anchor) {
  return `<a name="${anchor}"></a>`;
}

/**
 * Turns the doc blocks produced by the tokenizer into markdown lines.
 */
export class Generator {
  /**
   * @param {object} options
   * @param {boolean} options.add_line_number - Append line N after each code line.
   * @param {boolean} options.add_file_header - Add a note "> This document was generated from file `path`" at the top.
   * @param {string} options.file - Source file path, shown in the file header.
   */
  constructor(options) {
    this.options = options;

    // Heading depth of the current section ("## Foo" -> 2), set by absolute headings
    this.level = 0;
    // True while inside a ``` fenced block in the comment text (headings are not touched there)
    this.code_block = false;
    // Output markdown, one entry per line
    this.doc = [];
    // { pattern, callback } pairs registered with add_code_pattern
    this.code_patterns = [];
  }

  /**
   * Generates the whole document from the tokenizer output into `this.doc`.
   *
   * @param {object[]} tokens - Doc blocks returned by the tokenizer.
   */
  generate(tokens) {
    // Optional note at the top: > This document was generated from file `path`
    if (this.options.add_file_header) {
      this.add_line(
        `> This document was generated from file \`${this.options.file}\``,
      );
    }

    for (const t of tokens) {
      this.add_token(t);
    }
  }

  /**
   * Registers a pattern tested against each token's code line.
   * Patterns are tried in registration order; only the first match runs.
   *
   * @param {RegExp} pattern - Regex tested against `token.code`.
   * @param {(token: object) => void} fn - Called with the token; `this` is the generator.
   */
  add_code_pattern(pattern, fn) {
    this.code_patterns.push({
      pattern: pattern,
      callback: fn,
    });
  }

  /**
   * Runs the callback of the first registered pattern matching `token.code`.
   *
   * @param {object} token - The doc block to process.
   */
  apply_code_pattern(token) {
    for (const p of this.code_patterns) {
      if (p.pattern.test(token.code)) {
        // .call(this, ...) makes `this` inside the callback the generator
        p.callback.call(this, token);
        // Stop at the first match
        return;
      }
    }
  }

  /**
   * Renders one doc block.
   *
   * @param {object} token - The doc block to render.
   */
  add_token(token) {
    // Every block gets an anchor from its line number: <a name="line-42"></a>
    let anchors = get_html_anchor("line-" + token.textLine);

    // Plus a named anchor if the comment started with "(name)", e.g. /** (vec-add)
    if (token.anchor) {
      anchors += get_html_anchor(token.anchor);
    }

    this.add_line(anchors);

    if (token.code) {
      this.apply_code_pattern(token);
      this.add_code_line(token);
      // this.add_sub_header(token.header || token.code);
      this.add_lines(token.text, true);
      this.add_line("---");
      this.add_line("");
    } else {
      // Free text block: copied as-is, its headings set this.level
      this.add_lines(token.text);
    }

    this.add_line("");
  }

  /**
   * Adds a heading at the current level. Currently unused.
   *
   * @param {string} text - Heading text.
   */
  add_header(text) {
    this.add_line("#".times(this.level) + " " + text);
  }

  /**
   * Adds a heading one level below the current section,
   *
   * @param {string} text - Heading text.
   */
  add_sub_header(text) {
    this.add_line("#".repeat(this.level + 1) + " " + text);
  }

  // Appends one raw line to the output; every other add* method goes through here
  add_line(line) {
    this.doc.push(line);
  }

  /**
   * Adds the token's code line as a ```cpp fenced block, surrounded by blank lines.
   *
   * @param {object} token - Doc block with `code` and `code_line`.
   */
  add_code_line(token) {
    this.add_line("");
    this.add_line("```cpp");

    if (this.options.add_line_number) {
      this.add_line(token.code + ` /* line ${token.code_line} */`);
    } else {
      this.add_line(token.code);
    }

    this.add_line("```");
    this.add_line("");
  }

  /**
   * Adds one line of comment text, adjusting headings.
   *
   * @param {string} text - The comment line.
   * @param {boolean} relative - true: nest headings under the current level.
   *                             false: keep headings and update the current level.
   */
  add_text_line(text, relative) {
    // A ``` line opens or closes a fenced block inside the comment text
    if (text.startsWith("```")) {
      this.codeBlock = !this.codeBlock;
    }

    // Inside a fenced block, text is copied as-is, no heading adjustment
    if (this.codeBlock) {
      this.add_line(text);
      return;
    }

    // Headings are only adjusted outside fenced blocks
    if (text.startsWith("#")) {
      if (relative) {
        // Nest the heading under the current level, e.g. level 2 + "## Foo" -> "### Foo"
        this.add_line("#".repeat(this.level + 1) + text);
      } else {
        // Count the number of "#" to set the current section level, e.g. "## Foo" -> 2
        this.level = text.match(/^#+/)[0].length;
        this.add_line(text);
      }
    } else {
      // Normal text line
      this.add_line(text);
    }
  }

  /**
   * Adds several comment lines.
   *
   * @param {string[]} lines - The comment lines.
   * @param {boolean} relative - Passed to addTextLine.
   */
  add_lines(lines, relative) {
    for (const l of lines) {
      this.add_text_line(l, relative);
    }
  }
}
