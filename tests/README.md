# UI 自动化测试

`ui_tests.pro` / `ui_tests.cpp` 使用 Qt Test 验证页面导航、卡片与控件边界、温度上下限、独立风量、跨页状态、媒体控制、导航切换、倒车返回和异常状态，并生成六页及倒车/断连/低压截图。构建运行方法见 `doc/BUILD_AND_DEPLOY.md`。

`mock_can_publisher.py` 仍为注释占位，没有发送真实 CAN 报文。
