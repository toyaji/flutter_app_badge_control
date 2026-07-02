import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter_method_channel.dart';

void main() {
  MethodChannelAppBadgeControlFlutter platform =
      MethodChannelAppBadgeControlFlutter();
  const MethodChannel channel = MethodChannel('flutter_app_badge_control');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMethodCallHandler(channel, (MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMethodCallHandler(channel, null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
