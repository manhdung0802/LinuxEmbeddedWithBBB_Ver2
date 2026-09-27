import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import '../cubit/counter_cubit.dart';
import '../cubit/counter_state.dart';
import '../di/service_locator.dart';

class CounterPage extends StatelessWidget {
  const CounterPage({super.key});

  @override
  Widget build(BuildContext context) {
    // BlocProvider: cung cấp Cubit cho widget tree bên dưới
    // Lấy instance từ GetIt thay vì tạo mới
    return BlocProvider.value(
      value: getIt<CounterCubit>(), // ← GetIt cung cấp cùng 1 instance
      child: const _CounterView(),
    );
  }
}

class _CounterView extends StatelessWidget {
  const _CounterView();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('BLoC / Cubit / GetIt Demo')),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            // ── Phần 1: Giải thích ──────────────────────────────────────
            const Text('Kiến trúc:', style: TextStyle(fontWeight: FontWeight.bold)),
            const SizedBox(height: 4),
            const Text('GetIt  →  quản lý dependency (singleton)'),
            const Text('Cubit  →  xử lý logic + quản lý state'),
            const Text('BlocBuilder  →  lắng nghe state, rebuild UI'),
            const Divider(height: 40),

            // ── Phần 2: UI lắng nghe state ──────────────────────────────
            // BlocBuilder: tự rebuild khi Cubit emit state mới
            BlocBuilder<CounterCubit, CounterState>(
              builder: (context, state) {
                return Column(
                  children: [
                    Text(
                      '${state.count}',
                      style: const TextStyle(fontSize: 72, fontWeight: FontWeight.bold),
                    ),
                    Text(
                      state.message,
                      style: const TextStyle(fontSize: 16, color: Colors.grey),
                    ),
                  ],
                );
              },
            ),
            const SizedBox(height: 32),

            // ── Phần 3: Gọi action của Cubit ────────────────────────────
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                ElevatedButton(
                  onPressed: () => context.read<CounterCubit>().decrement(),
                  // context.read<T>() = lấy Cubit từ BlocProvider, gọi method
                  child: const Text('−'),
                ),
                const SizedBox(width: 16),
                ElevatedButton(
                  onPressed: () => context.read<CounterCubit>().reset(),
                  child: const Text('Reset'),
                ),
                const SizedBox(width: 16),
                ElevatedButton(
                  onPressed: () => context.read<CounterCubit>().increment(),
                  child: const Text('+'),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
