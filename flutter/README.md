# Dart
## Biến  và kiểu dữ liệu
- Object: là kiểu như void trong C/C++
- String: chỉ rõ kiểu 
    ```Dart
    `String hello = "Hello Dung";`
    `print(hello)`
    ```
- var: không quan tâm kiểu dữ liệu
- dynamic: gán giá trị gì thì nó trở thành kiểu đó
- kiểu số (chỉ có 2 loại):
    + int (64bit)
    + double (64bit)
        - hàm làm tròn tới x chữ số sau thập phân:
            + toStringAsFixed(x) 
            + toStringAsPrecision(x)
    + num: đại diện được cho int hoặc double
    + `print('Calc $a + $b = ${a + b}');`
    + phép chia `/`: ra luôn số thập phân chứ không phải mỗi phần nguyên
    + `~/`: chia chỉ lấy nguyên
    + `%`: lấy phần dư
- bool: `bool a = true;`
- Kiểu có thể null:
    + `int? x;` mặc định được gắn null
    + `x?.isCall`
- Enum: 
    ```Dart
    enum Person {tin, hoa, dung}
    ```
- Iterable: tạo danh sách 10 phần từ 
    ```Dart
    var numbers = Iterable.generate(10);
    ```
## Các toán tử cơ bản
- `??`: check null trước khi dùng
    + `name = check ?? 'Default'`
    + nếu check null thì name là Default
    + nếu check khác null thì name = check
- `..`: `numbers..add(1)..add(2)`
    + thực hiện liên tục các function của numbers
## Nhập liệu và chuyển đổi dữ liệu
- import 'dart:io': thư viện các hàm cơ bản
    + `stdin.readLineSync()`: hàm đọc giá trị từ bàn phím
        - stdin.readLineSync(): mặc định trả về String có thể null, nên có thể để dấu `!` ở cuối. 
        - Nếu giá trị gán cho hàm này là int thì cần hàm chuyển đổi:
            - `int age = int.parse(stdin.readLineSync()!);`
- import 'dart:convert';
    + `stdin.readLineSync(encoding: utf8)!;` thêm encoding để không bị lỗi ký tự ví dụ như tiếng việt khi nhập
## Cấu trúc if-else
- tương tự C++
## Kiểu dữ liệu List - []
- giá trị trong List có thể trùng lặp
- `var numbers = [1, 2, 3, 4, 5, 6];`
- `List<String> people = ['Dung', 'manh'];`
- `var name = <String>[]` các phần từ là String
- `var name = []`: kiểu object chứa được nhiều kiểu trong mảng
- thêm phần tử: `add(x)`
- truy cập 1 phần tử: `people[x]`
## For, For-in
- `for(var i = 0; i < 10>; i++)`: nếu modify giá trị của index thì ra khỏi for sẽ đổi luôn
- `for(var peo in people)`: peo ra khỏi for sẽ không đổi giá trị nếu có modify vì peo là biến tạm ra khỏi scope sẽ mất
## Hằng số final và const
- final: giá trị có thể không xác định trong 1 khoảng thời gian và được gán khi runtime
    + `final res = call api...;` res là hằng số nhưng nó chưa xác định cho tới khi call API xong;
- const: giá trị được xác định khi biên dịch
- `var peo = const []` tương đương `const peo = []`
## Switch case
- kiểu 1:
    ```Dart
    switch(variable){
        case X: do sth;
        case Y: do sth;
        default: do sth;
    }
    ```
- kiểu 2:
    ```Dart
    var variable = abc;
    Object ret = switch(variable){
        'x' => do sth,
        'y' => do sth,
        _ => do sth,
    }
    ```
## while, do-while
    ```Dart
    //while
    while(condition){
        do sth;
    }
    //do-while
    do {
        do sth;
    } while (condition)
    ```
## continue và break
- continue: next đến lần lặp tiếp theo
- break: thoát vòng lặp
## Function
- tương tự C/C++
- kiểu rút gọn: dùng khi hàm ngắn
    ```Dart
    // có thể không cần kiểu trả về
    func(int a) => print(a);
    ```
- hàm có tham số required: bắt buộc phải cấp n
    ```Dart
    bool isDung({required int n, required int m}){}
    isDung(n: 100, m: 200); // n,m không cần đúng thứ tự
    ```
