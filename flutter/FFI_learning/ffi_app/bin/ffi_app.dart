import 'dart:ffi';

import 'package:ffi_app/ffi_app.dart' as ffi_app;

void main(List<String> arguments) {
  print('Hello world: ${ffi_app.calculate()}!');
  final dylib = DynamicLibrary.open('lib/libhello.so');
  final hello = dylib.lookupFunction<Void Function(), void Function()>('hello');
  hello();

  final add = dylib
      .lookupFunction<Int32 Function(Int32, Int32), int Function(int, int)>(
        'add',
      );
  final ret = add(2, 3);
  print(ret);
}
