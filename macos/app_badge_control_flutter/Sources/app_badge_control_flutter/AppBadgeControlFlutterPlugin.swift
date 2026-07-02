import Cocoa
import FlutterMacOS

public class AppBadgeControlFlutterPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_app_badge_control", binaryMessenger: registrar.messenger)
    let instance = AppBadgeControlFlutterPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    case "updateBadgeCount":
      if let count = call.arguments as? Int {
        NSApp.dockTile.badgeLabel = count > 0 ? String(count) : nil
        result(nil)
      } else if let countString = call.arguments as? String, let count = Int(countString) {
        NSApp.dockTile.badgeLabel = count > 0 ? String(count) : nil
        result(nil)
      } else {
        result(FlutterError(code: "INVALID_ARGUMENT", message: "Count must be an integer", details: nil))
      }
    case "removeBadge":
      NSApp.dockTile.badgeLabel = nil
      result(nil)
    case "isAppBadgeSupported":
      result(true)
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}