- hàm có tham số có thể null (tham số tùy chọn)
    ```Dart
    bool isDung({int? a}){} // {int? a} hoặc [int? a]
    isDung(100); 
    isDung(); // a sẽ là null
    ```
    Hoặc
    ```Dart
    bool isDung([int a = 0, int? b])
    ```
- Hàm trả về nhiều giá trị - hàm record
    ```Dart
    (String, String) day(int day) {
        return switch(day){
            1 => ('Monday`, `thu 2`),
            ...
        }
    }
    ```
    ```Dart
    (String day, String foo) day(int day) {
        return switch(day){
            1 => (day: 'Monday', foo: 'thu 2'),
            ...
        }
    }
    ```
- Hàm vô danh: hàm dùng 1 lần
    + có thể gán hàm vô danh vào 1 biến để tái sử dụng
    ```Dart
    var peo = ['dung', 'manh'];
    peo.map((e) => e.toUpperCase()).forEach(...);
    ```
- Hàm đệ quy: tương tự c/c++
## Kiểu dữ liệu record
- là kiểu vô danh, có thể nhét nhiều kiểu dữ liệu khác vào đó
    ```Dart
    var record = ('first', 200, 2.3, 'last');
    // truy cập phần tử bằng cách
    record.$1 // first
    record.$2 // 200
    // ...
    ```
- cách khai báo khác
    ```Dart
    (String, int) record;
    record = ('One', 1);
    ```
- nếu phần tử trong recore được đặt tên, thì phần tử đó không được tính trong $x nữa
    ```Dart
    var record = ('first', age: 200, 2.3, 'last');
    // truy cập phần tử bằng cách
    record.$1 // first
    record.$2 // 2.3
    recort.age // 200
    // ...
    ```
## Kiểu dữ liệu Set {}
- cho phép lưu nhiều phần tử, thường dùng lưu danh sách đơn phần tử, có nghĩa là phần tử không trùng nhau
- Set dùng ngoặc `{}`
    ```Dart
    var friend = {'Long', 'dung'}; // cách 1
    var sets = <double>{}; // cách 2
    ```
- Nếu phần tử bị lặp thì Set chỉ tính 1 lần, không phải là có 2 phần tử đó
- Nếu thêm nhiều phần tử trùng lặp thì Set chỉ tính 1 lần
## Kiểu dữ liệu Map
- biểu diễn những cái đi theo cặp
- key: bất kỳ kiểu dữ liệu nào
    ```Dart
    var map = {
        "one": "1",
        "two": "2",
    };
    map['one'];
    for(var item in dic.entries){ // entries là phần tử gồm cả key và value

    }

    var gif = Map<String, String>();
    ```
## Hàm template
- đảm bảo dữ liệu đồng nhất, an toàn
- giảm code lặp lại
    ```Dart
    void show<T>(List<T> items)
    ```
## Class 
```Dart
class Dog {
    String? color;
    String? food;

    void sua(){
        //do sth
    }
}

