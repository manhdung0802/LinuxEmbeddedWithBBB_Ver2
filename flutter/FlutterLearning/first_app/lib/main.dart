import 'dart:async';

import 'package:flutter/material.dart';

void main() {
  runApp(
    MaterialApp(
      home: SafeArea(
        child: Scaffold(
          // body: Center(child: MyWidget2(false)),
          // appBar: AppBar(
          //   toolbarHeight: 30,
          //   backgroundColor: const Color.fromARGB(255, 246, 131, 123),
          //   title: Text("Manh Dung"),
          // ),
          // body: Center(child: Text("Hello world")),
          // body: Column(
          //   children: [
          //     MyWidget(false),
          //     MyWidget2(false),
          //     MyWidget3(),
          //     MyContainer(),
          //   ],
          // ),
          body: MyContainer(),
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
    return loading
        ? CircularProgressIndicator()
        : RichText(
            text: TextSpan(
              style: DefaultTextStyle.of(context).style,
              children: [
                TextSpan(text: 'Hello'),
                TextSpan(text: 'Dung'),
              ],
            ),
          );
  }
}

class MyContainer extends StatelessWidget {
  const MyContainer({super.key});

  @override
  Widget build(BuildContext context) {
    return Container(
      color: Colors.yellow,
      width: 200,
      height: 200,
      alignment: Alignment.center,
      transform: Matrix4.rotationZ(0.2),
      child: Text('Container'),
    );
  }
}

class MyWidget3 extends StatelessWidget {
  const MyWidget3({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      color: Colors.pink,
      margin: EdgeInsets.all(20),
      child: Container(
        child: Column(
          spacing: 10,
          children: [
            Padding(
              padding: const EdgeInsets.fromLTRB(
                10.0,
                20,
                30,
                40,
              ), // cách 20px đều 4 hướng
              child: Text(
                'Manh Dung',
                style: TextStyle(fontSize: 20, color: Colors.white),
              ),
            ),
            ElevatedButton(
              onPressed: () => print('Pressed'),
              style: ElevatedButton.styleFrom(backgroundColor: Colors.green),
              child: Text('Download', style: TextStyle(fontSize: 20)),
            ),
          ],
        ),
      ),
    );
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
    // gọi sau khi khởi tạo MyWidget2, trước hàm build, dùng để khởi tạo giá trị ban đầu
    _localLoading = widget.loading;
  }

  @override
  Widget build(BuildContext context) {
    return _localLoading
        ? CircularProgressIndicator()
        : FloatingActionButton(
            onPressed: (onClickButton),
            child: Text(
              "Button",
              style: TextStyle(color: Colors.pink, fontSize: 15),
            ),
          );
  }

  void onClickButton() {
    setState(() {
      // load lại state mới và chạy lại hàm build
      _localLoading = true;
      Timer(Duration(seconds: 3), finishedLoading);
    });
  }

  void finishedLoading() {
    setState(() {
      _localLoading = false;
    });
  }
}
