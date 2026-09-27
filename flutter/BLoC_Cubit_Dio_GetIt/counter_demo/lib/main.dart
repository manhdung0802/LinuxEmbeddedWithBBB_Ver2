import 'package:flutter/material.dart';
import 'di/service_locator.dart';
import 'presentation/counter_page.dart';

void main() {
  // 1. Đăng ký tất cả dependencies vào GetIt trước khi chạy app
  setupLocator();

  // 2. Chạy app
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Cubit + GetIt Demo',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.indigo),
        useMaterial3: true,
      ),
      home: const CounterPage(),
    );
  }
}