void main(){
    var bu = Dog();
    bu.color = 'red';
}
```
### Constructor
```Dart
class Dog{
    int x = 0;
    int y = 0;
    // Kiểu 1
    Dog(this.x, this.y); // var dog = Dog(2,3) sẽ gán x = 2, y = 3 luôn
    // Kiểu 2
    Dog({required this.x, required this.y}); // var dog = Dog(x: 1, y: 2)
    // Kiểu 3: đặt lại tên constructor
    Dog.original(): x = 0, y = 0; // var dog = Dog.original();
    Dog.fromJson(Map<String, int> map)
        : x = map['x']!,
          y = map['y']!; // var dog = Dog.fromJson({'x': 100, 'y': 200})
}
```
- Constructor `factory`
    ```Dart
    factory Song.fromJson(Map<String, dynamic> map) {}
    ```
    + constructor factory có thể chứa logic để parse data trước khi tạo object
    + nó không bắt buộc tạo ra object mới
    + dùng phổ biến với json
### Các phương thức của class
- instance method: là phương thức chỉ được gọi từ object của class (là các hàm thông thường trong class)
    ```Dart
    class Point {
        int x; 
        int y;

        // getter
        int get X => x;
        int get Y => y;
        
        // setter
        set X(int value) => x = value;
        set Y(int value) => y = value;

        // constructor
        Point(this.x, this.y);
        Point.original() : this.x = 0, this.y = 1;

        double distanceTO(Point other){
            var dx = x - other.x;
            var dy = y - other.y;
            return sqrt(dx * dx + dy * dy)
        }
    }
    var p = Point();
    p.X;
    p.X = 10;
    p.distanceTO(other);
    ```
- abstract method: phương thức trừu tượng, được khai báo trong class trừu tượng, thân hàm được khai báo trong class kế thừa nó
    ```Dart
    abstract class Calc {
        int add(int a, int b);
        int sub(int a, int b);
    }

    class Foo extends Calc {
        @override
        int add(int a, int b){

        }

        @override
        int sub(int a, int b){

        }
    }

    var foo = Foo();
    foo.add(x,y);
    ```
### Nạp chồng toán tử
    ```Dart
    class Vector {
        int x;
        int y;

        Vector(this.x, this.y);

        Vector operator +(Vector other) => Vector(x + other.x, y + other.y);
        Vector operator -(Vector other) => Vector(x - other.x, y - other.y);
    }
    Vector v1;
    Vector v2;
    Vector sum = v1 + v2; //Nhờ operator + thì mới cộng được vector như này
    ```
### Kế thừa - extends
- Constructor của con sẽ cần thêm đủ các thuộc tính như cons của cha (thuộc tính cha thì đi với `super`)
- *Class trong Dart chỉ có thể kế thừa 1 class duy nhất*
    ```Dart
    class Rec2D {
        int x;
        int y;

        Rec2D({required this.x, required this.y});

        int area(){
            return x * y;
        }
    }
    class Rec3D extends Rec2D {
        int z;

        Rec3D ({
            required super.x, // của cha
            required super.y, // của cha
            required this.z // của con
        });

        @override
        int area(){
            // return super.area()
            // return do sth
        }
    }
    ```
### Abstract class - class trừu tượng
- `abstract class Foo`
- class abstract định nghĩa các function chung, không thể tạo đối tượng riêng vì hàm chưa có thân hàm
- lớp con chỉ cần triển khai 1 số function abstract mà nó muốn
- các function abstract là kiểu kết thúc bằng dấu `;` mà không có thân hàm
    ```Dart
    abstract class Foo {
        Foo({required this.x, required this.y});
        int add(int x, int y); // abstract function không có thân hàm
    }
    class Bo extends Foo {
        int z;
        Bo({required super.x, required super.y, required this.z});
        @override
        int add(int x, int y){
            // do sth
        }
    }
    var bo = Bo(x: 1,y: 2,z: 3);
    ``` 
### Interface và abstract interface
- từ khóa `interface`, `implements`

- `interface class IFoo`
- lớp con phải triển khai lại toàn bộ biến, function abstract của lớp cha
- có thể tạo object riêng của class interface vì function của nó có thân hàm rồi
    ```Dart
    interface class IFoo {
        int x;
        int y;

        int add() => 0; //function trong class interface cần thân hàm
        int sub() => 0;
    }
    class Bo implements IFoo {
        int z;

        @override
        int x;

        @override
        int y;

        @override
        int add() => do sth;

        @override
        int sub() => do sth;
    }
    ```
- `abstract interface class Foo`
    + không tạo được đối tượng riêng
    + nếu function trong class interface không muốn định nghĩa thân hàm thì thêm `abstract` ở trước
    + các function, biến khác trong class implement class interface này vẫn cần override lại tất cả
    ```Dart
    abstract interface class IFoo {
        int x;
        int y;

        int add(); //function trong class abstract interface không cần thân hàm
        int sub();
    }
    ```
### public, private
- public: không có dấu _ trước tên
- private: có dấu _
    ```Dart
    class Foo {
        int x; // public
        int _y; // private
        void dosth() {}; // public
        void _doOhter(){}; // private
    }
    ```

## Phương thức mở rộng
- bổ sung chức năng cho các hàm từ thư viện khác, class khác
- Dùng khi thư viện đó không có chức năng mà mình muốn
    ```Dart
    // VD1
    extension MyList<T> on List<T> {
        // bổ sung chức năng cho List
        List<T> operator -() => doSth // dấu - là ví dụ thôi, là dấu hay gì cũng đc
    }
    var firned = ['a','b'];
    var foo = -firned;

    // VD2
    extension MyString on String {
        int countXXX() {
            return ...;
        }
    }
    var mess = 'Hello';
    mess.countXXX(); // gọi được hàm mà đã được mở rộng từ String
    ```

## Kiểu mixin
- dùng để cung cấp thuộc tính, function cho các class muốn dùng mà không cần kế thừa
- có thể dùng được nhiều mixin cùng lúc
- mixin không thể tạo đối tượng của nó, không hỗ trợ `extends`, không có `constructor`
    ```Dart
    mixin Pri {
        void doSth(int x){
            // doSth
        }
    }

    mixin Bro {
        int abc;
    }

    class A with Pri, Bro {
        //cách 1
        void funcname(){
            doSth(x); // gọi thẳng function của mixin
            abc = 1;
        }
        // cách 2
        @override
        void doSth(int x){ // override lại theo ý muốn

        }
    }

    var a = A();
    a.abc;
    a.doSth(x);
    ```
- abstract mixin class
    ```Dart
    abstract mixin class my {
        num add(num a); // phương thức trừu tượng
        num sub(){
            // do sth
        }
    }
    class X with my {
        @override
        num add(num a){ // bắt buộc override add
            return;
        }

        // sub override nếu có nhu cầu
    }
    ```
- `mixin Music on Person`: chỉ có những class nào kế thừa Person mới dùng mixin Music được
## Exception
- chương trình sẽ dừng khi gặp throw
    - `throw Exception('mess')`
    - `throw Error()`
    - `throw 'someth'`
    ```Dart
    // bắt dựa theo loại ngoại lệ
    try {
        // do sth
    } on Exception { // Exception hoặc Error hoặc FormatException hoặc ...
        // do sth
    } on Error { // có thể bắt nhiều ngoại lê

    } catch (e) {

    }
    
    // catch mọi ngoại lệ
    try {
        // do sth
    } catch(e, stackTrace){

    }
    ```
- Nếu 1 hàm con cần trả ngoại lệ cho hàm cha gọi nó, dùng `rethrow;`
    ```Dart
    void add(int x){
        rethrow;
    }
    ```
- `finally`: luôn chạy kể cả không xảy ra ngoại lệ nhưng chủ yếu dùng để giải phóng tài nguyên khi bị ngoại lệ
    ```Dart
    try {
        // do sth
    } catch(e, stackTrace){

    } finally {
        // dọn dẹp tài nguyên ở đây
    }
    ```
## Bất đồng bộ với future
- `await` chỉ tồn tại nếu hàm có đánh dấu `async`
    ```Dart
    // Cách 1
    Future<String> _read() async {
        var content = await file.doSth()
        return content;
    }

    void main() async {
        var foo = await _read();
        print(foo); // chờ await _read chạy xong thì dòng này mới chạy
    }

    // cách 2
    Future<String> _read() async {
        var content = await file.doSth()
        return content;
    }

    void main() { // dùng then thì k cần async nữa
        _read().then((value) => do sth);
        print(foo); // chờ await _read chạy xong thì dòng này mới chạy
    }

    ```
- khi nào hàm `await` chạy xong thì nó mới chạy tiếp các lệnh bên dưới. 
- nếu `main()` không có `await` thì main sẽ thực hiện tiếp công việc, còn hàm Future tự thực hiện bất đồng bộ đến khi xong

# Flutter
## lệnh với project Flutter: 
- Tạo project
    + `flutter create name`
    + `flutter create --org com.tencongty my_app`: chỉ định thêm package name
    + `flutter create -a java -i objc my_app`: chỉ định ngôn ngữ lập trình
    + `flutter create --platforms android,ios my_app`: chỉ định nền tảng
- Thêm dependency
    + `flutter pub add http` // thay http bằng dependencied mình muốn
## Các thư mục trong folder flutter
- lib: nơi viết code chính
- test: nơi viết testcase
- pubspec.yaml: cấu hình dependencies, font, ảnh, ...
- analysis_options.yaml: cấu hình coding convention, rule code của project
## Lớp StatefulWidget vs StatelessWidget
- StatelessWidget: không thay đổi trạng thái suốt vòng đời
    + gõ nhanh `stless`
