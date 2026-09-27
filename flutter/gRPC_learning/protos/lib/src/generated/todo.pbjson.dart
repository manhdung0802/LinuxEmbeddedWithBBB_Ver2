// This is a generated file - do not edit.
//
// Generated from todo.proto.

// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names
// ignore_for_file: curly_braces_in_flow_control_structures
// ignore_for_file: deprecated_member_use_from_same_package, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_relative_imports
// ignore_for_file: unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use todoDescriptor instead')
const Todo$json = {
  '1': 'Todo',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 5, '10': 'id'},
    {'1': 'title', '3': 2, '4': 1, '5': 9, '10': 'title'},
    {'1': 'completed', '3': 3, '4': 1, '5': 8, '10': 'completed'},
  ],
};

/// Descriptor for `Todo`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List todoDescriptor = $convert.base64Decode(
    'CgRUb2RvEg4KAmlkGAEgASgFUgJpZBIUCgV0aXRsZRgCIAEoCVIFdGl0bGUSHAoJY29tcGxldG'
    'VkGAMgASgIUgljb21wbGV0ZWQ=');

@$core.Deprecated('Use getTodoByIdRequestDescriptor instead')
const GetTodoByIdRequest$json = {
  '1': 'GetTodoByIdRequest',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 5, '10': 'id'},
  ],
};

/// Descriptor for `GetTodoByIdRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List getTodoByIdRequestDescriptor =
    $convert.base64Decode('ChJHZXRUb2RvQnlJZFJlcXVlc3QSDgoCaWQYASABKAVSAmlk');
