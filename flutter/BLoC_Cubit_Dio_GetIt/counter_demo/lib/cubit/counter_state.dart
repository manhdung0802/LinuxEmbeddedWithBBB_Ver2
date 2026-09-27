// STATE: mô tả các trạng thái có thể có của counter
// Cubit dùng class đơn giản thay vì sealed class như BLoC

class CounterState {
  final int count;
  final String message;

  const CounterState({required this.count, required this.message});
}
