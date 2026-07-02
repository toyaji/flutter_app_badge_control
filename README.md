# app_badge_control_flutter

`app_badge_control_flutter` is a Flutter plugin for managing app icon badges. It allows you to update or remove the badge count on both iOS and Android devices.

## Why this fork exists

This package is a fork of [`flutter_app_badge_control`](https://github.com/funseek/flutter_app_badge_control) by Funseek, republished under a new name and maintained independently. The original repository was forked and republished because:

- **The upstream repository is no longer maintained.** No commits, releases, or reviews have landed there in a long time.
- **It doesn't support the Swift Package Manager (SPM).** iOS projects that have migrated off CocoaPods can't consume it. This fork adds SPM support while keeping CocoaPods working.
- **Issues and pull requests go unanswered.** Bug reports and contributions have no path to being reviewed or merged upstream.

Since publishing changes under the original name isn't possible, this fork is published to pub.dev as a new, independent package: `app_badge_control_flutter`. Ongoing maintenance, bug fixes, and feature requests will happen here.

## Migrating from `flutter_app_badge_control`

Migration only requires a rename — the plugin's behavior and method signatures are unchanged.

1. Update your dependency in `pubspec.yaml`:

   ```yaml
   dependencies:
     # remove
     # flutter_app_badge_control: ^0.0.3
     # add
     app_badge_control_flutter: ^0.1.0
   ```

2. Update your imports:

   ```dart
   // before
   import 'package:flutter_app_badge_control/flutter_app_badge_control.dart';
   // after
   import 'package:app_badge_control_flutter/app_badge_control_flutter.dart';
   ```

3. Rename the class you call, `FlutterAppBadgeControl` → `AppBadgeControlFlutter`. Every method name and its signature stayed the same:

   | Old (`flutter_app_badge_control`)              | New (`app_badge_control_flutter`)             |
   | ----------------------------------------------- | ---------------------------------------------- |
   | `FlutterAppBadgeControl.updateBadgeCount(count)` | `AppBadgeControlFlutter.updateBadgeCount(count)` |
   | `FlutterAppBadgeControl.removeBadge()`           | `AppBadgeControlFlutter.removeBadge()`           |
   | `FlutterAppBadgeControl.isAppBadgeSupported()`   | `AppBadgeControlFlutter.isAppBadgeSupported()`   |

No native (Android/iOS) configuration changes are required — the native method channel and platform code are unchanged.

## Overview

This plugin provides an easy way to control the badge count of your app's icon. It is compatible with both iOS and Android platforms. You can update the badge count, remove the badge, or check if badge support is available on the device.

This plugin is inspired by and based on the [flutter_app_badger](https://github.com/g123k/flutter_app_badger) plugin.

## Getting Started

### iOS

In order to use this plugin on iOS, you need to request permission for notifications.
For more information about requesting notification permissions, please refer to the [Apple Developer Documentation](https://developer.apple.com/documentation/usernotifications/asking_permission_to_use_notifications).

### Android

On Android, the plugin uses notification channels to manage app icon badges, as there is no official support for badge count updates. Calling `removeBadge()` will cancel all notifications, effectively removing the badge. Note that calling `updateBadgeCount()` on Android will not have any effect.

For more information about notification channels on Android, please visit the [Android Developer Documentation](https://developer.android.com/develop/ui/views/notifications/channels).

## Usage

To use the plugin, add the following import to your Dart file:

```dart
import 'package:app_badge_control_flutter/app_badge_control_flutter.dart';
```

### Check if App Badge is Supported

```dart
AppBadgeControlFlutter.isAppBadgeSupported().then((value) {
  debugPrint("isAppBadgeSupported: $value");
});
```

### Remove Badge

```dart
AppBadgeControlFlutter.removeBadge();
```

### Update Badge Count

```dart
AppBadgeControlFlutter.updateBadgeCount(1);
```

**Note:** This method will not have any effect on Android devices.
