import 'dart:js_interop';
import 'package:flutter/foundation.dart';

@JS('Notification.requestPermission')
external JSPromise? _jsRequestPermission();

Future<void> requestWebNotificationPermission() async {
  try {
    final promise = _jsRequestPermission();
    if (promise != null) {
      final result = await promise.toDart;
      final permissionStr = (result as JSString).toDart;
      debugPrint('Web notification permission: $permissionStr');
    }
  } catch (e) {
    debugPrint('Permission request failed: $e');
  }
}
