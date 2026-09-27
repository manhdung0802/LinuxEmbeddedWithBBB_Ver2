import 'package:flutter_bloc/flutter_bloc.dart';
import 'counter_state.dart';

// CUBIT: chứa logic xử lý — giống state machine
// Cubit extends Cubit<KiểuState>
// Đây là phiên bản đơn giản hơn BLoC (không cần Event)

class CounterCubit extends Cubit<CounterState> {
  // Khởi tạo với state mặc định
  CounterCubit() : super(const CounterState(count: 0, message: 'Start!'));

  // Các method = "actions" mà UI có thể gọi
  void increment() {
    emit(CounterState(
      count: state.count + 1,
      message: 'Tăng lên ${state.count + 1}',
    ));
    // emit() = phát ra state mới → UI tự rebuild
  }

  void decrement() {
    if (state.count == 0) {
      emit(CounterState(count: 0, message: 'Không thể giảm xuống 0!'));
      return;
    }
    emit(CounterState(
      count: state.count - 1,
      message: 'Giảm xuống ${state.count - 1}',
    ));
  }

  void reset() {
    emit(const CounterState(count: 0, message: 'Đã reset!'));
  }
}
