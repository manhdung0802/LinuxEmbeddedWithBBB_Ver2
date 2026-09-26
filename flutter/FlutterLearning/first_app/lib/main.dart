import 'package:flutter/material.dart';

void main() {
  runApp(
    MaterialApp(
      home: SafeArea(
        child: Scaffold(
          body: Center(child: MyWidget2(false)),
          // appBar: AppBar(
          //   toolbarHeight: 30,
          //   backgroundColor: const Color.fromARGB(255, 246, 131, 123),
          //   title: Text("Manh Dung"),
          // ),
          // body: Center(child: Text("Hello world")),
        ),
      ),
      debugShowCheckedModeBanner: false,
    ),
  ); // khởi động app
}

class MyWidget extends StatelessWidget {
  final bool loading;

  const MyWidget(this.loading, {super.key});

  @override
  Widget build(BuildContext context) {
    return loading ? CircularProgressIndicator() : Text('State');
  }
}

class MyWidget2 extends StatefulWidget {
  final bool loading;

  const MyWidget2(this.loading, {super.key});

  @override
  State<StatefulWidget> createState() {
    return MyWidget2State();
  }
}

class MyWidget2State extends State<MyWidget2> {
  late bool _localLoading;
  @override
  void initState() {
    super.initState();
    // gọi sau khi khởi tạo MyWidget2, trước hàm build
    _localLoading = widget.loading;
  }

  @override
  Widget build(BuildContext context) {
    return _localLoading
        ? CircularProgressIndicator()
        : FloatingActionButton(onPressed: (onClickButton));
  }

  void onClickButton() {
    setState(() {
      // load lại state mới và chạy lại hàm build
      _localLoading = true;
    });
  }
}
