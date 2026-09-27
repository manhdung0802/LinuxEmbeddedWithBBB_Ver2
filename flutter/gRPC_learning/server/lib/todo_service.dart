import 'dart:math';

import 'package:protos/protos.dart';

class TodoService extends TodoServiceBase {
  @override
  Future<Todo> getTodo(ServiceCall call, GetTodoByIdRequest request) async {
    final id = request.id;
    final todo = Todo(id: id, title: 'title $id', completed: false);
    return todo;
  }

  @override
  Stream<Todo> getTodoStream(
    ServiceCall call,
    GetTodoByIdRequest request,
  ) async* {
    while (true) {
      final id = Random().nextInt(100);
      final todo = Todo(id: id, title: 'title $id', completed: false);

      yield todo;
      await Future.delayed(Duration(seconds: 1));
    }
  }
}
