@TestOn('browser')
library;

import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_web_plugins/flutter_web_plugins.dart';
import 'package:flutter/services.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter_platform_interface.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter_web.dart';

class MockRegistrar extends Registrar {
  @override
  BinaryMessenger get messenger => throw UnimplementedError();

  @override
  void registerMessageHandler() {}
}

void main() {
  final initialPlatform = AppBadgeControlFlutterPlatform.instance;

  tearDown(() {
    AppBadgeControlFlutterPlatform.instance = initialPlatform;
  });

  test('Web plugin registers successfully', () {
    AppBadgeControlFlutterWeb.registerWith(MockRegistrar());
    expect(
      AppBadgeControlFlutterPlatform.instance,
      isInstanceOf<AppBadgeControlFlutterWeb>(),
    );
  });
}
