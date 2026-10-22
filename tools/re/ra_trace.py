import argparse
import os
import sys
import threading
import time
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
COMPILER = REPO_ROOT / "tools" / "local" / "compilers" / "Wii" / "1.1" / "mwcceppc.exe"
GAME_FLAGS = [
    "-nodefaults", "-proc", "gekko", "-align", "powerpc", "-enum", "int", "-fp", "hardware",
    "-O4,p", "-inline", "auto", "-pragma", "cats off", "-pragma", "warn_notinlined off",
    "-maxerrors", "1", "-nosyspath", "-str", "reuse", "-DNDEBUG=1",
    "-Cpp_exceptions", "on", "-RTTI", "on", "-fp_contract", "off", "-ipa", "file", "-lang=c++",
]

HOOK = r"""
var base = Process.getModuleByName('mwcceppc.exe').base;
function A(x) { return base.add(x - 0x400000); }
function hashName(p) { try { return p.isNull() ? '?' : p.add(10).readCString(); } catch (e) { return '?'; } }
function objectName(o) { try { return o.isNull() ? '-' : hashName(o.add(12).readPointer()); } catch (e) { return '?'; } }
var CLASSES = ['spr', 'cr', 'v', 'f', 'r'];
var OPCODES = A(0x6F1B9A);
var availableRegisters = new NativeFunction(A(0x5B6570), 'int', ['int'], 'mscdecl');
var current = '';
function wanted() { return FILTER === '' || current === FILTER; }
function opcodeName(op) { try { return OPCODES.add(22 * op).readPointer().readCString(); } catch (e) { return 'op' + op; } }
function dump(title) {
  var out = ['CODE ' + title];
  for (var b = A(0x77520C).readPointer(); !b.isNull(); b = b.readPointer()) {
    out.push(' B' + b.add(28).readS32());
    for (var p = b.add(20).readPointer(); !p.isNull(); p = p.readPointer()) {
      var count = p.add(42).readS16();
      var parts = [];
      for (var i = 0; i < count; i++) {
        var a = p.add(44 + 14 * i);
        var kind = a.readU8();
        if (kind === 0) {
          var cls = a.add(1).readU8();
          var effect = a.add(2).readU16();
          var reg = a.add(4).readS16();
          if (reg < 32 && (effect & 2) && opcodeName(p.add(40).readS16()).charAt(0) === 'B') continue;
          parts.push((CLASSES[cls] || 'c' + cls) + reg + ((effect & 2) ? '=' : ''));
        } else if (kind === 1) {
          parts.push('#' + a.add(2).readS32());
        } else if (kind === 2) {
          parts.push('[' + objectName(a.add(2).readPointer()) + ']');
        }
      }
      out.push('   ' + opcodeName(p.add(40).readS16()) + ' ' + parts.join(','));
    }
  }
  send(out.join('\n'));
}
Interceptor.attach(A(0x46BC70), { onEnter: function (args) {
  try { current = objectName(args[0].add(4).readPointer()); } catch (e) { current = '?'; }
  if (wanted()) send('FUNCTION ' + current);
} });
Interceptor.attach(A(0x5BB930), { onEnter: function (args) {
  if (!wanted()) return;
  var post = args[0].toInt32() & 0xFF;
  if (!post && DUMP >= 2) dump('before pre-allocation scheduling');
  if (post && DUMP >= 3) dump('before final scheduling');
} });
Interceptor.attach(A(0x5B8E20), { onEnter: function (args) {
  if (wanted() && DUMP >= 1) dump('allocation input');
} });
Interceptor.attach(A(0x5B9120), {
  onEnter: function (args) { this.head = args[0]; this.cls = A(0x797480).readU8(); this.k = wanted() ? availableRegisters(this.cls) : 0; },
  onLeave: function (result) {
    if (!wanted()) return;
    var cls = this.cls;
    var name = CLASSES[cls] || 'c' + cls;
    var out = [];
    for (var n = this.head; !n.isNull(); n = n.readPointer()) {
      var color = n.add(18).readS16();
      out.push('  ' + name + n.add(16).readS16() + ' -> ' + (color < 0 ? 'spilled' : name + color) + '  ' + objectName(n.add(4).readPointer()) + '  degree ' + n.add(24).readS16());
    }
    send('COLOR ' + name + ' K=' + this.k + (result.toInt32() ? '' : ' (spill, retried)') + ' in colouring order\n' + out.join('\n'));
  }
});
"""


def main():
    parser = argparse.ArgumentParser(description="Trace CodeWarrior Wii/1.1 register allocation for one translation unit.")
    parser.add_argument("source", help="C++ source file to compile")
    parser.add_argument("-f", "--function", default="", help="unqualified function name to report (default: all)")
    parser.add_argument("-d", "--dump", type=int, default=0, help="1: code entering allocation, 2: also before pre-allocation scheduling, 3: also before final scheduling")
    parser.add_argument("-i", "--include", action="append", default=[], help="extra include directory (searched first)")
    parser.add_argument("--no-classes", default="spr,cr", help="register classes to omit from the report")
    args = parser.parse_args()

    import frida

    source = Path(args.source).resolve()
    includes = [str(Path(i).resolve()) for i in args.include]
    includes += [str(source.parent), str(REPO_ROOT / "include"), str(REPO_ROOT / "build" / "wpse-v1" / "include")]
    flags = list(GAME_FLAGS)
    for inc in includes:
        flags += ["-i", inc]
    out = Path(os.environ.get("TEMP", "/tmp")) / ("ra_trace_%d.o" % os.getpid())
    command = [str(COMPILER)] + flags + ["-c", str(source), "-o", str(out)]
    script_text = "var DUMP = %d;\nvar FILTER = %r;\n" % (args.dump, args.function) + HOOK
    skip = set(c for c in args.no_classes.split(",") if c)
    messages = []
    done = threading.Event()

    def on_message(message, data):
        if message["type"] == "send":
            text = message["payload"]
            if text.startswith("COLOR ") and text.split()[1] in skip:
                return
            messages.append(text)
        else:
            messages.append("ERROR " + str(message))

    pid = frida.spawn(command)
    session = frida.attach(pid)
    session.on("detached", lambda *a: done.set())
    script = session.create_script(script_text)
    script.on("message", on_message)
    script.load()
    frida.resume(pid)
    done.wait(600)
    time.sleep(0.2)
    if out.exists():
        out.unlink()
    sys.stdout.buffer.write(("\n".join(messages) + "\n").encode("utf-8", "replace"))


if __name__ == "__main__":
    main()
