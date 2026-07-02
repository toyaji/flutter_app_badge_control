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

This plugin provides an easy way to control the badge count of your app's icon. It is compatible with iOS, Android, macOS, Windows, and Web. You can update the badge count, remove the badge, or check if badge support is available on the device.

This plugin is inspired by and based on the [flutter_app_badger](https://github.com/g123k/flutter_app_badger) plugin.

## Platform Behavior

| Platform | Support Status | Implementation Details |
| :--- | :--- | :--- |
| **iOS** | Supported | Uses native iOS user notification settings. Requires permission request. |
| **Android** | Partial | Implemented via notification channels since Android has no official standalone badge API. Calling `removeBadge()` cancels notifications. |
| **macOS** | Supported | Updates the Dock icon badge using `NSApp.dockTile.badgeLabel`. |
| **Windows** | Supported | Implemented via **Taskbar Overlay Icons** using Win32 `ITaskbarList3::SetOverlayIcon`. Displays a red circle with the badge count overlaying the app's taskbar icon. |
| **Web** | Supported | Uses the Web Badging API (`navigator.setAppBadge`). Requires a **secure context** (HTTPS or `localhost`) and a **Chromium-based browser** (Chrome/Edge); Firefox and Safari are not supported. Mostly visible when the app is installed as a Progressive Web App (PWA). |

## Getting Started

### iOS

In order to use this plugin on iOS, you need to request permission for notifications.
For more information about requesting notification permissions, please refer to the [Apple Developer Documentation](https://developer.apple.com/documentation/usernotifications/asking_permission_to_use_notifications).

### Android

On Android, the plugin uses notification channels to manage app icon badges, as there is no official support for badge count updates. Calling `removeBadge()` will cancel all notifications, effectively removing the badge. Note that calling `updateBadgeCount()` on Android will not have any effect.

For more information about notification channels on Android, please visit the [Android Developer Documentation](https://developer.android.com/develop/ui/views/notifications/channels).

### macOS

No additional setup or permissions are required. The plugin updates the Dock tile icon badge natively using `NSApp.dockTile.badgeLabel`.

### Windows

No additional setup or permissions are required. Since standard Win32 desktop apps do not support native icon badges, the plugin displays the badge count as a **Taskbar Overlay Icon** on the application's taskbar window.

### Web

The Web Badging API (`navigator.setAppBadge`) relies on your app being installed as a **Progressive Web App (PWA)** and having **Notification Permissions**.

> **Requirements & limitations:**
> - **Secure context required.** The API is only exposed over **HTTPS** (or `localhost`). When served over plain HTTP, `navigator.setAppBadge` is undefined and `isAppBadgeSupported()` silently returns `false`.
> - **Chromium-only.** The Badging API is currently supported by **Chrome and Edge (Chromium)** only. **Firefox and Safari do not support it**, so `isAppBadgeSupported()` returns `false` there.

1. **Request Notification Permission**: You must request user permission before the badge can be shown on the dock/taskbar icon. Here is an example implementation using `dart:js_interop` (Dart 3.x compatible):

```dart
import 'package:flutter/foundation.dart' show kIsWeb;
import 'dart:js_interop';

@JS('Notification.requestPermission')
external JSPromise? _jsRequestPermission();

Future<void> requestWebNotificationPermission() async {
  if (kIsWeb) {
    try {
      final promise = _jsRequestPermission();
      if (promise != null) {
        final result = await promise.toDart;
        final permissionStr = (result as JSString).toDart;
        debugPrint('Notification Permission status: $permissionStr');
      }
    } catch (e) {
      debugPrint('Failed to request permission: $e');
    }
  }
}
```

2. **Run as PWA**: The application must be installed as a Progressive Web App (PWA) (via Chrome's/Edge's "Install" icon). The badge will be rendered on the standalone PWA's app launcher/dock icon rather than standard browser tabs.

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

## Troubleshooting

### Web (PWA) Badge Not Showing Up
Web Badging API relies heavily on browser permissions and operating system integration. If the badge doesn't appear on the PWA icon in macOS Dock:
1. **Enable Notification Permissions**: Click the icon (lock or settings slider) on the left of the address bar inside your installed PWA window, and ensure **Notifications** are set to **Allow**.
2. **Enable macOS System Settings**: Go to **System Settings** -> **Notifications** -> Find your PWA app in the list -> ensure **Badge app icon** is toggled **On**.
3. **Standalone Mode Required**: The app icon badge will not be displayed on normal browser tabs. The web app must be installed and running as a Progressive Web App (PWA).

### MissingPluginException on Web
If you encounter `MissingPluginException` while calling badge control APIs on Web:
* Web plugin registrations are dynamically resolved during the initial build. If you have modified dependencies or platforms in `pubspec.yaml`, you **must restart** the `flutter run` process completely. Simply hot reloading or hot restarting will not apply the web plugin register hook.
