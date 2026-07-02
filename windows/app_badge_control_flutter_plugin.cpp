#include "app_badge_control_flutter_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <string>

namespace app_badge_control_flutter {

// static
void AppBadgeControlFlutterPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "flutter_app_badge_control",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<AppBadgeControlFlutterPlugin>(registrar);

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

AppBadgeControlFlutterPlugin::AppBadgeControlFlutterPlugin(flutter::PluginRegistrarWindows *registrar)
    : registrar_(registrar) {
  InitializeTaskbarList();
}

AppBadgeControlFlutterPlugin::~AppBadgeControlFlutterPlugin() {
  CleanupIcon();
  if (taskbar_list_) {
    taskbar_list_->Release();
    taskbar_list_ = nullptr;
  }
}

void AppBadgeControlFlutterPlugin::InitializeTaskbarList() {
  HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&taskbar_list_));
  if (SUCCEEDED(hr)) {
    hr = taskbar_list_->HrInit();
    if (FAILED(hr)) {
      taskbar_list_->Release();
      taskbar_list_ = nullptr;
    }
  }
}

void AppBadgeControlFlutterPlugin::CleanupIcon() {
  if (current_overlay_icon_) {
    DestroyIcon(current_overlay_icon_);
    current_overlay_icon_ = nullptr;
  }
}

HICON AppBadgeControlFlutterPlugin::CreateBadgeIcon(int count) {
  int size = 16;
  HDC hdcScreen = GetDC(NULL);
  
  // 1. Color bitmap (32-bit DIB)
  BITMAPINFO bmi = {0};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = size;
  bmi.bmiHeader.biHeight = size;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  
  void* pvBits = nullptr;
  HBITMAP hbmColor = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
  
  HDC hdcColor = CreateCompatibleDC(hdcScreen);
  HGDIOBJ oldColorBmp = SelectObject(hdcColor, hbmColor);
  
  // Fill background with black
  RECT rect = {0, 0, size, size};
  HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
  FillRect(hdcColor, &rect, blackBrush);
  
  // Draw red circle
  HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
  HGDIOBJ oldBrush = SelectObject(hdcColor, redBrush);
  HPEN hNullPen = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ oldPen = SelectObject(hdcColor, hNullPen);
  
  Ellipse(hdcColor, 0, 0, size, size);
  
  SelectObject(hdcColor, oldBrush);
  DeleteObject(redBrush);
  SelectObject(hdcColor, oldPen);
  DeleteObject(hNullPen);
  
  // Draw number text
  SetTextColor(hdcColor, RGB(255, 255, 255));
  SetBkMode(hdcColor, TRANSPARENT);
  
  // Taskbar overlay icons conventionally show only one or two glyphs, so clamp
  // 3+ digit counts to "99+" to keep them legible in the small (16px) icon.
  std::wstring countStr = (count > 99) ? L"99+" : std::to_wstring(count);

  int fontHeight = (countStr.length() > 2) ? 8 : ((countStr.length() > 1) ? 10 : 12);
  HFONT hFont = CreateFontW(
      fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Arial"
  );
  HGDIOBJ oldFont = SelectObject(hdcColor, hFont);
  DrawTextW(hdcColor, countStr.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  
  SelectObject(hdcColor, oldFont);
  DeleteObject(hFont);
  
  SelectObject(hdcColor, oldColorBmp);
  DeleteDC(hdcColor);
  
  // 2. Mask bitmap (Monochrome, 1:Transparent, 0:Opaque)
  HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);
  HDC hdcMask = CreateCompatibleDC(hdcScreen);
  HGDIOBJ oldMaskBmp = SelectObject(hdcMask, hbmMask);
  
  // Mask background is white (transparent)
  FillRect(hdcMask, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
  
  // Draw black circle on mask (opaque area)
  HBRUSH maskBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
  SelectObject(hdcMask, maskBrush);
  hNullPen = CreatePen(PS_NULL, 0, 0);
  oldPen = SelectObject(hdcMask, hNullPen);
  
  Ellipse(hdcMask, 0, 0, size, size);
  
  SelectObject(hdcMask, oldPen);
  DeleteObject(hNullPen);
  
  SelectObject(hdcMask, oldMaskBmp);
  DeleteDC(hdcMask);
  
  ReleaseDC(NULL, hdcScreen);
  
  // 3. Create HICON from color and mask bitmaps
  ICONINFO ii = {0};
  ii.fIcon = TRUE;
  ii.hbmMask = hbmMask;
  ii.hbmColor = hbmColor;
  
  HICON hIcon = CreateIconIndirect(&ii);
  
  DeleteObject(hbmColor);
  DeleteObject(hbmMask);
  
  return hIcon;
}

void AppBadgeControlFlutterPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name().compare("updateBadgeCount") == 0) {
    int count = 0;
    const auto* arguments = method_call.arguments();
    if (arguments) {
      if (std::holds_alternative<int32_t>(*arguments)) {
        count = std::get<int32_t>(*arguments);
      } else if (std::holds_alternative<int64_t>(*arguments)) {
        count = static_cast<int>(std::get<int64_t>(*arguments));
      } else if (std::holds_alternative<std::string>(*arguments)) {
        try {
          count = std::stoi(std::get<std::string>(*arguments));
        } catch (...) {
          result->Error("INVALID_ARGUMENT", "Count string is not a valid integer");
          return;
        }
      } else {
        result->Error("INVALID_ARGUMENT", "Count must be an integer");
        return;
      }
    }

    HWND hwnd = registrar_->GetView() ? registrar_->GetView()->GetNativeWindow() : nullptr;
    if (taskbar_list_ && hwnd) {
      CleanupIcon();
      if (count <= 0) {
        // Convention: 0 (or less) means "no badge", matching iOS/Web/macOS.
        taskbar_list_->SetOverlayIcon(hwnd, NULL, NULL);
      } else {
        current_overlay_icon_ = CreateBadgeIcon(count);
        taskbar_list_->SetOverlayIcon(hwnd, current_overlay_icon_, L"Notifications");
      }
      result->Success();
    } else {
      result->Error("UNAVAILABLE", "Taskbar interface or Window Handle is unavailable");
    }
  } else if (method_call.method_name().compare("removeBadge") == 0) {
    HWND hwnd = registrar_->GetView() ? registrar_->GetView()->GetNativeWindow() : nullptr;
    if (taskbar_list_ && hwnd) {
      CleanupIcon();
      taskbar_list_->SetOverlayIcon(hwnd, NULL, NULL);
      result->Success();
    } else {
      result->Error("UNAVAILABLE", "Taskbar interface or Window Handle is unavailable");
    }
  } else if (method_call.method_name().compare("isAppBadgeSupported") == 0) {
    HWND hwnd = registrar_->GetView() ? registrar_->GetView()->GetNativeWindow() : nullptr;
    result->Success(flutter::EncodableValue(taskbar_list_ != nullptr && hwnd != nullptr));
  } else {
    result->NotImplemented();
  }
}

}  // namespace app_badge_control_flutter
