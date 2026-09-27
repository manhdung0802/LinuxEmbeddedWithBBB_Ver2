import 'dart:io';

import 'package:protos/protos.dart';
import 'package:server/todo_service.dart';

void main(List<String> arguments) async {
  final server = Server.create(services: [TodoService()]);
  final port = int.parse(Platform.environment['PORT'] ?? '8080');
  await server.serve(port: port);
  print('Server listening on port ${server.port}');
}
