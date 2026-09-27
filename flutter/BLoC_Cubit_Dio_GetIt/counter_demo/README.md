# counter_demo

Project demo đơn giản minh hoạ cách dùng **Cubit**, **BLoC pattern** và **GetIt** trong Flutter.

## Chạy project

```bash
DISPLAY=:0 flutter run --no-enable-impeller
```

---

## Các khái niệm

### GetIt — Dependency Injection / Service Locator

- Là một **container toàn cục** chứa các object (dependency)
- Giống như **singleton**: tạo object 1 lần, lấy ra ở bất kỳ đâu trong app mà không cần truyền qua constructor
- Tránh việc phải truyền object qua nhiều tầng widget (prop drilling)

```dart
// Đăng ký (chỉ gọi 1 lần lúc khởi động app)
getIt.registerSingleton<CounterCubit>(CounterCubit());

// Lấy ra ở bất kỳ đâu
getIt<CounterCubit>().increment();
```

| Method | Ý nghĩa |
|---|---|
| `registerSingleton<T>(instance)` | Tạo sẵn 1 instance, dùng mãi |
| `registerLazySingleton<T>(() => T())` | Tạo khi lần đầu được gọi, sau đó dùng lại |
| `registerFactory<T>(() => T())` | Tạo mới mỗi lần gọi |
| `getIt<T>()` | Lấy instance đã đăng ký |

---

### Cubit — Quản lý State

- Là phiên bản **đơn giản hơn BLoC** — không cần Event
- Hoạt động như **state machine**: nhận action → xử lý → emit state mới
- UI lắng nghe state và tự rebuild khi có thay đổi

```
UI gọi method  →  Cubit xử lý  →  emit(newState)  →  UI rebuild
```

```dart
class CounterCubit extends Cubit<CounterState> {
  CounterCubit() : super(CounterState(count: 0)); // state ban đầu

  void increment() {
    emit(CounterState(count: state.count + 1)); // phát state mới
    // emit() = thông báo cho tất cả BlocBuilder đang lắng nghe
  }
}
```

---

### BLoC — Business Logic Component

- Là pattern kiến trúc tách biệt **UI** khỏi **logic**
- **BLoC** đầy đủ: UI gửi **Event** → BLoC xử lý → emit **State**
- **Cubit** là rút gọn của BLoC: UI gọi **method** trực tiếp thay vì gửi Event

```
BLoC đầy đủ:  UI → add(Event) → BLoC → emit(State) → UI
Cubit:        UI → method()   → Cubit → emit(State) → UI
```

Khi nào dùng **BLoC** thay **Cubit**?
- Khi logic phức tạp, cần xử lý nhiều loại event khác nhau
- Khi muốn log / trace từng event rõ ràng
- Cubit đủ dùng cho hầu hết các trường hợp

---

### BlocBuilder — Kết nối UI với Cubit

- Widget lắng nghe Cubit và **tự rebuild** khi state thay đổi
- Tương tự `StreamBuilder` nhưng chuyên cho BLoC/Cubit

```dart
BlocBuilder<CounterCubit, CounterState>(
  builder: (context, state) {
    // Gọi mỗi khi Cubit emit state mới
    return Text('${state.count}');
  },
)
```

### BlocProvider — Cung cấp Cubit cho Widget Tree

- Inject Cubit vào widget tree để các widget con có thể dùng
- `context.read<T>()` — lấy Cubit để gọi method (không rebuild)
- `context.watch<T>()` — lấy Cubit và subscribe (có rebuild)

```dart
BlocProvider.value(
  value: getIt<CounterCubit>(), // lấy từ GetIt
  child: MyWidget(),
)

// Trong widget con:
context.read<CounterCubit>().increment(); // gọi action
```

---

## Cấu trúc project

```
lib/
├── main.dart                      ← setupLocator() rồi runApp()
├── cubit/
│   ├── counter_state.dart         ← Định nghĩa CounterState {count, message}
│   └── counter_cubit.dart         ← Logic: increment, decrement, reset
├── di/
│   └── service_locator.dart       ← GetIt: đăng ký CounterCubit singleton
└── presentation/
    └── counter_page.dart          ← UI: BlocProvider + BlocBuilder
```

## Luồng dữ liệu

```
main()
  │
  ├── setupLocator()          → GetIt tạo CounterCubit, lưu vào container
  │
  └── runApp(MyApp)
        │
        └── CounterPage
              │
              ├── BlocProvider.value(getIt<CounterCubit>())
              │     → Inject Cubit vào widget tree
              │
              └── BlocBuilder<CounterCubit, CounterState>
                    → Lắng nghe state, rebuild khi count thay đổi
                    │
                    └── ElevatedButton.onPressed:
                          context.read<CounterCubit>().increment()
                          → Cubit xử lý → emit(newState) → BlocBuilder rebuild
```
