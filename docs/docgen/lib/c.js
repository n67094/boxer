/**
 * Applies C-specific code patterns to a generator.
 *
 * @param {object} generator - The generator to which the patterns will be applied.
 */
export function apply_to_generator(generator) {
  // Code containing "(" is treated as a function or macro,
  generator.add_code_pattern(/\(/, (t) => {
    t.header = "🔹 " + t.code;
  });

  // Code with no "(" anywhere is treated as anything else (variable, constant, type, struct, ...),
  generator.add_code_pattern(/^[^\(]*$/, (t) => {
    t.header = "🔧 " + t.code;
  });
}
