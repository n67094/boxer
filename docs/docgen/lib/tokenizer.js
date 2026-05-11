function parseCode(text) {
  var lines = text.split("\n");

  var line;
  var lineNo = 0;
  var blocks = [];

  function nextLine() {
    lineNo++;
    return lines.shift();
  }

  while (lines.length > 0) {
    line = nextLine();
    var l = line.trim();

    if (!l.startsWith("/**")) {
      continue;
    }

    var block = {
      text: [],
      textLine: lineNo,
    };

    var m = l.match(/\(([^\(]+)\)/); // anchor

    if (m) {
      block.anchor = m[1];
    }

    while ((line = nextLine())) {
      l = line.trim();
      if (l.startsWith("*/") || l.startsWith("**/")) break;
      block.text.push(trimText(l));
    }

    block.code = trimCode(nextLine());
    block.codeLine = lineNo;

    blocks.push(block);
  }

  return blocks;
}

function trimCode(code) {
  if (!code) return code;

  return trimRight(code.trim(), ";").trim();
}

function trimRight(str, char) {
  while (str.at(-1) === char) str = str.slice(0, -1);
  return str;
}

function trimText(text) {
  if (!text) return text;

  text = text.trim();

  if (text.startsWith("* ")) {
    text = text.substr(2);
  }

  if (text == "*") {
    text = "";
  }

  return text;
}

exports.parseCode = parseCode;
