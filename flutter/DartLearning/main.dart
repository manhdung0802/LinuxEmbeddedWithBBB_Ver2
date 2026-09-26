import 'dart:io';
import 'dart:convert';

void main() {
  print("Họ và tên bạn");
  var fullName = stdin.readLineSync(encoding: utf8)!;
  print("Nhập tuổi");
  int age = int.parse(stdin.readLineSync()!);
  print("Điểm GPA");
  double gpa = double.parse(stdin.readLineSync()!);

  print("Hello $fullName with $age");
  print("GPA ${gpa.toStringAsFixed(2)}");
}
