// This is a generated file - do not edit.
//
// Generated from todo.proto.

// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names
// ignore_for_file: curly_braces_in_flow_control_structures
// ignore_for_file: deprecated_member_use_from_same_package, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_relative_imports

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

export 'package:protobuf/protobuf.dart' show GeneratedMessageGenericExtensions;

class Todo extends $pb.GeneratedMessage {
  factory Todo({
    $core.int? id,
    $core.String? title,
    $core.bool? completed,
  }) {
    final result = Todo._();
    if (id != null) result.id = id;
    if (title != null) result.title = title;
    if (completed != null) result.completed = completed;
    return result;
  }

  Todo._();

  factory Todo.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      Todo()..mergeFromBuffer(data, registry);
  factory Todo.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      Todo()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'Todo',
      createEmptyInstance: Todo.$_createMessage)
    ..aI(1, _omitFieldNames ? '' : 'id')
    ..aOS(2, _omitFieldNames ? '' : 'title')
    ..aOB(3, _omitFieldNames ? '' : 'completed')
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  Todo clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  Todo copyWith(void Function(Todo) updates) =>
      super.copyWith((message) => updates(message as Todo)) as Todo;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  @$core.Deprecated('Use Todo() / Todo.new instead')
  static Todo create() => Todo._();
  static $pb.GeneratedMessage $_createMessage() => Todo._();
  @$core.override
  Todo createEmptyInstance() => Todo._();
  @$core.pragma('dart2js:noInline')
  static Todo getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<Todo>(Todo.$_createMessage);
  static Todo? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => $_clearField(1);

  @$pb.TagNumber(2)
  $core.String get title => $_getSZ(1);
  @$pb.TagNumber(2)
  set title($core.String value) => $_setString(1, value);
  @$pb.TagNumber(2)
  $core.bool hasTitle() => $_has(1);
  @$pb.TagNumber(2)
  void clearTitle() => $_clearField(2);

  @$pb.TagNumber(3)
  $core.bool get completed => $_getBF(2);
  @$pb.TagNumber(3)
  set completed($core.bool value) => $_setBool(2, value);
  @$pb.TagNumber(3)
  $core.bool hasCompleted() => $_has(2);
  @$pb.TagNumber(3)
  void clearCompleted() => $_clearField(3);
}

class GetTodoByIdRequest extends $pb.GeneratedMessage {
  factory GetTodoByIdRequest({
    $core.int? id,
  }) {
    final result = GetTodoByIdRequest._();
    if (id != null) result.id = id;
    return result;
  }

  GetTodoByIdRequest._();

  factory GetTodoByIdRequest.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      GetTodoByIdRequest()..mergeFromBuffer(data, registry);
  factory GetTodoByIdRequest.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      GetTodoByIdRequest()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'GetTodoByIdRequest',
      createEmptyInstance: GetTodoByIdRequest.$_createMessage)
    ..aI(1, _omitFieldNames ? '' : 'id')
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  GetTodoByIdRequest clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  GetTodoByIdRequest copyWith(void Function(GetTodoByIdRequest) updates) =>
      super.copyWith((message) => updates(message as GetTodoByIdRequest))
          as GetTodoByIdRequest;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  @$core.Deprecated('Use GetTodoByIdRequest() / GetTodoByIdRequest.new instead')
  static GetTodoByIdRequest create() => GetTodoByIdRequest._();
  static $pb.GeneratedMessage $_createMessage() => GetTodoByIdRequest._();
  @$core.override
  GetTodoByIdRequest createEmptyInstance() => GetTodoByIdRequest._();
  @$core.pragma('dart2js:noInline')
  static GetTodoByIdRequest getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<GetTodoByIdRequest>(
          GetTodoByIdRequest.$_createMessage);
  static GetTodoByIdRequest? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => $_clearField(1);
}

const $core.bool _omitFieldNames =
    $core.bool.fromEnvironment('protobuf.omit_field_names');
const $core.bool _omitMessageNames =
    $core.bool.fromEnvironment('protobuf.omit_message_names');
