// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "app_badge_control_flutter",
    platforms: [
        .iOS("12.0")
    ],
    products: [
        .library(name: "app-badge-control-flutter", targets: ["app_badge_control_flutter"])
    ],
    dependencies: [
        .package(name: "FlutterFramework", path: "../FlutterFramework")
    ],
    targets: [
        .target(
            name: "app_badge_control_flutter",
            dependencies: [
                .product(name: "FlutterFramework", package: "FlutterFramework")
            ]
        )
    ]
)
