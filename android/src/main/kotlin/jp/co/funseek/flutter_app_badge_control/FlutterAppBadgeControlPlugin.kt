package jp.co.funseek.flutter_app_badge_control

import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.os.Build
import androidx.annotation.NonNull
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

/** FlutterAppBadgeControlPlugin */
class FlutterAppBadgeControlPlugin: FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel : MethodChannel
  private lateinit var context : Context

  companion object {
    private const val BADGE_CHANNEL_ID = "flutter_app_badge_control_channel"
    private const val BADGE_NOTIFICATION_ID = 1001
  }

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "flutter_app_badge_control")
    channel.setMethodCallHandler(this)
    context = flutterPluginBinding.applicationContext
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if (call.method == "getPlatformVersion") {
      result.success("Android ${android.os.Build.VERSION.RELEASE}")
    } else if (call.method == "updateBadgeCount") {
      val count = call.arguments as? Int
      if (count == null) {
        result.error("INVALID_ARGUMENT", "Invalid count value", null)
      } else {
        updateBadgeCount(count)
        result.success(null)
      }
    } else if (call.method == "removeBadge") {
      cancelBadgeNotification()
      result.success(null)
    } else if (call.method == "isAppBadgeSupported") {
      result.success(isAppBadgeSupported())
    } else {
      result.notImplemented()
    }
  }

  private fun updateBadgeCount(count: Int) {
    if (count <= 0) {
      cancelBadgeNotification()
      return
    }

    ensureBadgeChannel()

    val notification = NotificationCompat.Builder(context, BADGE_CHANNEL_ID)
      .setSmallIcon(context.applicationInfo.icon)
      .setPriority(NotificationCompat.PRIORITY_MIN)
      .setNumber(count)
      .setSilent(true)
      .setOngoing(true)
      .setAutoCancel(false)
      .build()

    NotificationManagerCompat.from(context).notify(BADGE_NOTIFICATION_ID, notification)
  }

  private fun cancelBadgeNotification() {
    NotificationManagerCompat.from(context).cancel(BADGE_NOTIFICATION_ID)
  }

  private fun ensureBadgeChannel() {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      val notificationManager = context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
      val existing = notificationManager.getNotificationChannel(BADGE_CHANNEL_ID)
      if (existing == null) {
        val channel = NotificationChannel(
          BADGE_CHANNEL_ID,
          "App badge",
          NotificationManager.IMPORTANCE_MIN
        )
        channel.setShowBadge(true)
        channel.setSound(null, null)
        channel.enableVibration(false)
        notificationManager.createNotificationChannel(channel)
      }
    }
  }

  private fun isAppBadgeSupported(): Boolean {
    if (!NotificationManagerCompat.from(context).areNotificationsEnabled()) {
      return false
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      val notificationManager = context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
      val existing = notificationManager.getNotificationChannel(BADGE_CHANNEL_ID)
      if (existing != null && existing.importance == NotificationManager.IMPORTANCE_NONE) {
        return false
      }
    }

    return true
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }
}
